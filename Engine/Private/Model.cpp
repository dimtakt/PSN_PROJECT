#include "Model.h"

#include "Mesh.h"
#include "Bone.h"
#include "MeshMaterial.h"
#include "Animation.h"
#include "Channel.h"

#include <fstream>

CModel::CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CComponent{ pDevice ,pContext }
{
}

CModel::CModel(const CModel& Prototype)
    : CComponent{ Prototype }
    , m_iNumMeshes{ Prototype.m_iNumMeshes }
    , m_Meshes{ Prototype.m_Meshes }
    , m_eModelType{ Prototype.m_eModelType }
    , m_PreTransformMatrix{ Prototype.m_PreTransformMatrix }
    , m_iNumMaterials{ Prototype.m_iNumMaterials }
    , m_Materials{ Prototype.m_Materials }
    , m_iNumAnimations{ Prototype.m_iNumAnimations }
{
    for (auto& pPrototypeAnimation : Prototype.m_Animations)
        m_Animations.push_back(pPrototypeAnimation->Clone());

    for (auto& pPrototypeBone : Prototype.m_Bones)
        m_Bones.push_back(pPrototypeBone->Clone());

    for (auto& pMesh : m_Meshes)
        Safe_AddRef(pMesh);

    for (auto& pMaterial : m_Materials)
        Safe_AddRef(pMaterial);
}

_float4x4* CModel::Get_BoneMatrix(const _char* pBoneName)
{
    auto    iter = find_if(m_Bones.begin(), m_Bones.end(), [&](CBone* pBone) {
        if (true == pBone->Compare_Name(pBoneName))
            return true;
        return false;
        });

    if (iter == m_Bones.end())
        return nullptr;

    return (*iter)->Get_CombinedTransformationMatrixPtr();
}

HRESULT CModel::Initialize_Prototype(MODELTYPE eModelType, const _char* pModelFilePath, _fmatrix PreTransformMatrix)
{
    /* aiProcess_PreTransformVertices : 각각의 메시를 붙여야할 위치에 적절히 배치한다. */
    /* 배치 : 각 메시의 정점들을 배치를 위한 임의의 행렬과 곱하여 로드한다. */

    m_eModelType = eModelType;

    XMStoreFloat4x4(&m_PreTransformMatrix, PreTransformMatrix);

    _uint   iFlag = { aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast };

    if (MODELTYPE::NONANIM == m_eModelType)
        iFlag |= aiProcess_PreTransformVertices;

    // 파일 확장자 확인, 모델타입 지정.
    _char   szExt[MAX_PATH] = {};
    _splitpath_s(pModelFilePath, nullptr, 0, nullptr, 0, nullptr, 0, szExt, MAX_PATH);

    if (!strcmp(szExt, ".fbx"))
        m_eFileType = FILETYPE::FBX;
    else if (!strcmp(szExt, ".datmodel"))
        m_eFileType = FILETYPE::DATMODEL;
    else
    {
        MessageBoxW(NULL, L"Wrong Type", L"지원하지 않는 확장자 로드 시도. 확장자는 \".fbx\" 또는 \".datmodel\"만 가능. \nModel::Initialize_Prototype()", MB_OK);
        return E_FAIL;
    }


    // fbx 인지, datmodel 인지에 따라 다르게 읽어옴.
    // 만약 fbx 라면, export 용 로컬 함수에 데이터 또한 저장.
    // (어차피 실 게임에서는 datmodel 을 사용할 것이기에, 최적화 문제는 괜찮을 듯?)
    if (m_eFileType == FILETYPE::FBX)
    {
        m_pAIScene = m_Importer.ReadFile(pModelFilePath, iFlag);

        if (nullptr == m_pAIScene)
            return E_FAIL;

        // for export
        m_BinModel.szModelName = m_pAIScene->mName;
        m_BinModel.eAnimtype = eModelType;
        XMStoreFloat4x4(&m_BinModel.matPreTransformMatrix, PreTransformMatrix);

        if (FAILED(Ready_Bones(m_pAIScene->mRootNode, -1)))
            return E_FAIL;

        if (FAILED(Ready_Meshes()))                        
            return E_FAIL;

        if (FAILED(Ready_Materials(pModelFilePath)))       
            return E_FAIL;

        if (FAILED(Ready_Animations()))                    
            return E_FAIL;
    }
    else if (m_eFileType == FILETYPE::DATMODEL)
    {
        // 1. 파일을 로드하여 로컬 변수에 저장. 로드는 따로 함수 만들기?
        // 2. fbx처럼 각 항목마다 로드 진행
        // 3. 
        // ksta : binary
        
        Import_FromBinary(pModelFilePath, PreTransformMatrix);

        if (FAILED(Ready_Bones(nullptr, 0)))            // done..
            return E_FAIL;

        if (FAILED(Ready_Meshes()))                     // done..
            return E_FAIL;

        if (FAILED(Ready_Materials(pModelFilePath)))    // do..
            return E_FAIL;

        if (FAILED(Ready_Animations()))                 // do..
            return E_FAIL;

    }





    return S_OK;
}

HRESULT CModel::Initialize(void* pArg)
{
    return S_OK;
}

HRESULT CModel::Bind_Materials(class CShader* pShader, const _char* pConstantName, _uint iMeshIndex, aiTextureType eTextureType, _uint iIndex)
{
    if (iMeshIndex >= m_iNumMeshes)
        return E_FAIL;

    _uint       iMaterialIndex = m_Meshes[iMeshIndex]->Get_MaterialIndex();

    if (m_iNumMaterials <= iMaterialIndex)
        return E_FAIL;

    return m_Materials[iMaterialIndex]->Bind_Resources(pShader, pConstantName, eTextureType, iIndex);
}

HRESULT CModel::Bind_BoneMatrices(CShader* pShader, const _char* pConstantName, _uint iMeshIndex)
{
    if (iMeshIndex >= m_iNumMeshes)
        return E_FAIL;

    return m_Meshes[iMeshIndex]->Bind_BoneMatrices(pShader, pConstantName, m_Bones);
}

_bool CModel::Play_Animation(_float fTimeDelta)
{
    m_isFinished = false;

    /* 현재 시간에 맞는 뼈의 상태대로 특정 뼈들의 TransformationMatrix를 갱신해준다. */
    
#pragma region logic comment (animChange transition blending)

    // 여기서 시간 경과에 따른 fTranslationTime 의 비율을 계산한 뒤
    // 인자로 넘겨주어 갱신되도록

    // 언제 블렌드가 이루어져야 하는가
    // 1. 애니메이션 변경 직후
    // 2. 변경된 지 0.2s 이내동안

    // fTimeDelta 에 기반하여 남은 시간 이용, 비율 계산 




    // if ( 애니메이션이 막 변경됐다면 )
    //   { 경과 시간 계산 시작. 즉, 타이머 진행중을 뜻하는 변수의 On }

    // 매 업데이트마다
    // 
    // 1. fDeltaTime 을 이용하여 경과 시간 계속 더함
    // 
    // 2. 남은시간 대비 경과시간에 비례해서 애닌메이션 블렌딩 << 인자로 비율 넘겨주어 내부에서 블렌딩되도록
    // >> 블렌딩 방식? 시간 경과량만큼 기존 뼈대 위치 : 적용 뼈대 위치 비율 산정하여 적용
    // >> 예시) 1초 중 0.1초만 지났다면       Anim : Existing =   1/10    : 9/10,
    //          거기서 0.2초가 더 지났다면                        2/9     : 7/9,
    //          거기서 0.5초가 더 지났다면                        5/7     : 2/7   ...
    // >> 왜 이렇게 하느냐? 프레임 저하 상황에서도 시간 기반 변수로 일정하게 작동시키기 위함

    // if ( 만약 경과시간이 최대시간을 넘겼다면 )
    //   { 경과 시간 계산 종료. }

#pragma endregion

#pragma region logic comment (loop blending)

    // 단, 애니메이션의 전/후가 같다면 blending 시점을 반 앞으로 당겨서 앞뒤 연결이 자연스럽게 되어야 함

#pragma endregion


    // 필요 지역변수
    _float fAnimBlendRatio = 1.f;
    _float fBlendLeftTime = {};
    m_isAnimChanged = (m_isAnimChanged)? true : m_Animations[m_iCurrentAnimIndex]->Get_isFinishedLoop();

    _bool isSameAnim = m_iCurrentAnimIndex == m_iPrevAnimIndex;

    // 블렌딩 트랜지션 시작 조건. 애니메이션이 바뀌었고, 블렌딩 진행중이 아닐 때 활성화
    if (m_isAnimChanged && !m_isDoingTransition)
        m_isDoingTransition = true;

    // 경과시간 갱신 및 블렌딩 ratio 계산
    if (m_isDoingTransition)
    {
        m_fAnimElapsedTime += fTimeDelta;
        fBlendLeftTime = m_fTranslationTime - m_fAnimElapsedTime;

        //fAnimBlendRatio = fTimeDelta / fBlendLeftTime;            // 이게 적용돼야 할 신규 Anim 가중치 (수정 전 백업 8/25 7:54)
        fAnimBlendRatio = m_fAnimElapsedTime / m_fTranslationTime;  // 이게 적용돼야 할 신규 Anim 가중치


        // clamping
        if      (fAnimBlendRatio > 1)       fAnimBlendRatio = 1;
        else if (fAnimBlendRatio < 0)       fAnimBlendRatio = 0;
        
        // 종료 조건
        if (fBlendLeftTime <= 0)
        {
            m_isDoingTransition = false;
            m_fAnimElapsedTime = 0.f;
        }
    }


    //m_Animations[m_iCurrentAnimIndex]->Update_TransformationMatrices(m_Bones, m_isLoop, &m_isFinished, fTimeDelta, fAnimBlendRatio);

    CChannel::CHANNEL_UPD_DESC pDesc = {};
    pDesc.fAnimDuration = m_Animations[m_iCurrentAnimIndex]->Get_Duration();
    pDesc.fTransitionTime = m_fTranslationTime;
    pDesc.fTickPerSecond = m_Animations[m_iCurrentAnimIndex]->Get_TickPerSecond();

    // 애니메이션 업데이트
    if (m_isDoingTransition)    // 전환 중이면 두 개 애니메이션을 모두 업데이트. 다만 같은 애니메이션 반복 시 문제 발생
    {

        // 문제..?
        // 위쪽 함수가 잠깐 실행되어 본 값을 이전 애니메이션으로 변경
        // 뒤쪽 함수가 그 뒤 실행되어 본 값을 blend를 통해 이전 애니메이션으로부터 변환되듯이 변경
        // 이게 매 loop마다 반복..
        //
        // 이전 애니메이션과 이후 애니메이션이 독립적으로 진행되는데
        // 이를 blend시킬 방법을 찾아야 함
        //
        // 이전 건 1.f 로 주고
        // 그럼 뼈 반영됐을테니까 그거 기반으로 그냥 뒤에꺼 fAnimBlendRatio 로 두면 되는 것 아닌지?

        if (!isSameAnim)        // 전환 간 애니메이션이 다를 때
        {
            if (m_iPrevAnimIndex != UINT_MAX)
                m_Animations[m_iPrevAnimIndex]->Update_TransformationMatrices(m_Bones, m_isLoop, &m_isFinished, fTimeDelta, 1.f);              // 이전 애니메이션은 full weight로

            m_Animations[m_iCurrentAnimIndex]->Update_TransformationMatrices(m_Bones, m_isLoop, &m_isFinished, fTimeDelta, fAnimBlendRatio);   // 새 애니메이션은 fAnimBlendRatio만큼
        }
        else                    // 전환 간 애니메이션이 같을 때
        {
            m_Animations[m_iCurrentAnimIndex]->Update_TransformationMatrices(m_Bones, m_isLoop, &m_isFinished, fTimeDelta, fAnimBlendRatio, isSameAnim, &pDesc);
        }
        //std::cout << "[CModel::Play_Animation] Playing Blend Anim.. (NewAnim BlendRatio : " << fAnimBlendRatio << ")" << std::endl;
    }
    else                        // 전환 중이 아니면 현재 애니메이션만
    {
        m_Animations[m_iCurrentAnimIndex]->Update_TransformationMatrices(m_Bones, m_isLoop, &m_isFinished, fTimeDelta, fAnimBlendRatio, isSameAnim, &pDesc);
        //std::cout << "[CModel::Play_Animation] Playing Cur Anim.." << std::endl;
    }



    /* 바꿔야할 뼈들의 Transformation행렬이 갱신되었다면, 정점들에게 직접 전달돼야할 CombindTransformationMatrix를 만들어준다. */
    for (auto& pBone : m_Bones)
    {
        pBone->Update_CombinedTransformationMatrix(m_PreTransformMatrix, m_Bones);
    }


    // 이전의 애니메이션이 남아있어, 신규 애니메이션 loop 시 영향을 받는 것을 막기 위함
    if (m_Animations[m_iCurrentAnimIndex]->Get_isFinishedLoop())
        m_iPrevAnimIndex = m_iCurrentAnimIndex;


    return m_isFinished;
}

HRESULT CModel::Export_ToBinary(_wstring* strSavePath)
{
    ofstream ofs(strSavePath->c_str(), ios::binary);

    if (ofs.is_open())
    {
        ofs.write(reinterpret_cast<const char*>(&m_BinModel.szModelName), sizeof(aiString));
        ofs.write(reinterpret_cast<const char*>(&m_BinModel.eAnimtype), sizeof(MODELTYPE));
        ofs.write(reinterpret_cast<const char*>(&m_BinModel.matPreTransformMatrix), sizeof(_float4x4));

        ofs.write(reinterpret_cast<const char*>(&m_BinModel.iNumBones), sizeof(_uint));
        ofs.write(reinterpret_cast<const char*>(&m_BinModel.iNumMeshes), sizeof(_uint));
        ofs.write(reinterpret_cast<const char*>(&m_BinModel.iNumMaterials), sizeof(_uint));
        ofs.write(reinterpret_cast<const char*>(&m_BinModel.iNumAnimations), sizeof(_uint));

        // Bone 저장
        {
            _uint boneCount = static_cast<_uint>(m_BinModel.vecBones.size());
            ofs.write(reinterpret_cast<const char*>(&boneCount), sizeof(_uint));

            for (const auto& bone : m_BinModel.vecBones)
            {
                ofs.write(reinterpret_cast<const char*>(&bone.szBoneName), sizeof(aiString));
                ofs.write(reinterpret_cast<const char*>(&bone.matTransformation), sizeof(_float4x4));
                ofs.write(reinterpret_cast<const char*>(&bone.iNumChildren), sizeof(_uint));
                ofs.write(reinterpret_cast<const char*>(&bone.iParentBoneIndex), sizeof(_uint));
                ofs.write(reinterpret_cast<const char*>(&bone.matOffset), sizeof(_float4x4));
            }
        }

        // Mesh 저장
        {
            _uint meshCount = static_cast<_uint>(m_BinModel.vecMeshes.size());
            ofs.write(reinterpret_cast<const char*>(&meshCount), sizeof(_uint));

            for (const auto& mesh : m_BinModel.vecMeshes)
            {
                ofs.write(reinterpret_cast<const char*>(&mesh.szMeshName), sizeof(aiString));
                ofs.write(reinterpret_cast<const char*>(&mesh.iMaterialIndex), sizeof(_uint));
                ofs.write(reinterpret_cast<const char*>(&mesh.iNumVertices), sizeof(_uint));
                ofs.write(reinterpret_cast<const char*>(&mesh.iVertexStride), sizeof(_uint));
                ofs.write(reinterpret_cast<const char*>(&mesh.iNumIndices), sizeof(_uint));
                ofs.write(reinterpret_cast<const char*>(&mesh.iNumFaces), sizeof(_uint));

                ofs.write(reinterpret_cast<const char*>(&mesh.iNumUsingBones), sizeof(_uint));


                _uint faceCount = static_cast<_uint>(mesh.vecFaces.size());
                ofs.write(reinterpret_cast<const char*>(&faceCount), sizeof(_uint));
                if (faceCount > 0)
                    ofs.write(reinterpret_cast<const char*>(mesh.vecFaces.data()), sizeof(MeshFace) * faceCount);

                _uint boneIdxCount = static_cast<_uint>(mesh.vecUsingBonesIndices.size());
                ofs.write(reinterpret_cast<const char*>(&boneIdxCount), sizeof(_uint));
                if (boneIdxCount > 0)
                    ofs.write(reinterpret_cast<const char*>(mesh.vecUsingBonesIndices.data()), sizeof(_uint) * boneIdxCount);


                // vecNonAnimVertices 저장
                _uint nonAnimCount = static_cast<_uint>(mesh.vecNonAnimVertices.size());
                ofs.write(reinterpret_cast<const char*>(&nonAnimCount), sizeof(_uint));
                if (nonAnimCount > 0)
                    ofs.write(reinterpret_cast<const char*>(mesh.vecNonAnimVertices.data()), sizeof(VTXMESH) * nonAnimCount);

                // vecAnimVertices 저장
                _uint animCount = static_cast<_uint>(mesh.vecAnimVertices.size());
                ofs.write(reinterpret_cast<const char*>(&animCount), sizeof(_uint));
                if (animCount > 0)
                    ofs.write(reinterpret_cast<const char*>(mesh.vecAnimVertices.data()), sizeof(VTXANIMMESH) * animCount);

            }
        }

        // Material 저장
        {
            _uint matCount = static_cast<_uint>(m_BinModel.vecMaterials.size());
            ofs.write(reinterpret_cast<const char*>(&matCount), sizeof(_uint));

            for (const auto& mat : m_BinModel.vecMaterials)
            {
                ofs.write(reinterpret_cast<const char*>(&mat.szMaterialName), sizeof(aiString));
                ofs.write(reinterpret_cast<const char*>(&mat.iMaterialIndex), sizeof(_uint));
                ofs.write(reinterpret_cast<const char*>(&mat.iNumTextures), sizeof(_uint));

                _uint texCount = static_cast<_uint>(mat.vecTexturePaths.size());
                ofs.write(reinterpret_cast<const char*>(&texCount), sizeof(_uint));
                for (const auto& texPath : mat.vecTexturePaths)
                {
                    ofs.write(reinterpret_cast<const char*>(&texPath.first), sizeof(aiTextureType));
                    ofs.write(reinterpret_cast<const char*>(&texPath.second), sizeof(aiString));
                }
            }
        }

        // Animation 저장
        if (m_BinModel.eAnimtype == MODELTYPE::ANIM)
        {

            _uint animCount = static_cast<_uint>(m_BinModel.vecAiAnimations.size());
            ofs.write(reinterpret_cast<const char*>(&animCount), sizeof(_uint));

            for (const auto& anim : m_BinModel.vecAiAnimations)
            {
                ofs.write(reinterpret_cast<const char*>(&anim.szAnimName), sizeof(aiString));
                ofs.write(reinterpret_cast<const char*>(&anim.fDuration), sizeof(float));
                ofs.write(reinterpret_cast<const char*>(&anim.fTicksPerSecond), sizeof(float));
                ofs.write(reinterpret_cast<const char*>(&anim.iNumChannels), sizeof(_uint));

                _uint channelCount = static_cast<_uint>(anim.vecChannels.size());
                ofs.write(reinterpret_cast<const char*>(&channelCount), sizeof(_uint));

                for (const auto& channel : anim.vecChannels)
                {
                    ofs.write(reinterpret_cast<const char*>(&channel.szChannelName), sizeof(aiString));
                    ofs.write(reinterpret_cast<const char*>(&channel.iBoneIndex), sizeof(_uint));
                    ofs.write(reinterpret_cast<const char*>(&channel.iNumPosKeys), sizeof(_uint));
                    ofs.write(reinterpret_cast<const char*>(&channel.iNumRotKeys), sizeof(_uint));
                    ofs.write(reinterpret_cast<const char*>(&channel.iNumScaKeys), sizeof(_uint));
                    ofs.write(reinterpret_cast<const char*>(&channel.iNumKeyFrames), sizeof(_uint));

                    _uint keyCount = static_cast<_uint>(channel.vecKeyFrame.size());
                    ofs.write(reinterpret_cast<const char*>(&keyCount), sizeof(_uint));
                    if (keyCount > 0)
                        ofs.write(reinterpret_cast<const char*>(channel.vecKeyFrame.data()), sizeof(KEYFRAME) * keyCount);
                }
            }
        }

        ofs.close();
    }




    return S_OK;
}

HRESULT CModel::Import_FromBinary(const _char* pModelFilePath, _fmatrix PreTransformMatrix)
{
    ifstream ifs(pModelFilePath, ios::binary);
    if (!ifs.is_open())
        return E_FAIL;

    // 최상위 기본 정보
    ifs.read(reinterpret_cast<char*>(&m_BinModel.szModelName), sizeof(aiString));
    ifs.read(reinterpret_cast<char*>(&m_BinModel.eAnimtype), sizeof(MODELTYPE));
    ifs.read(reinterpret_cast<char*>(&m_BinModel.matPreTransformMatrix), sizeof(_float4x4));
    XMStoreFloat4x4(&m_BinModel.matPreTransformMatrix, PreTransformMatrix);                     // 외부 입력으로 교체시.

    ifs.read(reinterpret_cast<char*>(&m_BinModel.iNumBones), sizeof(_uint));
    ifs.read(reinterpret_cast<char*>(&m_BinModel.iNumMeshes), sizeof(_uint));
    ifs.read(reinterpret_cast<char*>(&m_BinModel.iNumMaterials), sizeof(_uint));
    ifs.read(reinterpret_cast<char*>(&m_BinModel.iNumAnimations), sizeof(_uint));

    // Bone 로드
    {
        _uint boneCount = 0;
        ifs.read(reinterpret_cast<char*>(&boneCount), sizeof(_uint));
        m_BinModel.vecBones.resize(boneCount);

        for (_uint i = 0; i < boneCount; ++i)
        {
            ifs.read(reinterpret_cast<char*>(&m_BinModel.vecBones[i].szBoneName), sizeof(aiString));
            ifs.read(reinterpret_cast<char*>(&m_BinModel.vecBones[i].matTransformation), sizeof(_float4x4));
            ifs.read(reinterpret_cast<char*>(&m_BinModel.vecBones[i].iNumChildren), sizeof(_uint));
            ifs.read(reinterpret_cast<char*>(&m_BinModel.vecBones[i].iParentBoneIndex), sizeof(_uint));
            ifs.read(reinterpret_cast<char*>(&m_BinModel.vecBones[i].matOffset), sizeof(_float4x4));
        }
    }

    // Mesh 로드
    {
        _uint meshCount = 0;
        ifs.read(reinterpret_cast<char*>(&meshCount), sizeof(_uint));
        m_BinModel.vecMeshes.resize(meshCount);

        for (_uint i = 0; i < meshCount; ++i)
        {
            auto& mesh = m_BinModel.vecMeshes[i];

            ifs.read(reinterpret_cast<char*>(&mesh.szMeshName), sizeof(aiString));
            ifs.read(reinterpret_cast<char*>(&mesh.iMaterialIndex), sizeof(_uint));
            ifs.read(reinterpret_cast<char*>(&mesh.iNumVertices), sizeof(_uint));
            ifs.read(reinterpret_cast<char*>(&mesh.iVertexStride), sizeof(_uint));
            ifs.read(reinterpret_cast<char*>(&mesh.iNumIndices), sizeof(_uint));
            ifs.read(reinterpret_cast<char*>(&mesh.iNumFaces), sizeof(_uint));

            ifs.read(reinterpret_cast<char*>(&mesh.iNumUsingBones), sizeof(_uint));

            _uint faceCount = 0;
            ifs.read(reinterpret_cast<char*>(&faceCount), sizeof(_uint));
            if (faceCount > 0)
            {
                mesh.vecFaces.resize(faceCount);
                ifs.read(reinterpret_cast<char*>(mesh.vecFaces.data()), sizeof(MeshFace) * faceCount);
            }

            _uint boneIdxCount = 0;
            ifs.read(reinterpret_cast<char*>(&boneIdxCount), sizeof(_uint));
            if (boneIdxCount > 0)
            {
                mesh.vecUsingBonesIndices.resize(boneIdxCount);
                ifs.read(reinterpret_cast<char*>(mesh.vecUsingBonesIndices.data()), sizeof(_uint) * boneIdxCount);
            }

            // vecNonAnimVertices 로드
            _uint nonAnimCount = 0;
            ifs.read(reinterpret_cast<char*>(&nonAnimCount), sizeof(_uint));
            if (nonAnimCount > 0)
            {
                mesh.vecNonAnimVertices.resize(nonAnimCount);
                ifs.read(reinterpret_cast<char*>(mesh.vecNonAnimVertices.data()), sizeof(VTXMESH) * nonAnimCount);
            }

            // vecAnimVertices 로드
            _uint animCount = 0;
            ifs.read(reinterpret_cast<char*>(&animCount), sizeof(_uint));
            if (animCount > 0)
            {
                mesh.vecAnimVertices.resize(animCount);
                ifs.read(reinterpret_cast<char*>(mesh.vecAnimVertices.data()), sizeof(VTXANIMMESH) * animCount);
            }

        }
    }

    // Material 로드
    {
        _uint matCount = 0;
        ifs.read(reinterpret_cast<char*>(&matCount), sizeof(_uint));
        m_BinModel.vecMaterials.resize(matCount);

        for (_uint i = 0; i < matCount; ++i)
        {
            auto& mat = m_BinModel.vecMaterials[i];

            ifs.read(reinterpret_cast<char*>(&mat.szMaterialName), sizeof(aiString));
            ifs.read(reinterpret_cast<char*>(&mat.iMaterialIndex), sizeof(_uint));
            ifs.read(reinterpret_cast<char*>(&mat.iNumTextures), sizeof(_uint));

            _uint texCount = 0;
            ifs.read(reinterpret_cast<char*>(&texCount), sizeof(_uint));
            mat.vecTexturePaths.resize(texCount);

            for (_uint j = 0; j < texCount; ++j)
            {
                ifs.read(reinterpret_cast<char*>(&mat.vecTexturePaths[j].first), sizeof(aiTextureType));   // 텍스처 타입
                ifs.read(reinterpret_cast<char*>(&mat.vecTexturePaths[j].second), sizeof(aiString));       // 텍스처 경로
            }
        }
    }

    // Animation 로드
    if (m_BinModel.eAnimtype == MODELTYPE::ANIM)
    {
        _uint animCount = 0;
        ifs.read(reinterpret_cast<char*>(&animCount), sizeof(_uint));
        m_BinModel.vecAiAnimations.resize(animCount);

        for (_uint i = 0; i < animCount; ++i)
        {
            auto& anim = m_BinModel.vecAiAnimations[i];

            ifs.read(reinterpret_cast<char*>(&anim.szAnimName), sizeof(aiString));
            ifs.read(reinterpret_cast<char*>(&anim.fDuration), sizeof(float));
            ifs.read(reinterpret_cast<char*>(&anim.fTicksPerSecond), sizeof(float));
            ifs.read(reinterpret_cast<char*>(&anim.iNumChannels), sizeof(_uint));

            _uint channelCount = 0;
            ifs.read(reinterpret_cast<char*>(&channelCount), sizeof(_uint));
            anim.vecChannels.resize(channelCount);

            for (_uint j = 0; j < channelCount; ++j)
            {
                auto& channel = anim.vecChannels[j];

                ifs.read(reinterpret_cast<char*>(&channel.szChannelName), sizeof(aiString));
                ifs.read(reinterpret_cast<char*>(&channel.iBoneIndex), sizeof(_uint));
                ifs.read(reinterpret_cast<char*>(&channel.iNumPosKeys), sizeof(_uint));
                ifs.read(reinterpret_cast<char*>(&channel.iNumRotKeys), sizeof(_uint));
                ifs.read(reinterpret_cast<char*>(&channel.iNumScaKeys), sizeof(_uint));
                ifs.read(reinterpret_cast<char*>(&channel.iNumKeyFrames), sizeof(_uint));

                _uint keyCount = 0;
                ifs.read(reinterpret_cast<char*>(&keyCount), sizeof(_uint));
                if (keyCount > 0)
                {
                    channel.vecKeyFrame.resize(keyCount);
                    ifs.read(reinterpret_cast<char*>(channel.vecKeyFrame.data()), sizeof(KEYFRAME) * keyCount);
                }
            }
        }
    }



    ifs.close();
    return S_OK;
}


HRESULT CModel::Render(_uint iMeshIndex)
{
    if (FAILED(m_Meshes[iMeshIndex]->Bind_Resources()))
        return E_FAIL;

    if (FAILED(m_Meshes[iMeshIndex]->Render()))
        return E_FAIL;

    return S_OK;
}

void CModel::Set_Animation(_uint iIndex, _bool isLoop, _float fTransitionTime)
{
    if (iIndex >= m_iNumAnimations)
        return;

    m_fTranslationTime = fTransitionTime;
    m_isLoop = isLoop;

    m_isAnimChanged = (m_iCurrentAnimIndex != iIndex);

    if (m_isAnimChanged)
        m_iPrevAnimIndex = m_iCurrentAnimIndex;

    m_iCurrentAnimIndex = iIndex;
    if (m_iPrevAnimIndex == UINT_MAX)
        m_iPrevAnimIndex = iIndex;
}

HRESULT CModel::Ready_Meshes()
{
    if (m_eFileType == FILETYPE::FBX)
    {
        // ==============================
        // || load fbx
        // ==============================
        m_iNumMeshes = m_pAIScene->mNumMeshes;

        for (size_t i = 0; i < m_iNumMeshes; i++)
        {
            CMesh* pMesh = CMesh::Create(m_pDevice, m_pContext, m_eModelType, m_pAIScene->mMeshes[i], m_Bones, XMLoadFloat4x4(&m_PreTransformMatrix));
            if (nullptr == pMesh)
                return E_FAIL;

            m_Meshes.push_back(pMesh);
        }

        // ==============================
        // || store for binary
        // ==============================
        m_BinModel.iNumMeshes = m_iNumMeshes;

        // Mesh
        for (size_t i = 0; i < m_iNumMeshes; i++)
        {
            MESH_DESC tMeshDesc = {};

            aiMesh* tAiMesh = m_pAIScene->mMeshes[i];

            tMeshDesc.szMeshName = tAiMesh->mName;
            tMeshDesc.iMaterialIndex =  tAiMesh->mMaterialIndex;
            tMeshDesc.iNumVertices =  tAiMesh->mNumVertices;
            tMeshDesc.iVertexStride = (m_eModelType == MODELTYPE::NONANIM)? sizeof(VTXMESH) : sizeof(VTXANIMMESH);
            tMeshDesc.iNumIndices =  tAiMesh->mNumFaces * 3; // face 1개 당 인덱스 정점은 3개.

            tMeshDesc.iNumFaces = tAiMesh->mNumFaces;
            // Mesh / Faces
            for (size_t j = 0; j < tAiMesh->mNumFaces; j++)
            {
                MeshFace tFace = {
                    tAiMesh->mFaces[j].mIndices[0],
                    tAiMesh->mFaces[j].mIndices[1],
                    tAiMesh->mFaces[j].mIndices[2]                
                };

                tMeshDesc.vecFaces.push_back(tFace);
            }
            // Mesh / Vertices
            if (m_eModelType == MODELTYPE::NONANIM)
            {
                VTXMESH* pVertices = new VTXMESH[tAiMesh->mNumVertices];
                
                for (size_t j = 0; j < tAiMesh->mNumVertices; j++)
                {
                    // 원래 PreTransformMatrix 반영해서 옮겨줘야 하나, 이는 불러올 때 진행.
                    // 저장은 우선 원본값 그대로 저장
                    memcpy(&pVertices[j].vPosition,          &tAiMesh->mVertices[j], sizeof(_float3));
                    memcpy(&pVertices[j].vNormal,            &tAiMesh->mNormals[j], sizeof(_float3));

                    memcpy(&pVertices[j].vTangent,           &tAiMesh->mTangents[j], sizeof(_float3));
                    memcpy(&pVertices[j].vBinormal,          &tAiMesh->mBitangents[j], sizeof(_float3));
                    memcpy(&pVertices[j].vTexcoord,          &tAiMesh->mTextureCoords[0][j], sizeof(_float2));

                    tMeshDesc.vecNonAnimVertices.push_back(pVertices[j]);
                }
                Safe_Delete_Array(pVertices);
            }
            else if (m_eModelType == MODELTYPE::ANIM)
            {
                VTXANIMMESH* pAnimVertices = new VTXANIMMESH[tAiMesh->mNumVertices];
                ZeroMemory(pAnimVertices, sizeof(VTXANIMMESH) * tAiMesh->mNumVertices);
                
                for (size_t j = 0; j < tAiMesh->mNumVertices; j++)
                {
                    // 원래 PreTransformMatrix 반영해서 옮겨줘야 하나, 이는 불러올 때 진행.
                    // 저장은 우선 원본값 그대로 저장
                    memcpy(&pAnimVertices[j].vPosition,      &tAiMesh->mVertices[j], sizeof(_float3)); 
                    memcpy(&pAnimVertices[j].vNormal,        &tAiMesh->mNormals[j], sizeof(_float3)); 

                    memcpy(&pAnimVertices[j].vTangent,       &tAiMesh->mTangents[j], sizeof(_float3));
                    memcpy(&pAnimVertices[j].vBinormal,      &tAiMesh->mBitangents[j], sizeof(_float3));
                    memcpy(&pAnimVertices[j].vTexcoord,      &tAiMesh->mTextureCoords[0][j], sizeof(_float2));

                    tMeshDesc.vecAnimVertices.push_back(pAnimVertices[j]);
                }


                // Mesh 에 영향주는 뻐를 순회하여 찾은 뒤
                // 해당 뻐가 영향을 주는 버텍스 인덱스릐 vBlendIndex, vBlendWeight 를 할당

                for (_uint j = 0; j < tAiMesh->mNumBones; j++)
                {
                    aiBone* pAIBone = tAiMesh->mBones[j];
                    _uint iBoneIndex = 0;


                    //뼈의 오프셋 매트릭스를 가져온다
                    _float4x4	OffsetMatrix;

                    memcpy(&OffsetMatrix, &pAIBone->mOffsetMatrix, sizeof(_float4x4));

                    //뼈의 오프셋 매트릭스 전치
                    XMStoreFloat4x4(&OffsetMatrix, XMMatrixTranspose(XMLoadFloat4x4(&OffsetMatrix)));



                    auto iter = find_if(m_Bones.begin(), m_Bones.end(), [&](CBone* pBone)->_bool
                        {
                            if (pBone->Compare_Name(pAIBone->mName.data))
                                return true;
                            iBoneIndex++;
                            return false;
                        });

                    m_BinModel.vecBones[iBoneIndex].matOffset = OffsetMatrix;   // test
                    tMeshDesc.vecUsingBonesIndices.push_back(iBoneIndex);       // test
                    tMeshDesc.iNumUsingBones = tAiMesh->mNumBones;              // test

                
                    for (size_t k = 0; k < pAIBone->mNumWeights; k++)
                    {
                        aiVertexWeight	AIVertexWeight = pAIBone->mWeights[k];

                        /* j번째 뼈가 영향을 주는 k번째 정점의 정점버퍼상의 인덱스 */
                        if      (0.f == pAnimVertices[AIVertexWeight.mVertexId].vBlendWeight.x)
                        {
                            tMeshDesc.vecAnimVertices[AIVertexWeight.mVertexId].vBlendIndex.x = j;
                            tMeshDesc.vecAnimVertices[AIVertexWeight.mVertexId].vBlendWeight.x = AIVertexWeight.mWeight;
                        }
                        else if (0.f == pAnimVertices[AIVertexWeight.mVertexId].vBlendWeight.y)
                        {
                            tMeshDesc.vecAnimVertices[AIVertexWeight.mVertexId].vBlendIndex.y = j;
                            tMeshDesc.vecAnimVertices[AIVertexWeight.mVertexId].vBlendWeight.y = AIVertexWeight.mWeight;
                        }
                        else if (0.f == pAnimVertices[AIVertexWeight.mVertexId].vBlendWeight.z)
                        {
                            tMeshDesc.vecAnimVertices[AIVertexWeight.mVertexId].vBlendIndex.z = j;
                            tMeshDesc.vecAnimVertices[AIVertexWeight.mVertexId].vBlendWeight.z = AIVertexWeight.mWeight;
                        }
                        else
                        {
                            tMeshDesc.vecAnimVertices[AIVertexWeight.mVertexId].vBlendIndex.w = j;
                            tMeshDesc.vecAnimVertices[AIVertexWeight.mVertexId].vBlendWeight.w = AIVertexWeight.mWeight;
                        }
                    }

                    
                }
                Safe_Delete_Array(pAnimVertices);
            }

            /* 임시로 위 3줄로 대체
            //tMeshDesc.vecNonAnimVertices;
            //tMeshDesc.vecAnimVertices;

            tMeshDesc.iNumUsingBones = tAiMesh->mNumBones;
            // Mesh / UsingBones
            for (size_t j = 0; j < tAiMesh->mNumBones; j++)          // compare bone (bone in mesh)
            {
                // Mesh / UsingBones / Bones Name Compare Loop
                for (size_t k = 0; k < m_BinModel.iNumBones; k++)   // origin bone (bone in origin binmodel)
                {
                    // 원본 본과 메쉬가 사용중인 본을 비교 후, 이름 일치 시 해당 원본 본의 인덱스를 컨테이너에 추가.
                    aiString szBoneCompare = tAiMesh->mBones[j]->mName;
                    aiString szBoneOrigin = m_BinModel.vecBones[k].szBoneName;

                    if (szBoneOrigin == szBoneCompare)
                    {
                        _uint iUsingBoneIndex = static_cast<_uint>(j);
                        //tMeshDesc.vecUsingBonesIndices.push_back(iUsingBoneIndex);
                        
                        _float4x4	matOffset;
                        aiBone* pAIBone = tAiMesh->mBones[i];
                        memcpy(&matOffset, &pAIBone->mOffsetMatrix, sizeof(_float4x4));

                        m_BinModel.vecBones[iUsingBoneIndex].matOffset = matOffset;
                    }
                }
            }
            */



            m_BinModel.vecMeshes.push_back(tMeshDesc);
        }


    }
    else if (m_eFileType == FILETYPE::DATMODEL)
    {
        // ksta : binary
        m_iNumMeshes = m_BinModel.iNumMeshes;
        m_eModelType = m_BinModel.eAnimtype;

        for (size_t i = 0; i < m_iNumMeshes; i++)
        {
            MESH_DESC tMeshDesc = m_BinModel.vecMeshes[i];

            CMesh* pMesh = CMesh::Create_Binary(m_pDevice, m_pContext, m_eModelType, tMeshDesc, &m_BinModel.vecBones, XMLoadFloat4x4(&m_PreTransformMatrix));
            if (nullptr == pMesh)
                return E_FAIL;

            m_Meshes.push_back(pMesh);
        }
        
    }


    return S_OK;
}

HRESULT CModel::Ready_Materials(const _char* pModelFilePath)
{
    if (m_eFileType == FILETYPE::FBX)
    {
        // ==============================
        // || load fbx
        // ==============================

        m_iNumMaterials = m_pAIScene->mNumMaterials;

        for (size_t i = 0; i < m_iNumMaterials; i++)
        {
            CMeshMaterial* pMeshMaterial = CMeshMaterial::Create(m_pDevice, m_pContext, pModelFilePath, m_pAIScene->mMaterials[i]);
            if (nullptr == pMeshMaterial)
                return E_FAIL;

            m_Materials.push_back(pMeshMaterial);
        }

        // ==============================
        // || store for binary
        // ==============================

        m_BinModel.iNumMaterials = m_pAIScene->mNumMaterials;

        // Materials..
        for (size_t i = 0; i < m_pAIScene->mNumMaterials; i++)
        {
            MATERIAL_DESC tMatDesc = {};
            aiMaterial* tAiMat = m_pAIScene->mMaterials[i];
            
            tMatDesc.szMaterialName = tAiMat->GetName();
            tMatDesc.iMaterialIndex = static_cast<_uint>(i);

            _uint iTextureCount = 0;
            // Material / Textures..
            for (_uint texType = aiTextureType_NONE + 1; texType <= AI_TEXTURE_TYPE_MAX; ++texType)
            {
                const _uint numTex = tAiMat->GetTextureCount((aiTextureType)texType);
                iTextureCount += numTex;

                for (_uint j = 0; j < numTex; ++j)
                {
                    aiString path;
                    if (AI_SUCCESS == tAiMat->GetTexture(static_cast<aiTextureType>(texType), j, &path))
                        tMatDesc.vecTexturePaths.emplace_back(static_cast<aiTextureType>(texType), path);   // pair 만들면서 push_back
                }
            }

            tMatDesc.iNumTextures = iTextureCount;
            m_BinModel.vecMaterials.push_back(tMatDesc);        // 뽑아온 데이터를 벡터에 저장!
        }
        

    }
    else if (m_eFileType == FILETYPE::DATMODEL)
    {
        // ksta : binary
        m_iNumMaterials = m_BinModel.iNumMaterials;

        for (size_t i = 0; i < m_iNumMaterials; i++)
        {
            MATERIAL_DESC tMatDesc = m_BinModel.vecMaterials[i];
            CMeshMaterial* pMat = CMeshMaterial::Create_Binary(m_pDevice, m_pContext, pModelFilePath, tMatDesc); // ksta : 8/3 여기 채워야함
            if (nullptr == pMat)
                return E_FAIL;

            m_Materials.push_back(pMat);
        }
    }


    return S_OK;
}

HRESULT CModel::Ready_Bones(const aiNode* pAINode, _int iParentIndex)
{
    if (m_eFileType == FILETYPE::FBX)
    {
        // ==============================
        // || load fbx
        // ==============================
        
        CBone* pBone = CBone::Create(pAINode, iParentIndex);
        if (nullptr == pBone)
            return E_FAIL;

        m_Bones.push_back(pBone);           // 여기서 1개 추가했으니까,

        // ==============================
        // || store for binary
        // start=========================

        BONE_DESC tBinBone = pBone->Get_BinaryBone();
        m_BinModel.vecBones.push_back(tBinBone);
        
        // end===========================


        _int   iIndex = m_Bones.size() - 1; // 여기서 1 빼 주는 것. 1개 추가했으면 0번째 인덱스여야 하므로.

        for (size_t i = 0; i < pAINode->mNumChildren; i++)
        {
            Ready_Bones(pAINode->mChildren[i], iIndex);
        }


    }
    else if (m_eFileType == FILETYPE::DATMODEL)
    {
        // ksta : binary

        // ==============================
        // || load binary
        // ==============================

        for (size_t i = 0; i < m_BinModel.iNumBones; i++)
        {
            BONE_DESC tBoneDesc = m_BinModel.vecBones[i];
            
            CBone* pBone = CBone::Create_Binary(tBoneDesc);
            if (nullptr == pBone)
                return E_FAIL;

            m_Bones.push_back(pBone);           // 여기서 1개 추가했으니까,
        }
    }

 
    // for export
    if (m_eFileType == FILETYPE::FBX)
    {
        // store for binary
        m_BinModel.iNumBones = m_Bones.size();
    }
    else if (m_eFileType == FILETYPE::DATMODEL)
    {
        m_BinModel.iNumBones = m_Bones.size();
    }


    return S_OK;
}

HRESULT CModel::Ready_Animations()
{
    /* 시간에 따라 내 뼈들이 어떻게 움직여야하는가? 에 대한 정보가 필요하다.  */
    /* 대기동작을 위해서는 뼈들이 어떤 시간대에 어떤 상태를 취하는가? */
    /* 공격동작을 위해서는 뼈들이 어떤 시간대에 어떤 상태를 취하는가? */

    if (m_eFileType == FILETYPE::FBX)
    {
        // ==============================
        // || load fbx
        // ==============================

        m_iNumAnimations = m_pAIScene->mNumAnimations;

        for (size_t i = 0; i < m_iNumAnimations; i++)
        {
            CAnimation* pAnimation = CAnimation::Create(m_pAIScene->mAnimations[i], m_Bones);
            if (nullptr == pAnimation)
                return E_FAIL;

            m_Animations.push_back(pAnimation);
        }



        // ==============================
        // || store for binary
        // ==============================

        m_BinModel.iNumAnimations = m_pAIScene->mNumAnimations;

        // 애니메이션..
        for (size_t i = 0; i < m_pAIScene->mNumAnimations; i++)
        {
            AIANIM_DESC tAiAnimDesc = {};
            aiAnimation* aiAnim = m_pAIScene->mAnimations[i];

            tAiAnimDesc.szAnimName = aiAnim->mName;
            tAiAnimDesc.fDuration = aiAnim->mDuration;
            tAiAnimDesc.fTicksPerSecond = aiAnim->mTicksPerSecond;
            tAiAnimDesc.iNumChannels = aiAnim->mNumChannels;



            // 애니메이션 내의 채널..
            for (size_t j = 0; j < tAiAnimDesc.iNumChannels; j++)
            {
                AICHANNEL_DESC tAiChannelDesc = {};
                aiNodeAnim* aiChan = aiAnim->mChannels[j];
                _uint iBoneIndex = 0;

                tAiChannelDesc.szChannelName = aiChan->mNodeName;
                tAiChannelDesc.iNumScaKeys = aiChan->mNumScalingKeys;
                tAiChannelDesc.iNumRotKeys = aiChan->mNumRotationKeys;
                tAiChannelDesc.iNumPosKeys = aiChan->mNumPositionKeys;
                tAiChannelDesc.iNumKeyFrames = max(max(tAiChannelDesc.iNumPosKeys, tAiChannelDesc.iNumRotKeys), tAiChannelDesc.iNumScaKeys);

                // iBoneIndex 를 구하기 위함
                auto	iter = find_if(m_Bones.begin(), m_Bones.end(), [&](CBone* pBone)->_bool
                    {
                        if (true == pBone->Compare_Name(aiChan->mNodeName.data))
                            return true;

                        iBoneIndex++;

                        return false;
                    });
                    
                tAiChannelDesc.iBoneIndex =iBoneIndex;

                // 애니메이션 내의 채널 내의 키프레임..
                for (size_t k = 0; k < tAiChannelDesc.iNumKeyFrames; k++)
                {
                    KEYFRAME tKeyFrame = {};

                    if (k < aiChan->mNumScalingKeys) {
                        memcpy(&tKeyFrame.vScale, &aiChan->mScalingKeys[k].mValue, sizeof(_float3));

                        tKeyFrame.fTrackPosition = aiChan->mScalingKeys[k].mTime;
                    }
                    if (k < aiChan->mNumRotationKeys) {
                        tKeyFrame.vRotation.x = aiChan->mRotationKeys[k].mValue.x;
                        tKeyFrame.vRotation.y = aiChan->mRotationKeys[k].mValue.y;
                        tKeyFrame.vRotation.z = aiChan->mRotationKeys[k].mValue.z;
                        tKeyFrame.vRotation.w = aiChan->mRotationKeys[k].mValue.w;

                        tKeyFrame.fTrackPosition = aiChan->mRotationKeys[k].mTime;
                    }
                    if (k < aiChan->mNumPositionKeys) {
                        memcpy(&tKeyFrame.vTranslation, &aiChan->mPositionKeys[k].mValue, sizeof(_float3));

                        tKeyFrame.fTrackPosition = aiChan->mPositionKeys[k].mTime;
                    }

                    tAiChannelDesc.vecKeyFrame.push_back(tKeyFrame);
                }
                tAiAnimDesc.vecChannels.push_back(tAiChannelDesc);
            }
            m_BinModel.vecAiAnimations.push_back(tAiAnimDesc);
        }



    }
    else if (m_eFileType == FILETYPE::DATMODEL)
    {
        // ksta : binary
        m_iNumAnimations = m_BinModel.iNumAnimations;

        for (size_t i = 0; i < m_iNumAnimations; i++)
        {
            AIANIM_DESC tAnimDesc = m_BinModel.vecAiAnimations[i];
            CAnimation* pAnim = CAnimation::Create_Binary(tAnimDesc, m_Bones);
            if (nullptr == pAnim)
                return E_FAIL;

            m_Animations.push_back(pAnim);
        }
    }


    return S_OK;
}



CModel* CModel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, MODELTYPE eModelType, const _char* pModelFilePath, _fmatrix PreTransformMatrix)
{
    CModel* pInstance = new CModel(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype(eModelType, pModelFilePath, PreTransformMatrix)))
    {
        MSG_BOX(TEXT("Failed to Created : CModel"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CComponent* CModel::Clone(void* pArg)
{
    CModel* pInstance = new CModel(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CModel"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CModel::Free()
{
    __super::Free();

    for (auto& pAnimation : m_Animations)
        Safe_Release(pAnimation);

    m_Animations.clear();

    for (auto& pBone : m_Bones)
        Safe_Release(pBone);

    m_Bones.clear();

    for (auto& pMesh : m_Meshes)
        Safe_Release(pMesh);

    m_Meshes.clear();

    for (auto& pMaterial : m_Materials)
        Safe_Release(pMaterial);

    m_Materials.clear();

    if (m_pAIScene)
        m_Importer.FreeScene();



}

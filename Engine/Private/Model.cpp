#include "Model.h"

#include "Mesh.h"
#include "Bone.h"
#include "MeshMaterial.h"
#include "Animation.h"

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

        if (FAILED(Ready_Bones(nullptr, 0)))
            return E_FAIL;

        if (FAILED(Ready_Meshes()))
            return E_FAIL;

        if (FAILED(Ready_Materials(pModelFilePath)))
            return E_FAIL;

        if (FAILED(Ready_Animations()))
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
    m_Animations[m_iCurrentAnimIndex]->Update_TransformationMatrices(m_Bones, m_isLoop, &m_isFinished, fTimeDelta);


    /* 바꿔야할 뼈들의 Transforemation행렬이 갱신되었다면, 정점들에게 직접 전달되야할 CombindTransformationMatrix를 만들어준다. */
    for (auto& pBone : m_Bones)
    {
        pBone->Update_CombinedTransformationMatrix(m_PreTransformMatrix, m_Bones);
    }

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

                _uint faceCount = static_cast<_uint>(mesh.vecFaces.size());
                ofs.write(reinterpret_cast<const char*>(&faceCount), sizeof(_uint));
                if (faceCount > 0)
                    ofs.write(reinterpret_cast<const char*>(mesh.vecFaces.data()), sizeof(MeshFace) * faceCount);

                _uint boneIdxCount = static_cast<_uint>(mesh.vecUsingBonesIndices.size());
                ofs.write(reinterpret_cast<const char*>(&boneIdxCount), sizeof(_uint));
                if (boneIdxCount > 0)
                    ofs.write(reinterpret_cast<const char*>(mesh.vecUsingBonesIndices.data()), sizeof(_uint) * boneIdxCount);
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
                    ofs.write(reinterpret_cast<const char*>(&texPath), sizeof(aiString));
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
    //XMStoreFloat4x4(&m_BinModel.matPreTransformMatrix, PreTransformMatrix);                     // 외부 입력으로 교체시.

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
                ifs.read(reinterpret_cast<char*>(&mat.vecTexturePaths[j]), sizeof(aiString));
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

void CModel::Set_Animation(_uint iIndex, _bool isLoop)
{
    if (iIndex >= m_iNumAnimations)
        return;

    m_isLoop = isLoop;
    m_iCurrentAnimIndex = iIndex;
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
                        tMeshDesc.vecUsingBonesIndices.push_back(iUsingBoneIndex);
                    }
                }
            }

            m_BinModel.vecMeshes.push_back(tMeshDesc);
        }


    }
    else if (m_eFileType == FILETYPE::DATMODEL)
    {
        // ksta : binary
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
            for (int texType = aiTextureType_NONE + 1; texType <= AI_TEXTURE_TYPE_MAX; ++texType)
            {
                const _uint numTex = tAiMat->GetTextureCount((aiTextureType)texType);
                iTextureCount += numTex;

                for (_uint j = 0; j < numTex; ++j)
                {
                    aiString path;
                    if (AI_SUCCESS == tAiMat->GetTexture((aiTextureType)texType, j, &path))
                        tMatDesc.vecTexturePaths.push_back(path);
                }
            }

            tMatDesc.iNumTextures = iTextureCount;
            m_BinModel.vecMaterials.push_back(tMatDesc);        // 뽑아온 데이터를 벡터에 저장!
        }
        

    }
    else if (m_eFileType == FILETYPE::DATMODEL)
    {
        // ksta : binary
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
            // 로드..?
            
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

                tAiChannelDesc.szChannelName = aiChan->mNodeName;
                tAiChannelDesc.iNumScaKeys = aiChan->mNumScalingKeys;
                tAiChannelDesc.iNumRotKeys = aiChan->mNumRotationKeys;
                tAiChannelDesc.iNumPosKeys = aiChan->mNumPositionKeys;
                tAiChannelDesc.iNumKeyFrames = max(max(tAiChannelDesc.iNumPosKeys, tAiChannelDesc.iNumRotKeys), tAiChannelDesc.iNumScaKeys);

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

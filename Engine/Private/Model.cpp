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

    _uint           iFlag = { aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast };

    if (MODELTYPE::NONANIM == m_eModelType)
        iFlag |= aiProcess_PreTransformVertices;

    m_pAIScene = m_Importer.ReadFile(pModelFilePath, iFlag);
    if (nullptr == m_pAIScene)
        return E_FAIL;

    if (FAILED(Ready_Bones(m_pAIScene->mRootNode, -1)))
        return E_FAIL;

    if (FAILED(Ready_Meshes()))
        return E_FAIL;

    if (FAILED(Ready_Materials(pModelFilePath)))
        return E_FAIL;

    if (FAILED(Ready_Animations()))
        return E_FAIL;

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
    // 데이터를 구조체에 담기..

    MODEL_DESC tModelDesc = {};

    tModelDesc.eAnimtype = m_eModelType;
    tModelDesc.matPreTransformMatrix = m_PreTransformMatrix;

    tModelDesc.iNumBones = m_Bones.size();
    tModelDesc.iNumMeshes = m_iNumMeshes;
    tModelDesc.iNumMaterials = m_iNumMaterials;
    tModelDesc.iNumAnimations = (m_eModelType == MODELTYPE::ANIM)? m_iNumAnimations : 0;

    m_pAIScene->mMaterials;
    //for (auto bone : m_Bones)
    //    tModelDesc.vecBones.push_back(*bone);
    //for (auto mesh : m_Meshes)
    //    tModelDesc.vecMeshes.push_back(*mesh);
    //for (auto material : m_Materials)
    //    tModelDesc.vecMaterials.push_back(*material);

    // if modeltype animation..
    if (m_eModelType == MODELTYPE::ANIM)
    {
        // 애니메이션..
        for (size_t i = 0; i < m_iNumAnimations; i++)
        {
            AIANIM_DESC tAiAnimDesc = {};
            aiAnimation* aiAnim = m_pAIScene->mAnimations[i];

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
            tModelDesc.vecAiAnimations.push_back(tAiAnimDesc);
        }

    }



    //  데이터를 저장하기..
    ofstream ofs(strSavePath->c_str(), ios::binary);

    if (ofs.is_open()) {
        ofs.write(reinterpret_cast<char*>(&tModelDesc), sizeof(MODEL_DESC));
        ofs.close();
    }


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
    m_iNumMeshes = m_pAIScene->mNumMeshes;

    for (size_t i = 0; i < m_iNumMeshes; i++)
    {
        CMesh* pMesh = CMesh::Create(m_pDevice, m_pContext, m_eModelType, m_pAIScene->mMeshes[i], m_Bones, XMLoadFloat4x4(&m_PreTransformMatrix));
        if (nullptr == pMesh)
            return E_FAIL;

        m_Meshes.push_back(pMesh);
    }

    return S_OK;
}

HRESULT CModel::Ready_Materials(const _char* pModelFilePath)
{
    m_iNumMaterials = m_pAIScene->mNumMaterials;

    for (size_t i = 0; i < m_iNumMaterials; i++)
    {

        CMeshMaterial* pMeshMaterial = CMeshMaterial::Create(m_pDevice, m_pContext, pModelFilePath, m_pAIScene->mMaterials[i]);
        if (nullptr == pMeshMaterial)
            return E_FAIL;

        m_Materials.push_back(pMeshMaterial);
    }


    return S_OK;
}

HRESULT CModel::Ready_Bones(const aiNode* pAINode, _int iParentIndex)
{
    CBone* pBone = CBone::Create(pAINode, iParentIndex);
    if (nullptr == pBone)
        return E_FAIL;

    m_Bones.push_back(pBone);           // 여기서 1개 추가했으니까,

    _int   iIndex = m_Bones.size() - 1; // 여기서 1 빼 주는 것. 1개 추가했으면 0번째 인덱스여야 하므로.

    for (size_t i = 0; i < pAINode->mNumChildren; i++)
    {
        Ready_Bones(pAINode->mChildren[i], iIndex);
    }

    return S_OK;
}

HRESULT CModel::Ready_Animations()
{
    /* 시간에 따라 내 뼈들이 어떻게 움직여야하는가? 에 대한 정보가 필요하다.  */
    /* 대기동작을 위해서는 뼈들이 어떤 시간대에 어떤 상태를 취하는가? */
    /* 공격동작을 위해서는 뼈들이 어떤 시간대에 어떤 상태를 취하는가? */
    m_iNumAnimations = m_pAIScene->mNumAnimations;

    for (size_t i = 0; i < m_iNumAnimations; i++)
    {
        CAnimation* pAnimation = CAnimation::Create(m_pAIScene->mAnimations[i], m_Bones);        if (nullptr == pAnimation)
            return E_FAIL;

        m_Animations.push_back(pAnimation);
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


    m_Importer.FreeScene();



}

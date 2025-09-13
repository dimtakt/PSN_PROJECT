#include "Weapon_Karabin.h"
#include "GameInstance.h"
#include "Bullet.h"

CWeapon_Karabin::CWeapon_Karabin(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CWeapon_Gun{ pDevice, pContext }
{

}

CWeapon_Karabin::CWeapon_Karabin(const CWeapon_Karabin& Prototype)
    : CWeapon_Gun{ Prototype }
{

}

HRESULT CWeapon_Karabin::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CWeapon_Karabin::Initialize(void* pArg)
{
    WEAPON_DESC* pDesc = static_cast<WEAPON_DESC*>(pArg);
    m_pParentState = pDesc->pState;
    m_pSocketMatrix = pDesc->pSocketMatrix;
    m_pParentTarget = pDesc->pParentTarget;

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    // 초기 트랜스폼 설정
    _matrix matId = XMMatrixIdentity();
    _matrix matScale = matId, matRot1 = matId, matRot2 = matId, matPos = matId;


    if (m_pParentTarget->Get_ObjType() == ENUM_CLASS(GAMEOBJ_TYPE::PLAYER))
    {
        matPos = XMMatrixTranslation(0.2f, -0.05f, 0.5f); // 순서대로 오른쪽, 위쪽, 앞쪽
    }
    else if (m_pParentTarget->Get_ObjType() == ENUM_CLASS(GAMEOBJ_TYPE::ENEMY))
    {
        matScale = XMMatrixScaling(1.f, 1.f, 1.f);
        matRot1 = XMMatrixRotationX(TO_RAD(100));
        matRot2 = XMMatrixRotationZ(TO_RAD(215));
        matPos = XMMatrixTranslation(-0.15f, +0.15f, +0.1f);  // 오-뒤 - 왼-앞 - 위
    }

    // matScale = XMMatrixScaling(1.f, 1.f, 1.f);
    // matRot1 = XMMatrixRotationX(TO_RAD(-90));
    // matRot2 = XMMatrixRotationZ(TO_RAD(180));
    // matPos = XMMatrixTranslation(-0.2f, -0.6f, -1.5f);
    //matPos = XMMatrixTranslation(0.2f, -0.05f, 0.5f); // 순서대로 오른쪽, 위쪽, 앞쪽

    _matrix matTransform = matScale * matRot1 * matRot2 * matPos;
    m_pTransformCom->Set_WorldMatrix(matTransform);


    //m_pTransformCom->Scaling(_float3(1.f, 1.f, 1.f));
    //m_pTransformCom->Rotation(XMVectorSet(1.f, 1.f, 1.f, 0.f), XMConvertToRadians(180.0f));
    //m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(0.f, 0.f, -1.5f, 1.f));


    m_iGameObjType = ENUM_CLASS(GAMEOBJ_TYPE::WEAPON_RANGED_KARABIN);

    return S_OK;
}

void CWeapon_Karabin::Priority_Update(_float fTimeDelta)
{
    // 뭐 안하는듯?
    __super::Priority_Update(fTimeDelta);
}

void CWeapon_Karabin::Update(_float fTimeDelta)
{
    // weapon.cpp 에서 기본 업데이트 도는 중
    //__super::Update(fTimeDelta);


    // 수동 Update. m_pTransform 은 로컬 트랜스폼이 되어야 하는데..

    if      (m_pParentTarget->Get_ObjType() == ENUM_CLASS(GAMEOBJ_TYPE::PLAYER))
    {
        const _float4x4* matCam = m_pGameInstance->Get_Transform_Float4x4_Inverse(D3DTS::VIEW);
        XMStoreFloat4x4(&m_CombinedWorldMatrix, m_pTransformCom->Get_WorldMatrix() * XMLoadFloat4x4(matCam));


    }
    
    else if (m_pParentTarget->Get_ObjType() == ENUM_CLASS(GAMEOBJ_TYPE::ENEMY))
    {
        _matrix     BoneMatrix = XMLoadFloat4x4(m_pSocketMatrix);

        for (size_t i = 0; i < 3; i++)
            BoneMatrix.r[i] = XMVector3Normalize(BoneMatrix.r[i]);

        XMStoreFloat4x4(&m_CombinedWorldMatrix,
            m_pTransformCom->Get_WorldMatrix() * BoneMatrix * XMLoadFloat4x4(m_pParentMatrix));
    }


    for (auto& vecColliders : m_vecCollidersCom)
        for (auto& collider : vecColliders)
            collider->Update(XMLoadFloat4x4(&m_CombinedWorldMatrix));


    // 여기에서 업데이트 동작 진행.. (총알 발사 등)

}

void CWeapon_Karabin::Late_Update(_float fTimeDelta)
{
    // weapon.cpp 에서 렌더그룹 및 디버그용 렌더 준비중 
    __super::Late_Update(fTimeDelta);   

}

HRESULT CWeapon_Karabin::Render()
{
    if (FAILED(__super::Render()))
        return E_FAIL;

    _uint           iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (size_t i = 0; i < iNumMeshes; i++)
    {
        if (FAILED(m_pModelCom->Bind_Materials(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE, 0)))
            return E_FAIL;

        m_pShaderCom->Begin(0);

        m_pModelCom->Render(i);
    }

    return S_OK;
}

void CWeapon_Karabin::Shot(_vector vDir, _uint iObjTypeIndex)
{
    // 총알 생성..
    _uint iDestLevel = m_pGameInstance->Get_DestLevel();
    CBullet::Bullet_DESC bulletDesc = {};
    bulletDesc.vMoveDir = vDir;
    //bulletDesc.vMoveDir = vDir;
    bulletDesc.fSpeedPerSec = 0.f;
    bulletDesc.iGameObjType = iObjTypeIndex;

    // 주체가 플레이어면 카메라에서 나가도록
    // 적이라면 무기좌표에서 나가도록
    bulletDesc.matSpawnTransform;
    if      (iObjTypeIndex == ENUM_CLASS(GAMEOBJ_TYPE::PLAYERBULLET))
        bulletDesc.matSpawnTransform = m_pGameInstance->Get_Transform_Matrix_Inverse(D3DTS::VIEW);
    else if (iObjTypeIndex == ENUM_CLASS(GAMEOBJ_TYPE::ENEMYBULLET))
        bulletDesc.matSpawnTransform = XMLoadFloat4x4(&m_CombinedWorldMatrix);


    if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(iDestLevel, L"Layer_Bullet",
        ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Weapon_Bullet"), &bulletDesc)))
        MSG_BOX(L"총알 생성 실패");
}

HRESULT CWeapon_Karabin::Ready_Components()
{
    _uint iDestLevel = m_pGameInstance->Get_DestLevel();

    if (FAILED(CGameObject::Add_Component(iDestLevel, TEXT("Prototype_Component_Shader_VtxMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_Weapon_Karabin"),
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom), nullptr)))
        return E_FAIL;


    CBounding_OBB::BOUNDING_OBB_DESC  OBBDesc{};
    OBBDesc.vAngles = _float3(0.f, 0.f, 0.f);
    OBBDesc.vExtents = _float3(.05f, .15f, .5f);
    OBBDesc.vCenter = _float3(0.f, -(OBBDesc.vExtents.y * 0.65f), 0.f);

    CCollider* tmpColCom = nullptr;
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Collider_OBB"),
        TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&tmpColCom), &OBBDesc)))
        return E_FAIL;
    m_vecCollidersCom[ENUM_CLASS(COLLIDERTYPE::OBB)].push_back(tmpColCom);

    return S_OK;

}


//HRESULT CWeapon_Karabin::Bind_ShaderResources()
//{
//    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMatrix)))
//        return E_FAIL;
//
//    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
//        return E_FAIL;
//
//    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
//        return E_FAIL;
//
//    return S_OK;
//}

CWeapon_Karabin* CWeapon_Karabin::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CWeapon_Karabin* pInstance = new CWeapon_Karabin(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CWeapon_Karabin"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CWeapon_Karabin::Clone(void* pArg)
{
    CWeapon_Karabin* pInstance = new CWeapon_Karabin(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CWeapon_Karabin"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CWeapon_Karabin::Free()
{
    __super::Free();
}

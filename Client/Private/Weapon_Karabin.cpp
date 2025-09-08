#include "Weapon_Karabin.h"
#include "GameInstance.h"

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

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    // 초기 트랜스폼 설정
    m_pTransformCom->Scaling(_float3(0.1f, 0.1f, 0.1f));
    m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(90.0f));
    m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(0.8f, 0.f, 0.f, 1.f));

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
    __super::Update(fTimeDelta);


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
    OBBDesc.vExtents = _float3(1.0f, 1.5f, 2.f);
    OBBDesc.vCenter = _float3(0.f, OBBDesc.vExtents.y, 0.f);

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

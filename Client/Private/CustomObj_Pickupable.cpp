#include "CustomObj_Pickupable.h"
#include "Gameinstance.h"


CCustomObj_Pickupable::CCustomObj_Pickupable(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CCustomObj_NonAnim(pDevice, pContext)
{
}

CCustomObj_Pickupable::CCustomObj_Pickupable(const CGameObject& Prototype)
    : CCustomObj_NonAnim(Prototype)
{
}

HRESULT CCustomObj_Pickupable::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CCustomObj_Pickupable::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(this->Ready_Components(pArg)))
        return E_FAIL;

    return S_OK;
}

void CCustomObj_Pickupable::Priority_Update(_float fTimeDelta)
{
    __super::Priority_Update(fTimeDelta);
}

void CCustomObj_Pickupable::Update(_float fTimeDelta)
{


    m_vecCollidersCom[ENUM_CLASS(COLLIDERTYPE::OBB)][0]->Update(m_pTransformCom->Get_WorldMatrix());	// Direct Update
}

void CCustomObj_Pickupable::Late_Update(_float fTimeDelta)
{
    if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::NONBLEND, this)))
        return;

#ifdef _DEBUG
    for (auto& vecColliders : m_vecCollidersCom)
        for (auto& collider : vecColliders)
        {
            if (FAILED(m_pGameInstance->Add_DebugComponent(collider)))
                return;
        }
#endif
}

HRESULT CCustomObj_Pickupable::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    _uint           iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (_uint i = 0; i < iNumMeshes; i++)
    {
        if (FAILED(m_pModelCom->Bind_Materials(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE, 0)))
            return E_FAIL;

        m_pShaderCom->Begin(0);

        m_pModelCom->Render(i);


    }

    return S_OK;
}

void CCustomObj_Pickupable::OnCollisionRay()
{
    // 플레이어가 클릭 시 플레이어쪽으로 다가오며 사라지도록 할 것
    // 이게 무슨 오브젝트인지의 정보를 플레이어 단계에서 알아야 함

}

HRESULT CCustomObj_Pickupable::Ready_Components(void* pArg)
{
#pragma region old

    /*
   
    // 부모 오브젝트에서 이미 이루어짐

    //CUSTOMOBJ_DESC* pDesc = reinterpret_cast<CUSTOMOBJ_DESC*>(pArg);
    //
    //_wstring strPrototypeName = pDesc->strModelComPrototypeTag;
    //m_iGameObjType = pDesc->iGameObjType;
    //
    //_uint iDestLevelIndex = m_pGameInstance->Get_DestLevel();
    //
    //if (FAILED(CGameObject::Add_Component(iDestLevelIndex, TEXT("Prototype_Component_Shader_VtxMesh"),
    //    TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
    //    return E_FAIL;
    //
    //
    //if (FAILED(CGameObject::Add_Component(iDestLevelIndex, strPrototypeName,
    //    TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom), nullptr)))
    //    return E_FAIL;
    //Set_BufferRef(m_pModelCom);

    */

#pragma endregion

    // 콜라이더 추가..
    // 이게 일반 NonAnim과 Pickupable의 다른점 중 하나. 이를 통해 ray 인식이 가능하도록 함

    CBounding_OBB::BOUNDING_OBB_DESC  OBBDesc{};

    switch (m_iGameObjType)
    {
    case ENUM_CLASS(GAMEOBJ_TYPE::WEAPON_RANGED_KARABIN):
        OBBDesc.vAngles = _float3(0.f, 0.f, 0.f);
        OBBDesc.vExtents = _float3(.05f, .15f, .5f);
        OBBDesc.vCenter = _float3(0.f, -(OBBDesc.vExtents.y * 0.65f), 0.f);
        break;
    case ENUM_CLASS(GAMEOBJ_TYPE::WEAPON_RANGED_PISTOL):
        OBBDesc.vAngles = _float3(0.f, 0.f, 0.f);
        OBBDesc.vExtents = _float3(.03f, .10f, .15f);
        OBBDesc.vCenter = _float3(0.f, 0.f, +(OBBDesc.vExtents.z * 0.4f));
        break;
    case ENUM_CLASS(GAMEOBJ_TYPE::WEAPON_RANGED_SHOTGUN):
        OBBDesc.vAngles = _float3(0.f, 0.f, 0.f);
        OBBDesc.vExtents = _float3(.05f, .15f, .7f);
        OBBDesc.vCenter = _float3(0.f, -(OBBDesc.vExtents.y * 0.2f), +(OBBDesc.vExtents.z * 0.4f));
        break;
    default:
        break;
    }


    m_iGameObjType;

    CCollider* tmpColCom = nullptr;
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Collider_OBB"),
        TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&tmpColCom), &OBBDesc)))
        return E_FAIL;
    m_vecCollidersCom[ENUM_CLASS(COLLIDERTYPE::OBB)].push_back(tmpColCom);

    return S_OK;
}

HRESULT CCustomObj_Pickupable::Bind_ShaderResources()
{
    if (FAILED(m_pTransformCom->Bind_Shader_Resource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
        return E_FAIL;



    return S_OK;
}


CCustomObj_Pickupable* CCustomObj_Pickupable::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CCustomObj_Pickupable* pInstance = new CCustomObj_Pickupable(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Create : CCustomObj_Pickupable"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CCustomObj_Pickupable* CCustomObj_Pickupable::Clone(void* pArg)
{
    CCustomObj_Pickupable* pInstance = new CCustomObj_Pickupable(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Clone : CCustomObj_Pickupable"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CCustomObj_Pickupable::Free()
{
    __super::Free();

    for (auto& vecColliders : m_vecCollidersCom)
        for (auto& collider : vecColliders)
            Safe_Release(collider);
}

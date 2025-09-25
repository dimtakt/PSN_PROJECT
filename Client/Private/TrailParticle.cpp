#include "TrailParticle.h"
#include "GameInstance.h"

CTrailParticle::CTrailParticle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject{ pDevice, pContext }
{
}

CTrailParticle::CTrailParticle(const CTrailParticle& Prototype)
    : CGameObject{ Prototype }
{
}

HRESULT CTrailParticle::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CTrailParticle::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    TRAIL_DESC* pDesc = static_cast<TRAIL_DESC*>(pArg);

    m_pTargetObj = pDesc->pOwner;

    return S_OK;
}

void CTrailParticle::Priority_Update(_float fTimeDelta)
{
    int a = 10;
}

void CTrailParticle::Update(_float fTimeDelta)
{
    CTransform* pTargetTransformCom = dynamic_cast<CTransform*>(m_pTargetObj->Get_Component(L"Com_Transform"));
    m_pTransformCom->Set_Position_Direct(pTargetTransformCom->Get_Position());
    
    _vector vTargetPos = pTargetTransformCom->Get_Position();
    m_pVIBufferCom->Trail(fTimeDelta, m_pTargetObj);

    if (m_pVIBufferCom->Get_isLifeOver())
        m_isDead = true;
}

void CTrailParticle::Late_Update(_float fTimeDelta)
{
    if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::NONLIGHT, this)))
        return;
}

HRESULT CTrailParticle::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    m_pShaderCom->Begin(4);
    
    m_pTransformCom;

    m_pVIBufferCom->Bind_Resources();

    m_pVIBufferCom->Render();

    return S_OK;
}

HRESULT CTrailParticle::Ready_Components()
{
    _uint iDestLevel = m_pGameInstance->Get_DestLevel();

    //if (FAILED(CGameObject::Add_Component(iDestLevel, TEXT("Prototype_Component_Shader_VtxInstance_PointParticle"),
    //    TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
    //    return E_FAIL;
    if (FAILED(CGameObject::Add_Component(iDestLevel, TEXT("Prototype_Component_Shader_VtxInstance_Particle"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(iDestLevel, TEXT("Prototype_Component_VIBuffer_Particle_TrailEffect"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(iDestLevel, TEXT("Prototype_Component_Texture_CircleEffect"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))
        return E_FAIL;
    //if (FAILED(CGameObject::Add_Component(iDestLevel, TEXT("Prototype_Component_Texture_Snow"),
    //    TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))
    //    return E_FAIL;

    return S_OK;
}

HRESULT CTrailParticle::Bind_ShaderResources()
{
    if (FAILED(m_pTransformCom->Bind_Shader_Resource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
        return E_FAIL;

    if (FAILED(m_pTextureCom->Bind_Shader_Resource(m_pShaderCom, "g_DiffuseTexture", 0)))
        return E_FAIL;

    return S_OK;
}

CTrailParticle* CTrailParticle::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CTrailParticle* pInstance = new CTrailParticle(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CTrailParticle"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CTrailParticle::Clone(void* pArg)
{
    CTrailParticle* pInstance = new CTrailParticle(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CTrailParticle"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CTrailParticle::Free()
{
    __super::Free();

    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pShaderCom);
}

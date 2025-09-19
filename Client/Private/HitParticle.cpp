#include "HitParticle.h"
#include "GameInstance.h"

CHitParticle::CHitParticle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject{ pDevice, pContext }
{
}

CHitParticle::CHitParticle(const CHitParticle& Prototype)
    : CGameObject{ Prototype }
{
}

HRESULT CHitParticle::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CHitParticle::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    return S_OK;
}

void CHitParticle::Priority_Update(_float fTimeDelta)
{
    int a = 10;
}

void CHitParticle::Update(_float fTimeDelta)
{
    m_pVIBufferCom->Spread_Turn(fTimeDelta);

    if (m_pVIBufferCom->Get_isLifeOver())
        m_isDead = true;
}

void CHitParticle::Late_Update(_float fTimeDelta)
{
    if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::NONLIGHT, this)))
        return;
}

HRESULT CHitParticle::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    m_pShaderCom->Begin(3);
    
    m_pTransformCom;

    m_pVIBufferCom->Bind_Resources();

    m_pVIBufferCom->Render();

    return S_OK;
}

HRESULT CHitParticle::Ready_Components()
{
    _uint iDestLevel = m_pGameInstance->Get_DestLevel();

    //if (FAILED(CGameObject::Add_Component(iDestLevel, TEXT("Prototype_Component_Shader_VtxInstance_PointParticle"),
    //    TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
    //    return E_FAIL;
    if (FAILED(CGameObject::Add_Component(iDestLevel, TEXT("Prototype_Component_Shader_VtxInstance_Particle"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(iDestLevel, TEXT("Prototype_Component_VIBuffer_Particle_HitEffect"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(iDestLevel, TEXT("Prototype_Component_Texture_TriEffect"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))
        return E_FAIL;
    //if (FAILED(CGameObject::Add_Component(iDestLevel, TEXT("Prototype_Component_Texture_Snow"),
    //    TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))
    //    return E_FAIL;

    return S_OK;
}

HRESULT CHitParticle::Bind_ShaderResources()
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

CHitParticle* CHitParticle::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CHitParticle* pInstance = new CHitParticle(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CHitParticle"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CHitParticle::Clone(void* pArg)
{
    CHitParticle* pInstance = new CHitParticle(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CHitParticle"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CHitParticle::Free()
{
    __super::Free();

    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pShaderCom);
}

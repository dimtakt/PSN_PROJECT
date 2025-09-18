#include "TriParticle.h"
#include "GameInstance.h"

CTriParticle::CTriParticle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject{ pDevice, pContext }
{
}

CTriParticle::CTriParticle(const CTriParticle& Prototype)
    : CGameObject{ Prototype }
{
}

HRESULT CTriParticle::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CTriParticle::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    return S_OK;
}

void CTriParticle::Priority_Update(_float fTimeDelta)
{
    int a = 10;
}

void CTriParticle::Update(_float fTimeDelta)
{
    m_pVIBufferCom->Spread(fTimeDelta);

    m_fElapsedTime += fTimeDelta;
    if (m_fElapsedTime >= m_fMaxTime)
        m_isDead = true;
}

void CTriParticle::Late_Update(_float fTimeDelta)
{
    if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::NONLIGHT, this)))
        return;
}

HRESULT CTriParticle::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    m_pShaderCom->Begin(2);
    
    m_pTransformCom;

    m_pVIBufferCom->Bind_Resources();

    m_pVIBufferCom->Render();

    return S_OK;
}

HRESULT CTriParticle::Ready_Components()
{
    _uint iDestLevel = m_pGameInstance->Get_DestLevel();

    //if (FAILED(CGameObject::Add_Component(iDestLevel, TEXT("Prototype_Component_Shader_VtxInstance_PointParticle"),
    //    TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
    //    return E_FAIL;
    if (FAILED(CGameObject::Add_Component(iDestLevel, TEXT("Prototype_Component_Shader_VtxInstance_Particle"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(iDestLevel, TEXT("Prototype_Component_VIBuffer_Particle_TriEffect"),
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

HRESULT CTriParticle::Bind_ShaderResources()
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

CTriParticle* CTriParticle::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CTriParticle* pInstance = new CTriParticle(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CTriParticle"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CTriParticle::Clone(void* pArg)
{
    CTriParticle* pInstance = new CTriParticle(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CTriParticle"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CTriParticle::Free()
{
    __super::Free();

    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pShaderCom);
}

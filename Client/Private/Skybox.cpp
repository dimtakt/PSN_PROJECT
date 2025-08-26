#include "Skybox.h"
#include "GameInstance.h"

CSkybox::CSkybox(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject{ pDevice, pContext }
{
    /*m_pGraphic_Device->SetRenderState(, );*/

    /*ID3D11RasterizerState* pRSState = { nullptr };
    ID3D11DepthStencilState* pRSState = { nullptr };
    ID3D11BlendState**/
    // D3D11_DEPTH_STENCIL_DESC
  //  D3D11_RASTERIZER_DESC

     /*D3D11_RASTERIZER_DESC   RSDesc{};
     m_pDevice->CreateRasterizerState(RSDesc, &pRSState);

     m_pContext->RSSetState(pRSState);
     m_pContext->OMSetBlendState();
     */
     // m_pContext->OMSetDepthStencilState();

      //D3D11_BLEND_DESC

      // m_pContext->OMSetBlendState(, , )

}

CSkybox::CSkybox(const CSkybox& Prototype)
    : CGameObject{ Prototype }
{
}

HRESULT CSkybox::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CSkybox::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    return S_OK;
}

void CSkybox::Priority_Update(_float fTimeDelta)
{
    int a = 10;
}

void CSkybox::Update(_float fTimeDelta)
{
    m_pTransformCom->Set_State(STATE::POSITION, XMLoadFloat4(m_pGameInstance->Get_CamPosition()));

}

void CSkybox::Late_Update(_float fTimeDelta)
{
    if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::PRIORITY, this)))
        return;
}

HRESULT CSkybox::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    m_pShaderCom->Begin(0);

    m_pVIBufferCom->Bind_Resources();

    m_pVIBufferCom->Render();

    return S_OK;
}

HRESULT CSkybox::Ready_Components()
{
    _uint iDestLevel = m_pGameInstance->Get_DestLevel();


    if (FAILED(CGameObject::Add_Component(iDestLevel, TEXT("Prototype_Component_Shader_VtxCube"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(iDestLevel, TEXT("Prototype_Component_VIBuffer_Cube"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(iDestLevel, TEXT("Prototype_Component_Texture_Sky"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))
        return E_FAIL;




    return S_OK;
}

HRESULT CSkybox::Bind_ShaderResources()
{
    if (FAILED(m_pTransformCom->Bind_Shader_Resource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
        return E_FAIL;

    if (FAILED(m_pTextureCom->Bind_Shader_Resource(m_pShaderCom, "g_Texture", 3)))
        return E_FAIL;

    return S_OK;
}

CSkybox* CSkybox::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CSkybox* pInstance = new CSkybox(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CSkybox"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CSkybox::Clone(void* pArg)
{
    CSkybox* pInstance = new CSkybox(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CSkybox"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CSkybox::Free()
{
    __super::Free();

    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pShaderCom);
}

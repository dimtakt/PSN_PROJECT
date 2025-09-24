#include "UI_ScreenFont.h"
#include "GameInstance.h"

CUI_ScreenFont::CUI_ScreenFont(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUIObject{ pDevice, pContext }
{
}

CUI_ScreenFont::CUI_ScreenFont(const CUI_ScreenFont& Prototype)
    : CUIObject{ Prototype }
{
}

HRESULT CUI_ScreenFont::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CUI_ScreenFont::Initialize(void* pArg)
{
    _float fSizeMultiply = 1.f;

    UIOBJECT_DESC               Desc{};
    Desc.fX = g_iWinSizeX >> 1;
    Desc.fY = g_iWinSizeY >> 1;
    Desc.fSizeX = g_iWinSizeX * fSizeMultiply;
    Desc.fSizeY = g_iWinSizeY * fSizeMultiply;

    if (FAILED(__super::Initialize(&Desc)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    return S_OK;
}

void CUI_ScreenFont::Priority_Update(_float fTimeDelta)
{
}

void CUI_ScreenFont::Update(_float fTimeDelta)
{

}

void CUI_ScreenFont::Late_Update(_float fTimeDelta)
{
    if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::UI, this)))
        return;
}

HRESULT CUI_ScreenFont::Render()
{
    __super::Begin();



    if (FAILED(m_pTransformCom->Bind_Shader_Resource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
        return E_FAIL;
    

    if (m_pTextureCom == nullptr)
        return E_FAIL;;

    if (FAILED(m_pTextureCom->Bind_Shader_Resource(m_pShaderCom, "g_Texture", 0)))
        return E_FAIL;

    m_pShaderCom->Begin(2);

    m_pVIBufferCom->Bind_Resources();

    m_pVIBufferCom->Render();

    return S_OK;
}

void CUI_ScreenFont::Render_ScreenFont(_uint iTextIndex)
{
    _wstring strFontTag = L"Font_DOS";
    _wstring strFontText = {};
    _float2 vTextPos = {};

    switch (iTextIndex)
    {
    case ENUM_CLASS(SCREENFONT_INDEX::TUTORIAL_LMBTOSHOOT):
    {
        strFontText = L"LEFT CLICK to shoot";
        vTextPos = { 500.f, 600.f };
    }break;
    case ENUM_CLASS(SCREENFONT_INDEX::LVLEND_SPACETO):      
    {
        strFontText = L"SPACE to hand over the control";
        vTextPos = { 450.f, 600.f };
    }break;
    default:
        break;
    }

    m_pGameInstance->Render_Font_Immediately(strFontTag, strFontText.c_str(), vTextPos);
}

HRESULT CUI_ScreenFont::Ready_Components()
{
    // 셰이더. 그리기 위해 기본적으로 필요.
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
        return E_FAIL;

    // Rect 모델 버퍼.
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
        return E_FAIL;

    _uint iDestLevel = m_pGameInstance->Get_DestLevel();
    if (FAILED(CGameObject::Add_Component(iDestLevel, TEXT("Prototype_Component_Texture_Blank"),
        TEXT("Com_Texture_NoAmmo"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))
        return E_FAIL;

    return S_OK;
}

CUI_ScreenFont* CUI_ScreenFont::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CUI_ScreenFont* pInstance = new CUI_ScreenFont(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CUI_ScreenFont"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CUI_ScreenFont::Clone(void* pArg)
{
    CUI_ScreenFont* pInstance = new CUI_ScreenFont(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CUI_ScreenFont"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CUI_ScreenFont::Free()
{
    __super::Free();

    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pShaderCom);
}

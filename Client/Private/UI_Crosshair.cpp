#include "UI_Crosshair.h"
#include "GameInstance.h"

CUI_Crosshair::CUI_Crosshair(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUIObject{ pDevice, pContext }
{
}

CUI_Crosshair::CUI_Crosshair(const CUI_Crosshair& Prototype)
    : CUIObject{ Prototype }
{
}

HRESULT CUI_Crosshair::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CUI_Crosshair::Initialize(void* pArg)
{
    _float fSize = 50.f;

    UIOBJECT_DESC               Desc{};
    Desc.fX = g_iWinSizeX >> 1;
    Desc.fY = g_iWinSizeY >> 1;
    Desc.fSizeX = fSize;
    Desc.fSizeY = fSize;

    if (FAILED(__super::Initialize(&Desc)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_pCurTextureCom = m_pTextureCom_Gun;

    return S_OK;
}

void CUI_Crosshair::Priority_Update(_float fTimeDelta)
{
    int a = 10;
}

void CUI_Crosshair::Update(_float fTimeDelta)
{
    int a = 10;
}

void CUI_Crosshair::Late_Update(_float fTimeDelta)
{
    if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::UI, this)))
        return;
}

HRESULT CUI_Crosshair::Render()
{
    __super::Begin();

    m_pTransformCom->Rotation(XMVectorSet(0.0f, 0.f, 1.f, 1.0f), TO_RAD(-m_fRotDeg));

    if (FAILED(m_pTransformCom->Bind_Shader_Resource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
        return E_FAIL;


    // 조건에 따라 바뀌도록 할 것
    if (m_pCurTextureCom == nullptr)
        return E_FAIL;;

    if (FAILED(m_pCurTextureCom->Bind_Shader_Resource(m_pShaderCom, "g_Texture", 0)))
        return E_FAIL;

    m_pShaderCom->Begin(2);

    m_pVIBufferCom->Bind_Resources();

    m_pVIBufferCom->Render();

    return S_OK;
}

void CUI_Crosshair::Change_Crosshair(_uint iTexIndex)
{
    switch (iTexIndex)
    {
    case ENUM_CLASS(CROSSHAIR_INDEX::BASICHAND):       m_pCurTextureCom = m_pTextureCom_BasicHand;          break;
    case ENUM_CLASS(CROSSHAIR_INDEX::BASICDOT):        m_pCurTextureCom = m_pTextureCom_BasicDot;           break;
    case ENUM_CLASS(CROSSHAIR_INDEX::BASICPUNCH):      m_pCurTextureCom = m_pTextureCom_BasicPunch;         break;
    case ENUM_CLASS(CROSSHAIR_INDEX::GUN):             m_pCurTextureCom = m_pTextureCom_Gun;                break;
    case ENUM_CLASS(CROSSHAIR_INDEX::KATANA):          m_pCurTextureCom = m_pTextureCom_Katana;             break;
    case ENUM_CLASS(CROSSHAIR_INDEX::HS_HOVER):        m_pCurTextureCom = m_pTextureCom_HotswitchHover;     break;
    case ENUM_CLASS(CROSSHAIR_INDEX::HS_IDLE):         m_pCurTextureCom = m_pTextureCom_HotswitchIdle;      break;
    case ENUM_CLASS(CROSSHAIR_INDEX::HS_WAIT):         m_pCurTextureCom = m_pTextureCom_HotswitchWait;      break;
    
    case ENUM_CLASS(CROSSHAIR_INDEX::END):
    default:
        break;
    }
}

void CUI_Crosshair::Change_RotByCD(_float fCDRatio)
{
    m_fRotDeg = fCDRatio * 90.f;
}

HRESULT CUI_Crosshair::Ready_Components()
{
    // 셰이더. 그리기 위해 기본적으로 필요.
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
        return E_FAIL;

    // Rect 모델 버퍼.
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
        return E_FAIL;

    //if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::LOGO), TEXT("Prototype_Component_Texture_UI_Crosshair"),
    //    TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))
    //    return E_FAIL;

    // 텍스쳐.
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Crosshair_BasicHand"),
        TEXT("Com_Texture_BasicHand"), reinterpret_cast<CComponent**>(&m_pTextureCom_BasicHand), nullptr)))
        return E_FAIL;
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Crosshair_BasicDot"),
        TEXT("Com_Texture_BasicDot"), reinterpret_cast<CComponent**>(&m_pTextureCom_BasicDot), nullptr)))
        return E_FAIL;
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Crosshair_BasicPunch"),
        TEXT("Com_Texture_BasicPunch"), reinterpret_cast<CComponent**>(&m_pTextureCom_BasicPunch), nullptr)))
        return E_FAIL;
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Crosshair_Gun"),
        TEXT("Com_Texture_Gun"), reinterpret_cast<CComponent**>(&m_pTextureCom_Gun), nullptr)))
        return E_FAIL;
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Crosshair_Katana"),
        TEXT("Com_Texture_Katana"), reinterpret_cast<CComponent**>(&m_pTextureCom_Katana), nullptr)))
        return E_FAIL;
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Crosshair_HotswitchHover"),
        TEXT("Com_Texture_HotswitchHover"), reinterpret_cast<CComponent**>(&m_pTextureCom_HotswitchHover), nullptr)))
        return E_FAIL;
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Crosshair_HotswitchIdle"),
        TEXT("Com_Texture_HotswitchIdle"), reinterpret_cast<CComponent**>(&m_pTextureCom_HotswitchIdle), nullptr)))
        return E_FAIL;
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Crosshair_HotswitchWait"),
        TEXT("Com_Texture_HotswitchWait"), reinterpret_cast<CComponent**>(&m_pTextureCom_HotswitchWait), nullptr)))
        return E_FAIL;



    return S_OK;
}

CUI_Crosshair* CUI_Crosshair::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CUI_Crosshair* pInstance = new CUI_Crosshair(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CUI_Crosshair"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CUI_Crosshair::Clone(void* pArg)
{
    CUI_Crosshair* pInstance = new CUI_Crosshair(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CUI_Crosshair"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CUI_Crosshair::Free()
{
    __super::Free();

    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pShaderCom);

    Safe_Release(m_pTextureCom_BasicHand);
    Safe_Release(m_pTextureCom_BasicDot);
    Safe_Release(m_pTextureCom_BasicPunch);
    Safe_Release(m_pTextureCom_Gun);
    Safe_Release(m_pTextureCom_Katana);
    Safe_Release(m_pTextureCom_HotswitchHover);
    Safe_Release(m_pTextureCom_HotswitchIdle);
    Safe_Release(m_pTextureCom_HotswitchWait);

}

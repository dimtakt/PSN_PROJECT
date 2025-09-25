#include "UI_ScreenText.h"
#include "GameInstance.h"

CUI_ScreenText::CUI_ScreenText(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUIObject{ pDevice, pContext }
{
}

CUI_ScreenText::CUI_ScreenText(const CUI_ScreenText& Prototype)
    : CUIObject{ Prototype }
{
}

HRESULT CUI_ScreenText::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CUI_ScreenText::Initialize(void* pArg)
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


    m_pCurTextureCom /*= m_pTextureCom_Gun*/;

    return S_OK;
}

void CUI_ScreenText::Priority_Update(_float fTimeDelta)
{
    if (!m_isActive)
    {
        return;
    }

    
    if (
        m_pCurTextureCom == m_pTextureCom_NoAmmo
        )
    {
        m_pGameInstance->Req_EditTimeSpeed(0.01f, true);
    }

    
}

void CUI_ScreenText::Update(_float fTimeDelta)
{
    if (
        m_pCurTextureCom == m_pTextureCom_NoAmmo ||
        m_pCurTextureCom == m_pTextureCom_TimeMoves ||
        m_pCurTextureCom == m_pTextureCom_LVLEnd_Super ||
        m_pCurTextureCom == m_pTextureCom_LVLEnd_Hot
        )
    {
        Update_PopupUI(fTimeDelta);
    }

    else if (
        m_pCurTextureCom == m_pTextureCom_TUTO_LeftClick
        )
    {
        Update_FloatUI(fTimeDelta);
    }




}

void CUI_ScreenText::Late_Update(_float fTimeDelta)
{
    if (!m_isActive)
        return;

    if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::UI, this)))
        return;
}

HRESULT CUI_ScreenText::Render()
{
    __super::Begin();



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

void CUI_ScreenText::Change_ScreenText(_uint iTexIndex)
{
    CTexture* pPrevTextureCom = m_pCurTextureCom;

    switch (iTexIndex)
    {
    case ENUM_CLASS(SCREENTEXT_INDEX::NOAMMO):              m_pCurTextureCom = m_pTextureCom_NoAmmo;            break;
    case ENUM_CLASS(SCREENTEXT_INDEX::TIMEMOVES):           m_pCurTextureCom = m_pTextureCom_TimeMoves;         break;

    case ENUM_CLASS(SCREENTEXT_INDEX::TUTO_LEFTCLICK):      m_pCurTextureCom = m_pTextureCom_TUTO_LeftClick;    break;

    case ENUM_CLASS(SCREENTEXT_INDEX::LVLSTART_10_1):       m_pCurTextureCom = m_pTextureCom_LVLStart_10_1;     break;
    case ENUM_CLASS(SCREENTEXT_INDEX::LVLSTART_10_2):       m_pCurTextureCom = m_pTextureCom_LVLStart_10_2;     break;
    case ENUM_CLASS(SCREENTEXT_INDEX::LVLMID_10_1):         m_pCurTextureCom = m_pTextureCom_LVLMid_10_1;       break;
    case ENUM_CLASS(SCREENTEXT_INDEX::LVLMID_10_2):         m_pCurTextureCom = m_pTextureCom_LVLMid_10_2;       break;
    case ENUM_CLASS(SCREENTEXT_INDEX::LVLMID_10_3):         m_pCurTextureCom = m_pTextureCom_LVLMid_10_3;       break;

    case ENUM_CLASS(SCREENTEXT_INDEX::LVLEND_SUPER):        m_pCurTextureCom = m_pTextureCom_LVLEnd_Super;      break;
    case ENUM_CLASS(SCREENTEXT_INDEX::LVLEND_HOT):          m_pCurTextureCom = m_pTextureCom_LVLEnd_Hot;        break;

    default:                                                                                                    break;
    }
}

void CUI_ScreenText::Change_Size(_float fSizeMultiply)
{
    m_fSizeX = g_iWinSizeX * fSizeMultiply;
    m_fSizeY = g_iWinSizeY * fSizeMultiply;
}

void CUI_ScreenText::Show_ScreenText(_uint iTexIndex)
{
    m_isActive = true;
    m_fActiveElapsed = 0.f;

    Change_ScreenText(iTexIndex);

    switch (iTexIndex)
    {
    case ENUM_CLASS(SCREENTEXT_INDEX::TUTO_LEFTCLICK):     
        break;

    default:
        m_pGameInstance->PlaySoundFixed(L"R_Migawka_L.ogg", ENUM_CLASS(SOUNDCH::SOUND_MAINUI)); 
        break;
    }
    
}

HRESULT CUI_ScreenText::Ready_Components()
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


    // 나중에 텍스쳐 할당 필요
    _uint iDestLevel = m_pGameInstance->Get_DestLevel();

    if (FAILED(CGameObject::Add_Component(iDestLevel, TEXT("Prototype_Component_Texture_NoAmmo"),
        TEXT("Com_Texture_NoAmmo"), reinterpret_cast<CComponent**>(&m_pTextureCom_NoAmmo), nullptr)))
        return E_FAIL;
    if (FAILED(CGameObject::Add_Component(iDestLevel, TEXT("Prototype_Component_Texture_TimeMoves"),
        TEXT("Com_Texture_TimeMoves"), reinterpret_cast<CComponent**>(&m_pTextureCom_TimeMoves), nullptr)))
        return E_FAIL;
    if (FAILED(CGameObject::Add_Component(iDestLevel, TEXT("Prototype_Component_Texture_TUTO_LeftClick"),
        TEXT("Com_Texture_TUTO_LeftClick"), reinterpret_cast<CComponent**>(&m_pTextureCom_TUTO_LeftClick), nullptr)))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(iDestLevel, TEXT("Prototype_Component_Texture_LVLSTART_10_1"),
        TEXT("Com_Texture_TUTO_LeftClick"), reinterpret_cast<CComponent**>(&m_pTextureCom_LVLStart_10_1), nullptr)))
        return E_FAIL;
    if (FAILED(CGameObject::Add_Component(iDestLevel, TEXT("Prototype_Component_Texture_LVLSTART_10_2"),
        TEXT("Com_Texture_TUTO_LeftClick"), reinterpret_cast<CComponent**>(&m_pTextureCom_LVLStart_10_2), nullptr)))
        return E_FAIL;
    if (FAILED(CGameObject::Add_Component(iDestLevel, TEXT("Prototype_Component_Texture_LVLMID_10_1"),
        TEXT("Com_Texture_TUTO_LeftClick"), reinterpret_cast<CComponent**>(&m_pTextureCom_LVLMid_10_1), nullptr)))
        return E_FAIL;
    if (FAILED(CGameObject::Add_Component(iDestLevel, TEXT("Prototype_Component_Texture_LVLMID_10_2"),
        TEXT("Com_Texture_TUTO_LeftClick"), reinterpret_cast<CComponent**>(&m_pTextureCom_LVLMid_10_2), nullptr)))
        return E_FAIL;
    if (FAILED(CGameObject::Add_Component(iDestLevel, TEXT("Prototype_Component_Texture_LVLMID_10_3"),
        TEXT("Com_Texture_TUTO_LeftClick"), reinterpret_cast<CComponent**>(&m_pTextureCom_LVLMid_10_3), nullptr)))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(iDestLevel, TEXT("Prototype_Component_Texture_LVLEnd_Super"),
        TEXT("Com_Texture_LVLEnd_Super"), reinterpret_cast<CComponent**>(&m_pTextureCom_LVLEnd_Super), nullptr)))
        return E_FAIL;
    if (FAILED(CGameObject::Add_Component(iDestLevel, TEXT("Prototype_Component_Texture_LVLEnd_Hot"),
        TEXT("Com_Texture_LVLEnd_Hot"), reinterpret_cast<CComponent**>(&m_pTextureCom_LVLEnd_Hot), nullptr)))
        return E_FAIL;


    return S_OK;
}

void CUI_ScreenText::Update_PopupUI(_float fTimeDelta)
{
    //_float fRawTimeDelta = fTimeDelta / (m_pGameInstance->Get_TimeSpeed());
    _float fRawTimeDelta = m_pGameInstance->Get_RawTimeDelta();

    const _float fActiveTime = 1.2f;
    const _float fSizeTo = 0.95f;
    m_fActiveElapsed;

    // 시간이 지남에 따라 1.0 -> 0.8
    Change_Size(1.f - (min((m_fActiveElapsed / fActiveTime), 1.f) * (1.f - fSizeTo)));


    // 이동안에는 플레이어도 느려지게 해야 할 듯?
    if (m_isActive)
    {
        m_fActiveElapsed += fRawTimeDelta;

        if (m_fActiveElapsed > fActiveTime)
        {
            m_fActiveElapsed = 0.f;
            m_isActive = false;
        }
        else if (m_pGameInstance->Get_IsKeyDown(MOUSEKEYSTATE::LB) && (m_fActiveElapsed > (fActiveTime / 2.f)))
        {
            m_fActiveElapsed = 0.f;
            m_isActive = false;
        }
    }
}

void CUI_ScreenText::Update_FloatUI(_float fTimeDelta)
{
    //_float fRawTimeDelta = fTimeDelta / (m_pGameInstance->Get_TimeSpeed());
    _float fRawTimeDelta = m_pGameInstance->Get_RawTimeDelta();

    const _float fMoveTime = 1.2f;
    const _float fSizeTo = 1.f;
    m_fActiveElapsed;

    // 시간이 지남에 따라 1.5 -> 1.0
    Change_Size(1.f - (min((m_fActiveElapsed / fMoveTime), 1.f) * (1.5f - fSizeTo)));

    // 이동안에는 플레이어도 느려지게 해야 할 듯?
    if (m_isActive)
    {
        if (m_pGameInstance->Get_IsKeyDown(MOUSEKEYSTATE::LB) && (m_fActiveElapsed > (fMoveTime / 2.f)))
        {
            m_fActiveElapsed = 0.f;
            m_isActive = false;
        }
    }
}

CUI_ScreenText* CUI_ScreenText::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CUI_ScreenText* pInstance = new CUI_ScreenText(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CUI_ScreenText"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CUI_ScreenText::Clone(void* pArg)
{
    CUI_ScreenText* pInstance = new CUI_ScreenText(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CUI_ScreenText"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CUI_ScreenText::Free()
{
    __super::Free();

    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pShaderCom);

    Safe_Release(m_pTextureCom_LVLStart_10_1);
    Safe_Release(m_pTextureCom_LVLStart_10_2);
    Safe_Release(m_pTextureCom_LVLMid_10_1);
    Safe_Release(m_pTextureCom_LVLMid_10_2);
    Safe_Release(m_pTextureCom_LVLMid_10_3);

    Safe_Release(m_pTextureCom_NoAmmo);
    Safe_Release(m_pTextureCom_TimeMoves);
    Safe_Release(m_pTextureCom_TUTO_LeftClick);
    Safe_Release(m_pTextureCom_LVLEnd_Super);
    Safe_Release(m_pTextureCom_LVLEnd_Hot);
}

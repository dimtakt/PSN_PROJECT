#include "BackGround.h"
#include "GameInstance.h"

CBackGround::CBackGround(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUIObject { pDevice, pContext }
{
}

CBackGround::CBackGround(const CBackGround& Prototype)
    : CUIObject{ Prototype }
{
}

HRESULT CBackGround::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CBackGround::Initialize(void* pArg)
{
    UIOBJECT_DESC               Desc{};
    Desc.fX = g_iWinSizeX >> 1;
    Desc.fY = g_iWinSizeY >> 1;
    Desc.fSizeX = g_iWinSizeX;
    Desc.fSizeY = g_iWinSizeY;

    if (FAILED(__super::Initialize(&Desc)))
        return E_FAIL;

    if (FAILED(Ready_Components(pArg)))
        return E_FAIL;

    return S_OK;
}

void CBackGround::Priority_Update(_float fTimeDelta)
{
    int a = 10;
}

void CBackGround::Update(_float fTimeDelta)
{
    int a = 10;
}

void CBackGround::Late_Update(_float fTimeDelta)
{
    if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::UI, this)))
        return;
}

HRESULT CBackGround::Render()
{
    /*
    m_pShaderCom->Bind_Texture();*/

    __super::Begin();

    if (FAILED(m_pTransformCom->Bind_Shader_Resource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
        return E_FAIL;

    // 이걸로 텍스쳐 삽입. 이를 크로스헤어 출력 시에 사용하면 될 듯
    if (FAILED(m_pTextureCom->Bind_Shader_Resource(m_pShaderCom, "g_Texture", 0)))
        return E_FAIL;

    m_pShaderCom->Begin(0);    

    // 셰이더에 삽입된 텍스쳐를 이용 바인딩
    m_pVIBufferCom->Bind_Resources();

    m_pVIBufferCom->Render();

    return S_OK;
}

HRESULT CBackGround::Ready_Components(void* pArg)
{
    UIOBJECT_DESC* pDesc = static_cast<UIOBJECT_DESC*>(pArg);

    _uint iDestLevel = m_pGameInstance->Get_DestLevel();
    if (pArg != nullptr && pDesc->iCustomLoadLevelIndex != UINT_MAX)
        iDestLevel = pDesc->iCustomLoadLevelIndex;


    // 셰이더. 그리기 위해 기본적으로 필요.
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
        return E_FAIL;

    // Rect 모델 버퍼.
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
        return E_FAIL;

    //if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::LOGO), TEXT("Prototype_Component_Texture_BackGround"),
    //    TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))
    //    return E_FAIL;

    // 텍스쳐.
    
    if (FAILED(CGameObject::Add_Component(iDestLevel, TEXT("Prototype_Component_Texture_Black"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))
        return E_FAIL;

    

    return S_OK;
}

CBackGround* CBackGround::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CBackGround* pInstance = new CBackGround(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CBackGround"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CBackGround::Clone(void* pArg)
{
    CBackGround* pInstance = new CBackGround(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CBackGround"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CBackGround::Free()
{
    __super::Free();

    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pShaderCom);
}

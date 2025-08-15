#include "CustomObj_NonAnim.h"
#include "Gameinstance.h"


CCustomObj_NonAnim::CCustomObj_NonAnim(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject(pDevice, pContext)
{
}

CCustomObj_NonAnim::CCustomObj_NonAnim(const CGameObject& Prototype)
    : CGameObject(Prototype)
{
}

HRESULT CCustomObj_NonAnim::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CCustomObj_NonAnim::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;
    if (FAILED(this->Ready_Components(pArg)))
        return E_FAIL;

    return S_OK;
}

void CCustomObj_NonAnim::Priority_Update(_float fTimeDelta)
{
    __super::Priority_Update(fTimeDelta);
}

void CCustomObj_NonAnim::Update(_float fTimeDelta)
{
}

void CCustomObj_NonAnim::Late_Update(_float fTimeDelta)
{
    __super::Late_Update(fTimeDelta);

    if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::NONBLEND, this)))
        return;
}

HRESULT CCustomObj_NonAnim::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    _uint           iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (_uint i = 0; i < iNumMeshes; i++)
    {
        if (FAILED(m_pModelCom->Bind_Materials(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE, 0)))
            return E_FAIL;

        //if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i)))
        //    return E_FAIL;

        m_pShaderCom->Begin(0);

        m_pModelCom->Render(i);


    }

    return S_OK;
}

HRESULT CCustomObj_NonAnim::Ready_Components(void* pArg)
{
    CUSTOMOBJ_NA_DESC* pDesc = static_cast<CUSTOMOBJ_NA_DESC*>(pArg);

    _wstring strPrototypeName = pDesc->strModelComPrototypeTag;
    m_iGameObjType = pDesc->iGameObjType;

    //LEVEL eLevel;
    //// ksta : 임시조치. 에디터에선 에디터로, 아니면 게임플레이로. 추후 수정 필요.
    //if (m_pGameInstance->Get_CurLevel() == ENUM_CLASS(LEVEL::EDITOR))
    //    eLevel = LEVEL::EDITOR;
    //else
    //    eLevel = LEVEL::GAMEPLAY;

    _uint iDestLevelIndex = m_pGameInstance->Get_DestLevel();

    if (FAILED(CGameObject::Add_Component(iDestLevelIndex, TEXT("Prototype_Component_Shader_VtxMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(iDestLevelIndex, strPrototypeName,
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom), nullptr)))
        return E_FAIL;
    Set_BufferRef(m_pModelCom);

    return S_OK;
}

HRESULT CCustomObj_NonAnim::Bind_ShaderResources()
{
    if (FAILED(m_pTransformCom->Bind_Shader_Resource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
        return E_FAIL;

    const LIGHT_DESC* pLightDesc = m_pGameInstance->Get_LightDesc(0);
    if (nullptr == pLightDesc)
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightDir", &pLightDesc->vDirection, sizeof(_float4))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightDiffuse", &pLightDesc->vDiffuse, sizeof(_float4))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightAmbient", &pLightDesc->vAmbient, sizeof(_float4))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightSpecular", &pLightDesc->vSpecular, sizeof(_float4))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition(), sizeof(_float4))))
        return E_FAIL;

    return S_OK;
}


CCustomObj_NonAnim* CCustomObj_NonAnim::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CCustomObj_NonAnim* pInstance = new CCustomObj_NonAnim(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Create : CCustomObj_NonAnim"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CCustomObj_NonAnim* CCustomObj_NonAnim::Clone(void* pArg)
{
    CCustomObj_NonAnim* pInstance = new CCustomObj_NonAnim(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Clone : CCustomObj_NonAnim"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CCustomObj_NonAnim::Free()
{
    __super::Free();

    Safe_Release(m_pModelCom);
    Safe_Release(m_pShaderCom);
}

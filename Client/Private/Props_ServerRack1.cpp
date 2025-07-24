#include "Props_ServerRack1.h"
#include "Gameinstance.h"

CProps_ServerRack1::CProps_ServerRack1(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject(pDevice, pContext)
{
}

CProps_ServerRack1::CProps_ServerRack1(const CGameObject& Prototype)
    : CGameObject(Prototype)
{
}

HRESULT CProps_ServerRack1::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CProps_ServerRack1::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;
    if (FAILED(this->Ready_Components(pArg)))
        return E_FAIL;

    return S_OK;
}

void CProps_ServerRack1::Priority_Update(_float fTimeDelta)
{
    __super::Priority_Update(fTimeDelta);
}

void CProps_ServerRack1::Update(_float fTimeDelta)
{
}

void CProps_ServerRack1::Late_Update(_float fTimeDelta)
{
    __super::Late_Update(fTimeDelta);

    if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::NONBLEND, this)))
        return;
}

HRESULT CProps_ServerRack1::Render()
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

HRESULT CProps_ServerRack1::Ready_Components(void* pArg)
{
    LEVEL eLevel;
    // ksta : 임시조치. 에디터에선 에디터로, 아니면 게임플레이로. 추후 수정 필요.
    if (m_pGameInstance->Get_CurLevel() == ENUM_CLASS(LEVEL::EDITOR))
        eLevel = LEVEL::EDITOR;
    else
        eLevel = LEVEL::GAMEPLAY;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(eLevel), TEXT("Prototype_Component_Shader_VtxMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_Props_ServerRack1"),
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom), nullptr)))
        return E_FAIL;
    Set_BufferRef(m_pModelCom);

    return S_OK;
}

HRESULT CProps_ServerRack1::Bind_ShaderResources()
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


CProps_ServerRack1* CProps_ServerRack1::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CProps_ServerRack1* pInstance = new CProps_ServerRack1(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Create : CProps_ServerRack1"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CProps_ServerRack1* CProps_ServerRack1::Clone(void* pArg)
{
    CProps_ServerRack1* pInstance = new CProps_ServerRack1(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Clone : CProps_ServerRack1"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CProps_ServerRack1::Free()
{
    __super::Free();

    Safe_Release(m_pModelCom);
    Safe_Release(m_pShaderCom);
}

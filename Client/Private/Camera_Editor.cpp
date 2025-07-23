#include "Camera_Editor.h"
#include "GameInstance.h"


CCamera_Editor::CCamera_Editor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CCamera{ pDevice, pContext }
{
}

CCamera_Editor::CCamera_Editor(const CCamera_Editor& Prototype)
    : CCamera{ Prototype }
{
}

HRESULT CCamera_Editor::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CCamera_Editor::Initialize(void* pArg)
{
    CAMERA_EDITOR_DESC* pDesc = static_cast<CAMERA_EDITOR_DESC*>(pArg);

    m_fMouseSensor = pDesc->fMouseSensor;

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    return S_OK;
}

void CCamera_Editor::Priority_Update(_float fTimeDelta)
{
    // * 키보드 WASD 에 의한 카메라 이동
    _float fSpeed = 10.f;

    if (m_pGameInstance->Get_IsKeyPressing(DIK_W))
        m_pTransformCom->Go_Straight(fTimeDelta * fSpeed);
    if (m_pGameInstance->Get_IsKeyPressing(DIK_S))
        m_pTransformCom->Go_Backward(fTimeDelta * fSpeed);
    if (m_pGameInstance->Get_IsKeyPressing(DIK_A))
        m_pTransformCom->Go_Left(fTimeDelta * fSpeed);
    if (m_pGameInstance->Get_IsKeyPressing(DIK_D))
        m_pTransformCom->Go_Right(fTimeDelta * fSpeed);



    // * 마우스 움직임에 의한 카메라 회전

    _int    iMouseMove = {};

    if (m_pGameInstance->Get_IsKeyPressing(MOUSEKEYSTATE::RB))        // 우클릭 중일 때만 회전
    {
        // 항시 글로벌 Y축에 따라 회전해야 하므로 Y축을 고정적인 회전 기준점으로 둠
        if (iMouseMove = m_pGameInstance->Get_DIMouseMove(MOUSEMOVESTATE::X))
            m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * iMouseMove * m_fMouseSensor);

        // Y축 회전값에 따라 변하는, 로컬 X축에 따라 회전해야 하므로
        // Get_State로 RIGHT 값을 받아와 사용
        if (iMouseMove = m_pGameInstance->Get_DIMouseMove(MOUSEMOVESTATE::Y))
            m_pTransformCom->Turn(m_pTransformCom->Get_State(STATE::RIGHT), fTimeDelta * iMouseMove * m_fMouseSensor);
    }
    else if (m_pGameInstance->Get_IsKeyPressing(MOUSEKEYSTATE::MB))   // 휠클릭 중일 때만 드래그 이동
    {
        if (iMouseMove = m_pGameInstance->Get_DIMouseMove(MOUSEMOVESTATE::X))
            m_pTransformCom->Go_Right(-fTimeDelta * iMouseMove * m_fMouseSensor * fSpeed);
        if (iMouseMove = m_pGameInstance->Get_DIMouseMove(MOUSEMOVESTATE::Y))
            m_pTransformCom->Go_Above(fTimeDelta * iMouseMove * m_fMouseSensor * fSpeed);
    }


    __super::Update_PipeLines();
}

void CCamera_Editor::Update(_float fTimeDelta)
{
}

void CCamera_Editor::Late_Update(_float fTimeDelta)
{
}

HRESULT CCamera_Editor::Render()
{
    return S_OK;
}

CCamera_Editor* CCamera_Editor::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CCamera_Editor* pInstance = new CCamera_Editor(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CCamera_Editor"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CCamera_Editor::Clone(void* pArg)
{
    CCamera_Editor* pInstance = new CCamera_Editor(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CCamera_Editor"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CCamera_Editor::Free()
{
    __super::Free();


}

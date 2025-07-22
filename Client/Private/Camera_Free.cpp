#include "Camera_Free.h"
#include "GameInstance.h"

CCamera_Free::CCamera_Free(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CCamera{ pDevice, pContext }
{
}

CCamera_Free::CCamera_Free(const CCamera_Free& Prototype)
    : CCamera{ Prototype }
{
}

HRESULT CCamera_Free::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CCamera_Free::Initialize(void* pArg)
{
    CAMERA_FREE_DESC* pDesc = static_cast<CAMERA_FREE_DESC*>(pArg);

    m_fMouseSensor = pDesc->fMouseSensor;

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    return S_OK;
}

void CCamera_Free::Priority_Update(_float fTimeDelta)
{
    // * 키보드 WASD 에 의한 카메라 이동

    //if (m_pGameInstance->Get_DIKeyState(DIK_W) & 0x80)
    //    m_pTransformCom->Go_Straight(fTimeDelta);
    //if (m_pGameInstance->Get_DIKeyState(DIK_S) & 0x80)
    //    m_pTransformCom->Go_Backward(fTimeDelta);
    //if (m_pGameInstance->Get_DIKeyState(DIK_A) & 0x80)
    //    m_pTransformCom->Go_Left(fTimeDelta);
    //if (m_pGameInstance->Get_DIKeyState(DIK_D) & 0x80)
    //    m_pTransformCom->Go_Right(fTimeDelta);

    if (m_pGameInstance->Get_IsKeyPressing(DIK_W))
        m_pTransformCom->Go_Straight(fTimeDelta);
    if (m_pGameInstance->Get_IsKeyPressing(DIK_S))
        m_pTransformCom->Go_Backward(fTimeDelta);
    if (m_pGameInstance->Get_IsKeyPressing(DIK_A))
        m_pTransformCom->Go_Left(fTimeDelta);
    if (m_pGameInstance->Get_IsKeyPressing(DIK_D))
        m_pTransformCom->Go_Right(fTimeDelta);

    //if (m_pGameInstance->Get_IsKeyUp(DIK_P) ||
    //    m_pGameInstance->Get_IsKeyDown(DIK_P))
    //    m_pTransformCom->Go_Straight(fTimeDelta * 20);

    // * 마우스 움직임에 의한 카메라 회전

    _int    iMouseMove = {};

    // 항시 글로벌 Y축에 따라 회전해야 하므로 Y축을 고정적인 회전 기준점으로 둠
    if (iMouseMove = m_pGameInstance->Get_DIMouseMove(MOUSEMOVESTATE::X))
        m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * iMouseMove * m_fMouseSensor);

    // Y축 회전값에 따라 변하는, 로컬 X축에 따라 회전해야 하므로
    // Get_State로 RIGHT 값을 받아와 사용
    if (iMouseMove = m_pGameInstance->Get_DIMouseMove(MOUSEMOVESTATE::Y))
        m_pTransformCom->Turn(m_pTransformCom->Get_State(STATE::RIGHT), fTimeDelta * iMouseMove * m_fMouseSensor);


    __super::Update_PipeLines();


}

void CCamera_Free::Update(_float fTimeDelta)
{
}

void CCamera_Free::Late_Update(_float fTimeDelta)
{
}

HRESULT CCamera_Free::Render()
{
    return S_OK;
}

CCamera_Free* CCamera_Free::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CCamera_Free* pInstance = new CCamera_Free(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CCamera_Free"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CCamera_Free::Clone(void* pArg)
{
    CCamera_Free* pInstance = new CCamera_Free(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CCamera_Free"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CCamera_Free::Free()
{
    __super::Free();


}

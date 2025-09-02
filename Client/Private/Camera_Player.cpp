#include "Camera_Player.h"
#include "GameInstance.h"

CCamera_Player::CCamera_Player(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CCamera{ pDevice, pContext }
{
}

CCamera_Player::CCamera_Player(const CCamera_Player& Prototype)
    : CCamera{ Prototype }
{
}

HRESULT CCamera_Player::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CCamera_Player::Initialize(void* pArg)
{
    Camera_Player_DESC* pDesc = static_cast<Camera_Player_DESC*>(pArg);

    m_fMouseSensor = pDesc->fMouseSensor;

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    return S_OK;
}

void CCamera_Player::Priority_Update(_float fTimeDelta)
{
    // * 키보드 WASD 에 의한 카메라 이동
    
    if (m_pGameInstance->Get_IsKeyDown(DIK_TAB))
        m_isFreeMode = !m_isFreeMode;
    
    
    if (m_isFreeMode)
    {
        if (m_pGameInstance->Get_IsKeyPressing(DIK_UP))
            m_pTransformCom->Go_Straight(fTimeDelta);
        if (m_pGameInstance->Get_IsKeyPressing(DIK_DOWN))
            m_pTransformCom->Go_Backward(fTimeDelta);
        if (m_pGameInstance->Get_IsKeyPressing(DIK_LEFT))
            m_pTransformCom->Go_Left(fTimeDelta);
        if (m_pGameInstance->Get_IsKeyPressing(DIK_RIGHT))
            m_pTransformCom->Go_Right(fTimeDelta);
    }

    else
    {
        _uint iDestLevel = m_pGameInstance->Get_DestLevel();
        m_pPlayerTransformCom = dynamic_cast<CTransform*>(m_pGameInstance->Find_Component(iDestLevel, L"Layer_Player", L"Com_Transform"));

        _float fPlayerHeight = 7.8f;

        _vector vPlayerPos = m_pPlayerTransformCom->Get_State(STATE::POSITION);
        _vector pCalcedPlayerPos = vPlayerPos + XMVectorSet(0.f, fPlayerHeight, 0.f, 0.f);

        m_pTransformCom->Set_State(STATE::POSITION, pCalcedPlayerPos);
    }


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

void CCamera_Player::Update(_float fTimeDelta)
{
}

void CCamera_Player::Late_Update(_float fTimeDelta)
{
}

HRESULT CCamera_Player::Render()
{
    return S_OK;
}

CCamera_Player* CCamera_Player::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CCamera_Player* pInstance = new CCamera_Player(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CCamera_Player"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CCamera_Player::Clone(void* pArg)
{
    CCamera_Player* pInstance = new CCamera_Player(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CCamera_Player"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CCamera_Player::Free()
{
    __super::Free();


}

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
    CAMERA_PLAYER_DESC* pDesc = static_cast<CAMERA_PLAYER_DESC*>(pArg);

    m_fMouseSensor = pDesc->fMouseSensor;

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    return S_OK;
}

void CCamera_Player::Priority_Update(_float fTimeDelta)
{
    _float fRawTimeDelta = fTimeDelta / (m_pGameInstance->Get_TimeSpeed());

    // * 키보드 WASD 에 의한 카메라 이동
    
    if (m_pGameInstance->Get_IsKeyDown(DIK_TAB))
        m_isFreeMode = !m_isFreeMode;
    
    
    if (m_isFreeMode)
    {
        if (m_pGameInstance->Get_IsKeyPressing(DIK_UP))
            m_pTransformCom->Go_Straight(fRawTimeDelta);
        if (m_pGameInstance->Get_IsKeyPressing(DIK_DOWN))
            m_pTransformCom->Go_Backward(fRawTimeDelta);
        if (m_pGameInstance->Get_IsKeyPressing(DIK_LEFT))
            m_pTransformCom->Go_Left(fRawTimeDelta);
        if (m_pGameInstance->Get_IsKeyPressing(DIK_RIGHT))
            m_pTransformCom->Go_Right(fRawTimeDelta);
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


    _int    iMouseMove = {};


    if (iMouseMove = m_pGameInstance->Get_DIMouseMove(MOUSEMOVESTATE::X))
        m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f),
            fRawTimeDelta * iMouseMove * m_fMouseSensor);

    if (iMouseMove = m_pGameInstance->Get_DIMouseMove(MOUSEMOVESTATE::Y))
    {
        _float fDeltaPitch = fRawTimeDelta * iMouseMove * m_fMouseSensor;

        // 기존 Pitch + 이번 프레임 delta
        m_fPitch += fDeltaPitch;

        // 상하 한도 적용
        _float fAngleLimit = 80.f;
        if (m_fPitch > TO_RAD(fAngleLimit)) m_fPitch = TO_RAD(fAngleLimit);
        if (m_fPitch < -TO_RAD(fAngleLimit)) m_fPitch = -TO_RAD(fAngleLimit);

        // 제한된 Pitch 기준으로 회전
        m_pTransformCom->Set_Rotation_DirectEuler(
            _float3(TO_DEG(m_fPitch), m_pTransformCom->Get_RotationEuler_Store().y, 0.f)
        );

    }
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

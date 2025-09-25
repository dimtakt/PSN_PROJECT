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
    m_fOriginFovy = pDesc->fFovy;

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    m_pTransformCom->Set_Rotation_DirectEuler(
        _float3(TO_DEG(0.f), m_pTransformCom->Get_RotationEuler_Store().y, 0.f)
    );
    

    return S_OK;
}

void CCamera_Player::Priority_Update(_float fTimeDelta)
{
    _float fRawTimeDelta = fTimeDelta / (m_pGameInstance->Get_TimeSpeed());

    // * 키보드 WASD 에 의한 카메라 이동
    
    if (m_pGameInstance->Get_IsKeyDown(DIK_TAB))
        m_isFreeMode = !m_isFreeMode;
    
    // Free Move
    if (m_isFreeMode)
    {
        ShowCursor(TRUE);

        if (m_pGameInstance->Get_IsKeyPressing(DIK_UP))
            m_pTransformCom->Go_Straight(fRawTimeDelta);
        if (m_pGameInstance->Get_IsKeyPressing(DIK_DOWN))
            m_pTransformCom->Go_Backward(fRawTimeDelta);
        if (m_pGameInstance->Get_IsKeyPressing(DIK_LEFT))
            m_pTransformCom->Go_Left(fRawTimeDelta);
        if (m_pGameInstance->Get_IsKeyPressing(DIK_RIGHT))
            m_pTransformCom->Go_Right(fRawTimeDelta);
    }
    // Player Attached
    else
    {
        Cursor_Lock();
        ShowCursor(FALSE);

        _uint iDestLevel = m_pGameInstance->Get_DestLevel();
        m_pPlayerTransformCom = dynamic_cast<CTransform*>(m_pGameInstance->Find_Component(iDestLevel, L"Layer_Player", L"Com_Transform"));

        _float fPlayerHeight = 7.8f;

        _vector vPlayerPos = m_pPlayerTransformCom->Get_State(STATE::POSITION);
        _vector pCalcedPlayerPos = vPlayerPos + XMVectorSet(0.f, fPlayerHeight, 0.f, 0.f);

        m_pTransformCom->Set_State(STATE::POSITION, pCalcedPlayerPos);
    }


    _int    iMouseMove = {};


    //if (iMouseMove = m_pGameInstance->Get_DIMouseMove(MOUSEMOVESTATE::X))
    //{
    //    m_pPlayerTransformCom->Turn(
    //        XMVectorSet(0.f, 1.f, 0.f, 0.f),
    //        fRawTimeDelta * iMouseMove * m_fMouseSensor
    //    );
    //}
    iMouseMove = m_pGameInstance->Get_DIMouseMove(MOUSEMOVESTATE::X);
    iMouseMove = m_pGameInstance->Get_DIMouseMove(MOUSEMOVESTATE::Y);


    {
        _float fDeltaPitch = fRawTimeDelta * iMouseMove * m_fMouseSensor;

        // 기존 Pitch + 이번 프레임 delta
        m_fPitch += fDeltaPitch;

        // 상하 한도 적용
        _float fAngleLimit = 80.f;
        if (m_fPitch > TO_RAD(fAngleLimit)) m_fPitch = TO_RAD(fAngleLimit);
        if (m_fPitch < -TO_RAD(fAngleLimit)) m_fPitch = -TO_RAD(fAngleLimit);

        // 제한된 Pitch 기준으로 회전..
        // 플레이어 회전값 가져와서
        _float3 vPlayerEuler = m_pPlayerTransformCom->Get_RotationEuler_Store();

        // 카메라에 직접 적용 (Pitch, PlayerYaw, 0)
        m_pTransformCom->Set_Rotation_DirectEuler(
            _float3(TO_DEG(m_fPitch), vPlayerEuler.y, 0.f)
        );

    }







    // fov restore 
    if (m_fFovy != m_fOriginFovy)
    {
        const _float fRestorePower = TO_RAD(90.f);

        if      (m_fFovy > m_fOriginFovy)
        {
            m_fFovy = m_fFovy - fRestorePower * fRawTimeDelta;
            if (m_fFovy < m_fOriginFovy) m_fFovy = m_fOriginFovy;
        }
        else if (m_fFovy < m_fOriginFovy)
        {
            m_fFovy = m_fFovy + fRestorePower * fRawTimeDelta;
            if (m_fFovy > m_fOriginFovy) m_fFovy = m_fOriginFovy;
        }
    }




    // for Skake

    // 원본 값 저장
    _matrix matOrigin = m_pTransformCom->Get_WorldMatrix();

    // 원본 행렬 정보 가져온 뒤, 랜덤 값 구함
    _vector vLook = m_pTransformCom->Get_State(STATE::LOOK);
    _vector vRight = m_pTransformCom->Get_State(STATE::RIGHT);
    _vector vUp = m_pTransformCom->Get_State(STATE::UP);
    _float fRandX = m_pGameInstance->Rand(TO_RAD(-m_fShakeDegree), TO_RAD(m_fShakeDegree));
    _float fRandY = m_pGameInstance->Rand(TO_RAD(-m_fShakeDegree), TO_RAD(m_fShakeDegree));
    _float fRandZ = m_pGameInstance->Rand(TO_RAD(-m_fShakeDegree), TO_RAD(m_fShakeDegree));

    // 오일러 랜덤값 기반 회전행렬 생성, 각 벡터에 계산
    _matrix mRot = XMMatrixRotationRollPitchYaw(fRandX, fRandY, fRandZ);
    vLook = XMVector3TransformNormal(vLook, mRot);
    vUp = XMVector3TransformNormal(vUp, mRot);
    vRight = XMVector3TransformNormal(vRight, mRot);

    // 정규화 후 적용
    vLook = XMVector3Normalize(vLook);
    vRight = XMVector3Normalize(XMVector3Cross(vUp, vLook));
    vUp = XMVector3Cross(vLook, vRight);
    m_pTransformCom->Set_State(STATE::LOOK, vLook);
    m_pTransformCom->Set_State(STATE::RIGHT, vRight);
    m_pTransformCom->Set_State(STATE::UP, vUp);

    // 카메라 값 적용 타이밍
    __super::Update_PipeLines();

    // 복구
    m_pTransformCom->Set_WorldMatrix(matOrigin);

    if (m_fShakeDegree != 0)
    {
        m_fShakeDegree = m_fShakeDegree * 0.8f;

        if (IS_BETWEEN(m_fShakeDegree, -1.f, 1.f))
            m_fShakeDegree = 0.f;
    }

    if (m_pGameInstance->Get_IsKeyDown(DIK_M))
        m_fShakeDegree = 10.f;
}

void CCamera_Player::Update(_float fTimeDelta)
{
    // 여기서 시간 경과에 따른 정상화 진행,
    // 외부에서 함수를 통해 기본값을 망가뜨리고 그에 맞게 흔들림이나 줌인아웃이 진행되는 방식으로

}

void CCamera_Player::Late_Update(_float fTimeDelta)
{
}

HRESULT CCamera_Player::Render()
{
    return S_OK;
}

void CCamera_Player::Cursor_Lock()
{
    RECT rect;
    GetClientRect(g_hWnd, &rect);

    _int iX = (rect.right - rect.left) / 2;
    _int iY = (rect.bottom - rect.top) / 2;

    POINT pt{ iX, iY };
    ClientToScreen(g_hWnd, &pt);
    SetCursorPos(pt.x, pt.y);
}

void CCamera_Player::Camera_ShortZoom(_float fZoomStrength)
{
    if (m_fFovy != m_fOriginFovy)
        return;

    m_fFovy = m_fFovy + TO_RAD(fZoomStrength);
}

void CCamera_Player::Camera_Shake(_float fShakeStrength)
{
    m_fShakeDegree = fShakeStrength;
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

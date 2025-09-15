#include "Weapon_Gun.h"
#include "GameInstance.h"
#include "Bullet.h"

CWeapon_Gun::CWeapon_Gun(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CWeapon{ pDevice, pContext }
{

}

CWeapon_Gun::CWeapon_Gun(const CWeapon_Gun& Prototype)
    : CWeapon{ Prototype }
{

}

HRESULT CWeapon_Gun::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CWeapon_Gun::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    return S_OK;
}

void CWeapon_Gun::Priority_Update(_float fTimeDelta)
{
    __super::Priority_Update(fTimeDelta);
}

void CWeapon_Gun::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);
}

void CWeapon_Gun::Late_Update(_float fTimeDelta)
{
    __super::Late_Update(fTimeDelta);
}

HRESULT CWeapon_Gun::Render()
{
    if (FAILED(__super::Render()))
        return E_FAIL;

    return S_OK;
}

void CWeapon_Gun::Shot(_vector vDir, _uint iObjTypeIndex)
{
    m_fShotRandRange;
    m_fZeroDst;

    _vector vZeroDstPos = vDir * m_fZeroDst;
    _vector vStartPos = XMVectorZero();

    _float fRandRangeX = m_pGameInstance->Rand(- m_fShotRandRange / 2.0f, m_fShotRandRange / 2.0f);
    _float fRandRangeY = m_pGameInstance->Rand(- m_fShotRandRange / 2.0f, m_fShotRandRange / 2.0f);
    _float fRandRangeZ = m_pGameInstance->Rand(- m_fShotRandRange / 2.0f, m_fShotRandRange / 2.0f);
    _vector vRandRange = XMVectorSet(fRandRangeX, fRandRangeY, fRandRangeZ, 1.0f);

    _vector vCalcedZeroDstPos = vZeroDstPos + vRandRange;
    _vector vCalcedDir = XMVector3Normalize(vCalcedZeroDstPos);     // 최종 계산된, 랜덤이 가미된 방향

    
    
    
    // 총알 생성..
    _uint iDestLevel = m_pGameInstance->Get_DestLevel();
    CBullet::Bullet_DESC bulletDesc = {};
    bulletDesc.vMoveDir = vCalcedDir /*vDir*/;
    //bulletDesc.vMoveDir = vDir;
    bulletDesc.fSpeedPerSec = 0.f;
    bulletDesc.iGameObjType = iObjTypeIndex;


    // 주체가 플레이어면 카메라에서 나가도록
    // 적이라면 무기좌표에서 나가도록
    bulletDesc.matSpawnTransform;
    if      (iObjTypeIndex == ENUM_CLASS(GAMEOBJ_TYPE::PLAYERBULLET))
        bulletDesc.matSpawnTransform = m_pGameInstance->Get_Transform_Matrix_Inverse(D3DTS::VIEW);
    else if (iObjTypeIndex == ENUM_CLASS(GAMEOBJ_TYPE::ENEMYBULLET))
        bulletDesc.matSpawnTransform = XMLoadFloat4x4(&m_CombinedWorldMatrix);


    if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(iDestLevel, L"Layer_Bullet",
        ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Weapon_Bullet"), &bulletDesc)))
        MSG_BOX(L"총알 생성 실패");
}

void CWeapon_Gun::Throw(_vector vDir, _vector vRot, _uint iObjTypeIndex)
{

}

void CWeapon_Gun::Free()
{
    __super::Free();
}

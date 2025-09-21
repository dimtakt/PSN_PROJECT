#include "Weapon_Gun.h"
#include "GameInstance.h"
#include "Bullet.h"
#include "CustomObj_Pickupable.h"

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
    if (m_isOnCD)
    {
        m_fShotElapsed += fTimeDelta;
    }

    if (m_fShotElapsed >= m_fShotMaxCD)
    {
        m_isOnCD = false;
        m_fShotElapsed = 0.f;
    }


    //__super::Update(fTimeDelta);
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

void CWeapon_Gun::Shot(_vector* pDir, _uint iObjTypeIndex)
{
    if (m_isOnCD)
        return;

    Add_Bullet(pDir, iObjTypeIndex);

    if (iObjTypeIndex == ENUM_CLASS(GAMEOBJ_TYPE::PLAYERBULLET))
    {
        m_iCurBullets--;
        std::cout << "[CWeapon_Gun::Shot] Current Left Bullets : " << m_iCurBullets << std::endl;
    }

    Add_ShotEffect();
}

void CWeapon_Gun::Throw(_vector* pDir, _float fThrowPower, _vector vRot, _uint iObjTypeIndex)
{
    // 현재 위치로부터 던지는 오브젝트 생성되도록 진행,
    // 해당 오브젝트는 pickupable 클래스이며, 잔탄 정보를 들고 있을 것임

    _matrix matStartTransform = m_pTransformCom->Get_WorldMatrix();
    _uint iDestLevel = m_pGameInstance->Get_DestLevel();



    // Create Pickupables...

    _wstring strModelPrototypeTag = {};
    switch (iObjTypeIndex)
    {
    case ENUM_CLASS(GAMEOBJ_TYPE::WEAPON_RANGED_KARABIN):   
        strModelPrototypeTag = L"Prototype_Component_Model_Custom_Weapon_Karabin_Fixed";     break;
    case ENUM_CLASS(GAMEOBJ_TYPE::WEAPON_RANGED_PISTOL):    
        strModelPrototypeTag = L"Prototype_Component_Model_Custom_Weapon_Pistol_Fixed";      break;
    case ENUM_CLASS(GAMEOBJ_TYPE::WEAPON_RANGED_SHOTGUN):   
        strModelPrototypeTag = L"Prototype_Component_Model_Custom_Weapon_Shotgun_Fixed";     break;
    }

    CCustomObj_Pickupable::THROWN_PICKUPOBJ_DESC tDesc = {};
    tDesc.strModelComPrototypeTag = strModelPrototypeTag;
    tDesc.iGameObjType = iObjTypeIndex;

    tDesc.vThrowDir = *pDir * fThrowPower;
    tDesc.vThrowRot = vRot;
    tDesc.tGunInfoDesc.iCurLeftBullets = m_iCurBullets;
    
    if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(iDestLevel, L"Layer_Loaded_Object_Pickupable",
        ENUM_CLASS(LEVEL::STATIC), L"Prototype_GameObject_Pickupable", &tDesc)))
        MSG_BOX(L"[CWeapon_Gun::Throw] Pickupable Object Create Failed.");

    // 생성한 pickupable 오브젝트의 Transform
    CTransform* pObjTransformCom = static_cast<CTransform*>((m_pGameInstance->Get_LastGameObject(iDestLevel, L"Layer_Loaded_Object_Pickupable")->Get_Component(L"Com_Transform")));
    pObjTransformCom->Set_WorldMatrix(m_CombinedWorldMatrix);
   

    // 이걸로 멀하려고했지?
    // 원래 쓰던 총의 정보가 들어있ㅇ므

    // 아마 잔탄 정보는 이쪽에 있었던거같은데..


}

void CWeapon_Gun::Add_Bullet(_vector* pDir, _uint iObjTypeIndex)
{
    _vector vZeroDstPos = *pDir * m_fZeroDst;
    _vector vStartPos = XMVectorZero();

    _float fRandRangeX = m_pGameInstance->Rand(-m_fShotRandRange / 2.0f, m_fShotRandRange / 2.0f);
    _float fRandRangeY = m_pGameInstance->Rand(-m_fShotRandRange / 2.0f, m_fShotRandRange / 2.0f);
    _float fRandRangeZ = m_pGameInstance->Rand(-m_fShotRandRange / 2.0f, m_fShotRandRange / 2.0f);
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
    if (iObjTypeIndex == ENUM_CLASS(GAMEOBJ_TYPE::PLAYERBULLET))
        bulletDesc.matSpawnTransform = m_pGameInstance->Get_Transform_Matrix_Inverse(D3DTS::VIEW);
    else if (iObjTypeIndex == ENUM_CLASS(GAMEOBJ_TYPE::ENEMYBULLET))
        bulletDesc.matSpawnTransform = XMLoadFloat4x4(&m_CombinedWorldMatrix);

    if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(iDestLevel, L"Layer_Bullet",
        ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Weapon_Bullet"), &bulletDesc)))
        MSG_BOX(L"총알 생성 실패");
}

void CWeapon_Gun::Add_ShotEffect()
{
    _matrix matGunTransform = m_pTransformCom->Get_WorldMatrix();
    _uint iDestLevel = m_pGameInstance->Get_DestLevel();
    const _wstring strEffectTag = L"Layer_Particle_ShotEffect";


    if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(iDestLevel, strEffectTag,
        iDestLevel, TEXT("Prototype_GameObject_Particle_ShotEffect"))))
        MSG_BOX(L"이펙트 생성 실패");
    CGameObject* pEffectObj = m_pGameInstance->Get_LastGameObject(iDestLevel, strEffectTag);
    CTransform* pEffectTransform = dynamic_cast<CTransform*>(pEffectObj->Get_Component(L"Com_Transform"));

    // effect의 -z축 방향이 무기 원점 좌표방향을 보도록 최초 회전값을 설정해주어야 함.
    // 위치의 경우는 무기 콜라이더의 앞쪽 부분으로.

    // 총의 좌표를 반영한다.
    pEffectTransform->Set_WorldMatrix(m_CombinedWorldMatrix);

    _matrix matCalcedPos = XMMatrixIdentity();
    _float3 vAdjustPos = {};

    // 총구부분에 맞도록 보정
    switch (m_iGameObjType)
    { 
    case ENUM_CLASS(GAMEOBJ_TYPE::WEAPON_RANGED_KARABIN):   vAdjustPos = {0.f, 0.04f, 3.f * 0.5f / 2.f};    break;
    case ENUM_CLASS(GAMEOBJ_TYPE::WEAPON_RANGED_PISTOL):    vAdjustPos = {0.f, 0.04f, 4.f * 0.15f / 2.f};   break;
    case ENUM_CLASS(GAMEOBJ_TYPE::WEAPON_RANGED_SHOTGUN):   vAdjustPos = {0.f, 0.04f, 3.f * 0.7f / 2.f};    break;
    default:                                                                                                break;
    }

    matCalcedPos = XMMatrixTranslationFromVector(XMLoadFloat3(&vAdjustPos)) * XMLoadFloat4x4(&m_CombinedWorldMatrix);
    pEffectTransform->Set_WorldMatrix(matCalcedPos);

    return;
}

void CWeapon_Gun::Free()
{
    __super::Free();
}


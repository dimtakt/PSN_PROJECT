#include "EnemyTrackWeaponState.h"
#include "EnemyAI.h"
#include "Enemy.h"

HRESULT CEnemyTrackWeaponState::Initialize()
{
	return S_OK;
}

void CEnemyTrackWeaponState::Enter(CComponent* pOwner)
{
	CEnemy* pEnemy = Get_Enemy(pOwner);
	Apply_StateTag(pEnemy, L"TrackWeapon");
}

void CEnemyTrackWeaponState::Update(CComponent* pOwner, _float fTimeDelta)
{
	CEnemy* pEnemy = Get_Enemy(pOwner);

	if (pEnemy != nullptr)
	{
		pEnemy->Chase_NearestWeapon(fTimeDelta);
		pEnemy->Try_PickupNearestWeapon();
	}

	Check_Transition(pOwner);
}

void CEnemyTrackWeaponState::Exit(CComponent* pOwner)
{
	return;
}

_bool CEnemyTrackWeaponState::Check_Transition(CComponent* pOwner)
{
	CEnemyAI* pEnemyAI = Get_EnemyAI(pOwner);
	if (pEnemyAI == nullptr)
		return false;

	const CEnemyAI::ENEMY_AI_PARAM& tAIParam = pEnemyAI->Get_AIParam();
	if (tAIParam.isHaveWeapon || !tAIParam.isNearExistWeapon)
	{
		pEnemyAI->Change_State(L"Idle");
		return true;
	}

	return false;
}

CEnemyTrackWeaponState* CEnemyTrackWeaponState::Create()
{
	CEnemyTrackWeaponState* pInstance = new CEnemyTrackWeaponState();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Create : CEnemyTrackWeaponState"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

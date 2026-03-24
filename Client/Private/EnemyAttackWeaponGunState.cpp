#include "EnemyAttackWeaponGunState.h"
#include "EnemyAI.h"
#include "Enemy.h"

HRESULT CEnemyAttackWeaponGunState::Initialize()
{
	return S_OK;
}

void CEnemyAttackWeaponGunState::Enter(CComponent* pOwner)
{
	CEnemy* pEnemy = Get_Enemy(pOwner);
	Apply_StateTag(pEnemy, L"AttackWeaponGun");
}

void CEnemyAttackWeaponGunState::Update(CComponent* pOwner, _float fTimeDelta)
{
	CEnemy* pEnemy = Get_Enemy(pOwner);

	if (pEnemy != nullptr)
		pEnemy->Face_Player();

	Check_Transition(pOwner);
}

void CEnemyAttackWeaponGunState::Exit(CComponent* pOwner)
{
	return;
}

_bool CEnemyAttackWeaponGunState::Check_Transition(CComponent* pOwner)
{
	CEnemyAI* pEnemyAI = Get_EnemyAI(pOwner);
	if (pEnemyAI == nullptr)
		return false;

	const CEnemyAI::ENEMY_AI_PARAM& tAIParam = pEnemyAI->Get_AIParam();
	if (!tAIParam.isHaveGunWeapon || tAIParam.fDistToPlayer >= 60.f)
	{
		pEnemyAI->Change_State(L"TrackPlayer");
		return true;
	}

	return false;
}

CEnemyAttackWeaponGunState* CEnemyAttackWeaponGunState::Create()
{
	CEnemyAttackWeaponGunState* pInstance = new CEnemyAttackWeaponGunState();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Create : CEnemyAttackWeaponGunState"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

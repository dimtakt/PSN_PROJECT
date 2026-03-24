#include "EnemyAttackMeleeState.h"
#include "EnemyAI.h"
#include "Enemy.h"

HRESULT CEnemyAttackMeleeState::Initialize()
{
	return S_OK;
}

void CEnemyAttackMeleeState::Enter(CComponent* pOwner)
{
	CEnemy* pEnemy = Get_Enemy(pOwner);
	Apply_StateTag(pEnemy, L"AttackMelee");
}

void CEnemyAttackMeleeState::Update(CComponent* pOwner, _float fTimeDelta)
{
	CEnemy* pEnemy = Get_Enemy(pOwner);

	if (pEnemy != nullptr)
		pEnemy->Face_Player();

	Check_Transition(pOwner);
}

void CEnemyAttackMeleeState::Exit(CComponent* pOwner)
{
	return;
}

_bool CEnemyAttackMeleeState::Check_Transition(CComponent* pOwner)
{
	CEnemyAI* pEnemyAI = Get_EnemyAI(pOwner);
	if (pEnemyAI == nullptr)
		return false;

	const CEnemyAI::ENEMY_AI_PARAM& tAIParam = pEnemyAI->Get_AIParam();
	if (tAIParam.fDistToPlayer >= 6.f)
	{
		pEnemyAI->Change_State(L"TrackPlayer");
		return true;
	}

	return false;
}

CEnemyAttackMeleeState* CEnemyAttackMeleeState::Create()
{
	CEnemyAttackMeleeState* pInstance = new CEnemyAttackMeleeState();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Create : CEnemyAttackMeleeState"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

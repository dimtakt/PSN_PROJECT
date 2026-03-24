#include "EnemyIdleState.h"
#include "EnemyAI.h"
#include "Enemy.h"

HRESULT CEnemyIdleState::Initialize()
{
	return S_OK;
}

void CEnemyIdleState::Enter(CComponent* pOwner)
{
	CEnemy* pEnemy = Get_Enemy(pOwner);
	Apply_StateTag(pEnemy, L"Idle");
}

void CEnemyIdleState::Update(CComponent* pOwner, _float fTimeDelta)
{
	CEnemy* pEnemy = Get_Enemy(pOwner);

	if (pEnemy != nullptr)
		pEnemy->Face_Player();
	
	Check_Transition(pOwner);
}

void CEnemyIdleState::Exit(CComponent* pOwner)
{
	return;
}

_bool CEnemyIdleState::Check_Transition(CComponent* pOwner)
{
	CEnemyAI* pEnemyAI = Get_EnemyAI(pOwner);
	if (pEnemyAI == nullptr)
		return false;

	const CEnemyAI::ENEMY_AI_PARAM& tAIParam = pEnemyAI->Get_AIParam();
	if (tAIParam.fDistToPlayer < 80.f)
	{
		pEnemyAI->Change_State(L"TrackPlayer");
		return true;
	}

	return false;
}

CEnemyIdleState* CEnemyIdleState::Create()
{
	CEnemyIdleState* pInstance = new CEnemyIdleState();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Create : CEnemyIdleState"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

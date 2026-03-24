#include "EnemyDamagedState.h"
#include "EnemyAI.h"
#include "Enemy.h"

HRESULT CEnemyDamagedState::Initialize()
{
	return S_OK;
}

void CEnemyDamagedState::Enter(CComponent* pOwner)
{
	CEnemy* pEnemy = Get_Enemy(pOwner);
	Apply_StateTag(pEnemy, L"Damaged");
}

void CEnemyDamagedState::Update(CComponent* pOwner, _float fTimeDelta)
{
	CEnemy* pEnemy = Get_Enemy(pOwner);

	if (pEnemy && pEnemy->Is_Groggy())
		return;

	Check_Transition(pOwner);
}

void CEnemyDamagedState::Exit(CComponent* pOwner)
{
	return;
}

_bool CEnemyDamagedState::Check_Transition(CComponent* pOwner)
{
	CEnemyAI* pEnemyAI = Get_EnemyAI(pOwner);
	if (pEnemyAI == nullptr)
		return false;

	const CEnemyAI::ENEMY_AI_PARAM& tAIParam = pEnemyAI->Get_AIParam();
	if (!tAIParam.isGroggy)
	{
		pEnemyAI->Change_State(L"Idle");
		return true;
	}

	return false;
}

CEnemyDamagedState* CEnemyDamagedState::Create()
{
	CEnemyDamagedState* pInstance = new CEnemyDamagedState();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Create : CEnemyDamagedState"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

#include "EnemyAnyState.h"
#include "EnemyAI.h"

HRESULT CEnemyAnyState::Initialize()
{
	return S_OK;
}

void CEnemyAnyState::Enter(CComponent* pOwner)
{
	return;
}

void CEnemyAnyState::Update(CComponent* pOwner, _float fTimeDelta)
{
	return;
}

void CEnemyAnyState::Exit(CComponent* pOwner)
{
	return;
}

_bool CEnemyAnyState::Check_Transition(CComponent* pOwner)
{
	CEnemyAI* pEnemyAI = static_cast<CEnemyAI*>(pOwner);
	CEnemyAI::ENEMY_AI_PARAM* pEnemyAIParam = &pEnemyAI->Get_AIParam();
	CGameObject* pOwner = pEnemyAI->Get_Owner();

	



}

CEnemyAnyState* CEnemyAnyState::Create()
{

	CEnemyAnyState* pInstance = new CEnemyAnyState();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Create : CEnemyAnyState"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEnemyAnyState::Free()
{
	__super::Free();
}

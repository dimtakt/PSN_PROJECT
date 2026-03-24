#include "EnemyAnyState.h"
#include "EnemyAI.h"
#include "Enemy.h"

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
	if (pEnemyAI == nullptr)
		return false;

	const CEnemyAI::ENEMY_AI_PARAM& tAIParam = pEnemyAI->Get_AIParam();
	CEnemy* pEnemy = static_cast<CEnemy*>(pEnemyAI->Get_Owner());
	if (pEnemy == nullptr)
		return false;

	if (tAIParam.isGetDamaged_Upper)
	{
		pEnemy->Set_StateFlags(ENUM_CLASS(ENEMY_STATE::DMGD_U));
		pEnemyAI->Change_State(L"Damaged");
		return true;
	}

	if (tAIParam.isGetDamaged_Lower)
	{
		pEnemy->Set_StateFlags(ENUM_CLASS(ENEMY_STATE::DMGD_L));
		pEnemyAI->Change_State(L"Damaged");
		return true;
	}

	if (!tAIParam.isGroggy &&
		!tAIParam.isHaveWeapon &&
		tAIParam.isNearExistWeapon &&
		pEnemyAI->Get_CurrentStateTag() != L"TrackWeapon")
	{
		pEnemyAI->Change_State(L"TrackWeapon");
		return true;
	}

	return false;
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

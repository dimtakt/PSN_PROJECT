#include "EnemyTrackPlayerState.h"
#include "EnemyAI.h"
#include "Enemy.h"

HRESULT CEnemyTrackPlayerState::Initialize()
{
	return S_OK;
}

void CEnemyTrackPlayerState::Enter(CComponent* pOwner)
{
	CEnemy* pEnemy = Get_Enemy(pOwner);
	Apply_StateTag(pEnemy, L"TrackPlayer");
}

void CEnemyTrackPlayerState::Update(CComponent* pOwner, _float fTimeDelta)
{
	CEnemy* pEnemy = Get_Enemy(pOwner);

	if (pEnemy != nullptr)
		pEnemy->Chase_Player(fTimeDelta);

	Check_Transition(pOwner);
}

void CEnemyTrackPlayerState::Exit(CComponent* pOwner)
{
	return;
}

_bool CEnemyTrackPlayerState::Check_Transition(CComponent* pOwner)
{
	CEnemyAI* pEnemyAI = Get_EnemyAI(pOwner);
	if (pEnemyAI == nullptr)
		return false;

	const CEnemyAI::ENEMY_AI_PARAM& tAIParam = pEnemyAI->Get_AIParam();
	if (tAIParam.fDistToPlayer >= 80.f)
	{
		pEnemyAI->Change_State(L"Idle");
		return true;
	}

	if (!tAIParam.isHaveWeapon && tAIParam.fDistToPlayer < 6.f)
	{
		pEnemyAI->Change_State(L"AttackMelee");
		return true;
	}

	if (tAIParam.isHaveGunWeapon && tAIParam.fDistToPlayer < 60.f)
	{
		pEnemyAI->Change_State(L"AttackWeaponGun");
		return true;
	}

	return false;
}

CEnemyTrackPlayerState* CEnemyTrackPlayerState::Create()
{
	CEnemyTrackPlayerState* pInstance = new CEnemyTrackPlayerState();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Create : CEnemyTrackPlayerState"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

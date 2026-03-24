#include "EnemyState.h"
#include "EnemyAI.h"
#include "Enemy.h"

CEnemyAI* CEnemyState::Get_EnemyAI(CComponent* pOwner)
{
	return static_cast<CEnemyAI*>(pOwner);
}

CEnemy* CEnemyState::Get_Enemy(CComponent* pOwner)
{
	CEnemyAI* pEnemyAI = Get_EnemyAI(pOwner);
	if (pEnemyAI == nullptr)
		return nullptr;

	return static_cast<CEnemy*>(pEnemyAI->Get_Owner());
}

void CEnemyState::Apply_StateTag(CEnemy* pEnemy, const _wstring& strStateTag)
{
	if (pEnemy == nullptr)
		return;

	if (strStateTag == L"Idle")
	{
		pEnemy->Set_StateFlags(ENUM_CLASS(ENEMY_STATE::IDLE));
		return;
	}

	if (strStateTag == L"TrackPlayer")
	{
		pEnemy->Set_StateFlags(ENUM_CLASS(ENEMY_STATE::TRACK_PLAYER) | ENUM_CLASS(ENEMY_STATE::MOVE));
		return;
	}

	if (strStateTag == L"TrackWeapon")
	{
		pEnemy->Set_StateFlags(ENUM_CLASS(ENEMY_STATE::TRACK_WEAPON) | ENUM_CLASS(ENEMY_STATE::MOVE));
		return;
	}

	if (strStateTag == L"AttackMelee")
	{
		pEnemy->Set_StateFlags(ENUM_CLASS(ENEMY_STATE::ATK_MELEE));
		return;
	}

	if (strStateTag == L"AttackWeaponGun")
	{
		pEnemy->Set_StateFlags(ENUM_CLASS(ENEMY_STATE::ATK_WEAPON_GUN));
		return;
	}

	if (strStateTag == L"Damaged")
	{
		if ((pEnemy->Get_StateFlags() & ENUM_CLASS(ENEMY_STATE::DMGD_L)) == 0 &&
			(pEnemy->Get_StateFlags() & ENUM_CLASS(ENEMY_STATE::DMGD_U)) == 0)
		{
			pEnemy->Set_StateFlags(ENUM_CLASS(ENEMY_STATE::DMGD_L));
		}
	}
}

void CEnemyState::Free()
{
	__super::Free();
}

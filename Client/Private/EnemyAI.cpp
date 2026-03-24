#include "EnemyAI.h"

#include "Gameinstance.h"
#include "EnemyState.h"
#include "EnemyAnyState.h"
#include "Enemy.h"
#include "EnemyIdleState.h"
#include "EnemyTrackPlayerState.h"
#include "EnemyTrackWeaponState.h"
#include "EnemyAttackMeleeState.h"
#include "EnemyAttackWeaponGunState.h"
#include "EnemyDamagedState.h"

CEnemyAI::CEnemyAI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent { pDevice, pContext }
{
}

HRESULT CEnemyAI::Initialize(void* pArg)
{
	if (pArg == nullptr)
		return E_FAIL;

	ENEMY_AI_DESC* pEnemyAI = static_cast<ENEMY_AI_DESC*>(pArg);
	m_pOwner = pEnemyAI->pOwner;

	Insert_State(L"AnyState", CEnemyAnyState::Create());
	Insert_State(L"Idle", CEnemyIdleState::Create());
	Insert_State(L"TrackPlayer", CEnemyTrackPlayerState::Create());
	Insert_State(L"TrackWeapon", CEnemyTrackWeaponState::Create());
	Insert_State(L"AttackMelee", CEnemyAttackMeleeState::Create());
	Insert_State(L"AttackWeaponGun", CEnemyAttackWeaponGunState::Create());
	Insert_State(L"Damaged", CEnemyDamagedState::Create());

	CEnemyState* pEnemyAnyState = Find_State(L"AnyState");
	m_pAnyState = static_cast<CEnemyAnyState*>(pEnemyAnyState);

	// Entry
	Change_State(L"Idle");

	return S_OK;
}

void CEnemyAI::Update(_float fTimeDelta)
{
	_bool isStateInterruped = Check_AnyState();

	if (!isStateInterruped)
	{
		if (m_pCurrentState)
			m_pCurrentState->Update(this, fTimeDelta);
	}

	m_tAIParam.ResetTriggers();
}

void CEnemyAI::Change_State(CEnemyState* pNewState)
{
	if (m_pCurrentState)
		m_pCurrentState->Exit(this);

	m_pCurrentState = pNewState;

	if (m_pCurrentState)
		m_pCurrentState->Enter(this);
}

void CEnemyAI::Change_State(const _wstring& strStateTag)
{
	auto state = Find_State(strStateTag);

	if (state == nullptr)
	{
#ifdef _DEBUG
		std::cout << "[WRN][EnemyAI::Change_State] Cannot Find State." << std::endl;
#endif // _DEBUG
		return;
	}

	m_strCurrentStateTag = strStateTag;
	Change_State(state);
}

_bool CEnemyAI::Insert_State(const _wstring& strStateTag, CEnemyState* state)
{
	if (state == nullptr)
		return false;

	_bool isAlreadyExists = (Find_State(strStateTag) != nullptr);

	if (isAlreadyExists)
	{
#ifdef _DEBUG
		std::cout << "[WRN][EnemyAI::InsertState] State Already Exists." << std::endl;
#endif // _DEBUG
		Safe_Release(state);
		return false;
	}

	m_umapStates.insert({strStateTag, state});

	return true;
}

CEnemyState* CEnemyAI::Find_State(const _wstring& strStateTag)
{
	auto it = m_umapStates.find(strStateTag);

	if (it != m_umapStates.end())
		return it->second;

	return nullptr;
}

_bool CEnemyAI::Check_AnyState()
{
	if (m_pAnyState == nullptr)
		return false;

	return m_pAnyState->Check_Transition(this);
}

CEnemyAI* CEnemyAI::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CEnemyAI* pInstance = new CEnemyAI(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CEnemyAI"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CEnemyAI::Clone(void* pArg)
{
	CEnemyAI* pInstance = new CEnemyAI(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CEnemyAI"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEnemyAI::Free()
{
	for (auto& statepair : m_umapStates)
	{
		auto state = (&statepair)->second;
		Safe_Release(state);
	}

	__super::Free();
}

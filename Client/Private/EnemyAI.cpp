#include "EnemyAI.h"

#include "Gameinstance.h"
#include "EnemyState.h"
#include "EnemyAnyState.h"
#include "Enemy.h"

CEnemyAI::CEnemyAI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent { pDevice, pContext }
{
}

HRESULT CEnemyAI::Initialize(void* pArg)
{
	if (pArg == nullptr)
		return E_FAIL;

	ENEMY_AI* pEnemyAI = static_cast<ENEMY_AI*>(pArg);
	m_pOwner = pEnemyAI->pOwner;

	// ==============================
	// 여기에서 State들 new 한 뒤 삽입.
	// 이후 Safe_Delete 필수!!!








	// ==============================

	CEnemyState* pEnemyAnyState = Find_State(L"AnyState");
	m_pAnyState = static_cast<CEnemyAnyState*>(pEnemyAnyState);

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
	
	Change_State(state);
}

_bool CEnemyAI::Insert_State(const _wstring& strStateTag, CEnemyState* state)
{
	_bool isAlreadyExists = (Find_State(strStateTag) == nullptr);

	if (!isAlreadyExists)
		m_umapStates.insert({strStateTag, state});

#ifdef _DEBUG
	std::cout << "[WRN][EnemyAI::InsertState] State Already Exists." << std::endl;
#endif // _DEBUG

	return !isAlreadyExists;
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
	// 조건들 정의

	// - 1번 조건?
	// 상세조건 : 보유중인 무기 없음, 주변에 주인없는 무기 감지
	// 목표상태 : 무기추적
	// - 2번 조건? : 
	// 상세조건 : 상체 또는 하체 공격받음
	// 목표상태 : 피격중

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

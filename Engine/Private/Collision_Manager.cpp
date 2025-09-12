#include "Collision_Manager.h"
#include "GameInstance.h"
#include "GameObject.h"


CCollision_Manager::CCollision_Manager()
	: m_pGameInstance{ CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CCollision_Manager::Initialize(_uint iNumLevels)
{
	m_iNumLevels = iNumLevels;

	return S_OK;
}

void CCollision_Manager::Update()
{
	for (_uint i = 0; i < m_vecColliders.size(); i++)
		for (_uint j = 0; j < m_vecColliders.size(); j++)
			if (i != j)
			{
				if (Check_Collision(m_vecColliders[i], m_vecColliders[j]))
				{
					COLLISION_DESC descA = m_vecColliders[i]->Get_ColDesc();	// 공격
					COLLISION_DESC descB = m_vecColliders[j]->Get_ColDesc();	// 피격

					descB.pOwner->OnCollision(descA.pOwner);
				}
			}
}

HRESULT CCollision_Manager::Add_Collider(CCollider* pCollider)
{
	if (pCollider == nullptr)
		return S_OK;

	m_vecColliders.push_back(pCollider);

	return S_OK;
}

HRESULT CCollision_Manager::Remove_Collider(CCollider* pCollider)
{
	if (pCollider == nullptr)
		return S_OK;
		
	auto iter = std::find(m_vecColliders.begin(), m_vecColliders.end(), pCollider);
	if (iter != m_vecColliders.end())
	{
		m_vecColliders.erase(iter);
		return S_OK;
	}

	return E_FAIL;
}

_bool CCollision_Manager::Check_Collision(CCollider* pColAtk, CCollider* pColHit)
{
	if (!pColAtk || !pColHit)
		return false;

	// 이미 콜라이더 내에 정보가 다 담겨 있으므로,
	// 콜라이더끼리만 검사하면 될 것 같은데..
	
	COLLISION_DESC descA = pColAtk->Get_ColDesc();	// 공격
	COLLISION_DESC descB = pColHit->Get_ColDesc();	// 피격

	_bool isBothActive = descA.isActive && descB.isActive;

	if ((descA.iMask & descB.iLayerIndex) && isBothActive)
		return (pColAtk->Intersect(pColHit));

	return false;
}

CCollision_Manager* CCollision_Manager::Create(_uint iNumLevels)
{
	CCollision_Manager* pInstance = new CCollision_Manager();

	if (FAILED(pInstance->Initialize(iNumLevels)))
	{
		MSG_BOX(TEXT("Failed to Created : CObject_Manager"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCollision_Manager::Free()
{
	__super::Free();

	
	Safe_Release(m_pGameInstance);
}
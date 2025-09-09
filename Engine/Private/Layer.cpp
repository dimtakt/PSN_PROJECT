#include "Layer.h"

#include "GameObject.h"

CLayer::CLayer()
{

}

CComponent* CLayer::Get_Component(const _wstring& strComponentTag, _uint iIndex)
{
	auto	iter = m_GameObjects.begin();

	for (size_t i = 0; i < iIndex; i++)
		++iter;
	
	return (*iter)->Get_Component(strComponentTag);
}

CGameObject* CLayer::Get_GameObject(_uint iIndex)
{
	auto	iter = m_GameObjects.begin();

	for (size_t i = 0; i < iIndex; i++)
		++iter;

	if (iter == m_GameObjects.end())	// 임의로 방어코드 넣음
		return nullptr;

	return *iter;
}

void CLayer::Priority_Update(_float fTimeDelta)
{
	for (auto& pGameObject : m_GameObjects)
	{
		if (nullptr != pGameObject)
			pGameObject->Priority_Update(fTimeDelta);
	}
}

void CLayer::Update(_float fTimeDelta)
{

	for (auto& pGameObject : m_GameObjects)
	{
		if (nullptr != pGameObject)
			pGameObject->Update(fTimeDelta);
		
		if (pGameObject->Get_isDead())
			m_vecDeadObjects.push_back(pGameObject);
	}
}

void CLayer::Late_Update(_float fTimeDelta)
{
	for (auto& pGameObject : m_GameObjects)
	{
		if (nullptr != pGameObject)
			pGameObject->Late_Update(fTimeDelta);
	}
}


CGameObject* CLayer::Get_LastGameObject()
{
	if (!m_GameObjects.empty())
	{
		return m_GameObjects.back();
	}

	return nullptr;
}

void CLayer::Remove_DeadObjects()
{
	for (auto& obj : m_vecDeadObjects)
		Remove_GameObject(obj);
}

CLayer* CLayer::Create()
{
	return new CLayer();
}

void CLayer::Free()
{
	__super::Free();

	for (auto& pGameObject : m_GameObjects)
		Safe_Release(pGameObject);

	m_GameObjects.clear();
	
}

#pragma once

#include "Base.h"

NS_BEGIN(Engine)

class CLayer final : public CBase
{
private:
	CLayer();
	virtual ~CLayer() = default;

public:
	class CComponent* Get_Component(const _wstring& strComponentTag, _uint iIndex);
	class CGameObject* Get_GameObject(_uint iIndex);

public:
	HRESULT Add_GameObject(class CGameObject* pGameObject) {
		m_GameObjects.push_back(pGameObject);
		return S_OK;
	}
	HRESULT Remove_GameObject(CGameObject* pObject)
	{
		if (nullptr == pObject)
			return E_FAIL;
		for (auto iter = m_GameObjects.begin(); iter != m_GameObjects.end(); ++iter){
			if (*iter == pObject){
				Safe_Release(*iter);
				m_GameObjects.erase(iter);
				return S_OK;
			}
		}
		return E_FAIL;
	}

	void Priority_Update(_float fTimeDelta);
	void Update(_float fTimeDelta);
	void Late_Update(_float fTimeDelta);

public:
	CGameObject* Get_LastGameObject();


	void Remove_DeadObjects();

private:
	list<class CGameObject*>			m_GameObjects;
	vector<class CGameObject*>			m_vecDeadObjects;


public:
	static CLayer* Create();
	virtual void Free() override;
};

NS_END
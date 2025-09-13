#pragma once
#include "Base.h"

NS_BEGIN(Engine)

class CCollider;

class CCollision_Manager final : public CBase
{
private:
	CCollision_Manager();
	virtual ~CCollision_Manager() = default;

public:
	HRESULT Initialize(_uint iNumLevels);
	void Update();
	void Clear();
	
public:
	HRESULT Add_Collider(CCollider* pCollider);
	HRESULT Remove_Collider(CCollider* pCollider);

	_bool Check_RayCollisions(RAYCOLLISION_DESC* pRayDesc, CGameObject*& OutIntersectObj, _float& fOutDistance);

private:
	_bool Check_Collision(CCollider* pColAtk, CCollider* pColHit);
	_bool Check_RayCollision(RAYCOLLISION_DESC* pRayDesc, CCollider* pColTarget, _float& fOutDistance);

public:
	static CCollision_Manager* Create(_uint iNumLevels);
	virtual void Free() override;


private:
	vector<CCollider*>		m_vecColliders = { };

	class CGameInstance*	m_pGameInstance = { nullptr };
	_uint					m_iNumLevels = {};
};

NS_END

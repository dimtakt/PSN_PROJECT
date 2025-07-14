#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

NS_BEGIN(Client)

class CEnemy final : public CGameObject
{
public:
	typedef struct tagEnemyDesc : public GAMEOBJECT_DESC
	{

	}ENEMY_DESC;
private:
	CEnemy(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEnemy(const CGameObject& Prototype);
	virtual ~CEnemy() = default;

public:
	HRESULT		Initialize_Prototype()				override;
	HRESULT		Initialize(void* pArg)				override;
	void		Priority_Update(_float fTimeDelta)	override;
	void		Update(_float fTimeDelta)			override;
	void		Late_Update(_float fTimeDelta)		override;
	HRESULT		Render()							override;

public:
	// 퍼블릭 함수들 (Get/Set 등..)
	// ..

private:
	// 로컬 함수들 (기능 분리)
	HRESULT		Ready_Components(void* pArg);

private:
	// 로컬 변수들 (타입, 컴포넌트 등..)
	GAMEOBJ_TYPE	m_eGameObjType	=	GAMEOBJ_TYPE::ENEMY;



public:
	static CEnemy*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEnemy*			Clone(void* pArg)					override;
	void			Free()								override;


};

NS_END
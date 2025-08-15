#pragma once

#include "Client_Defines.h"
#include "ContainerObject.h"

NS_BEGIN(Engine)

class CNavigation;

NS_END


NS_BEGIN(Client)

class CPlayer final : public CContainerObject
{
public:
	enum STATE { 
		IDLE	= (1 << 0),
		RUN		= (1 << 1),
		ATTACK	= (1 << 2)
	};

public:
	typedef struct tagPlayerDesc : public GAMEOBJECT_DESC
	{

	}PLAYER_DESC;
private:
	CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayer(const CPlayer& Prototype);
	virtual ~CPlayer() = default;

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
	HRESULT		Ready_PartObjects();

private:
	// 로컬 변수들 (타입, 컴포넌트 등..)
	CNavigation*	m_pNavigationCom = { nullptr };


	_uint			m_iGameObjType = { };
	_int			m_iHp = {};

	_uint			m_iState = { };

public:
	static CPlayer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject*	Clone(void* pArg)				override;
	void			Free()							override;


};

NS_END
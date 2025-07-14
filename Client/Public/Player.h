#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

NS_BEGIN(Client)

class CPlayer final : public CGameObject
{
public:
	typedef struct tagPlayerDesc : public GAMEOBJECT_DESC
	{

	}PLAYER_DESC;
private:
	CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayer(const CGameObject& Prototype);
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

private:
	// 로컬 변수들 (타입, 컴포넌트 등..)
	GAMEOBJ_TYPE	m_eGameObjType = GAMEOBJ_TYPE::PLAYER;
	_int			m_iHp = {};


public:
	static CPlayer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayer*		Clone(void* pArg)					override;
	void			Free()								override;


};

NS_END
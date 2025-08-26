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
	HRESULT		Bind_ShaderResources();
	HRESULT		Ready_PartObjects();

	void		Update_AnimationState();	// 조작 등에 의한 "상태" 제어
	void		Update_AnimationIndex();	// 상태에 의한 "애니메이션" 제어

private:
	// 로컬 변수들 (타입, 컴포넌트 등..)
	CNavigation*	m_pNavigationCom = { nullptr };

	CShader*		m_pShaderCom = { nullptr };
	CModel*			m_pModelCom = { nullptr };


private:
	_uint			m_iGameObjType = { };
	_int			m_iHp = {};

	_uint			m_iState = { };

	_float			m_fMouseSensor = { .1f };



public:
	static CPlayer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject*	Clone(void* pArg)				override;
	void			Free()							override;


};

NS_END
#pragma once

#include "Client_Defines.h"
#include "ContainerObject.h"

NS_BEGIN(Engine)

class CNavigation;
class CCollider;

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
	
	HRESULT		Render_Shadow()						override;

public:
	// 퍼블릭 함수들 (Get/Set 등..)
	// ..
	CPartObject* Get_WeaponPart() { return m_pPart_Weapon; };

private:
	// 로컬 함수들 (기능 분리)
	HRESULT		Ready_Components(void* pArg);
	HRESULT		Bind_ShaderResources();
	HRESULT		Ready_PartObjects(_uint iObjType);

	void		Update_Transform(_float fTimeDelta);		// 조작 등에 의한 "Transform" 제어
	void		Update_AnimationState(_float fTimeDelta);	// 조작 등에 의한 "상태" 제어 (이를 기반으로 이벤트 등..)
	void		Update_AnimationIndex(_float fTimeDelta);	// 조작 등에 의한 "애니메이션" 제어
	void		Update_TimeControl(_float fTimeDelta);
	void		Update_Interact(_float fTimeDelta);

	void		Update_BoneColliders();

private:
	void		Update_BoneCollider(CCollider* pCollider, const _char* szBoneName);

	HRESULT		Ready_Colliders(void* pArg);


private:
	void		Update_ToggleColliders();

private:
	// 로컬 변수들 (타입, 컴포넌트 등..)
	CShader*						m_pShaderCom		= { nullptr };
	CModel*							m_pModelCom			= { nullptr };
	CNavigation*					m_pNavigationCom	= { nullptr };

	CPartObject*					m_pPart_Weapon		= { nullptr };

private:;
	_uint			m_iState = { };

	_float			m_fMouseSensor = { .1f };

public:
	static CPlayer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject*	Clone(void* pArg)				override;
	void			Free()							override;


};

NS_END
#pragma once

#include "Client_Defines.h"
#include "CustomObj_NonAnim.h"
#include "Weapon_Gun.h"


NS_BEGIN(Engine)

class CShader;
class CModel;

NS_END

NS_BEGIN(Client)

class CCustomObj_Pickupable final : public CCustomObj_NonAnim
{
public:
	typedef struct thrownPickupObjDesc : public CUSTOMOBJ_DESC
	{
		_vector vThrowDir = {};
		_vector vThrowRot = {};

		CWeapon_Gun::GUNINFO_DESC tGunInfoDesc = {};
	} THROWN_PICKUPOBJ_DESC;

private:
	CCustomObj_Pickupable(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCustomObj_Pickupable(const CGameObject& Prototype);
	virtual ~CCustomObj_Pickupable() = default;

public:
	HRESULT		Initialize_Prototype()				override;
	HRESULT		Initialize(void* pArg)				override;
	void		Priority_Update(_float fTimeDelta)	override;
	void		Update(_float fTimeDelta)			override;
	void		Late_Update(_float fTimeDelta)		override;
	HRESULT		Render()							override;

	void		OnCollisionRay(CGameObject* pCollisionHitBy)					override;
	
public:
	// 퍼블릭 함수들 (Get/Set 등..)
	CWeapon_Gun::GUNINFO_DESC Get_GunInfoDesc() { return m_tGunInfoDesc; }
	_bool		Get_IsPickingUp() { return m_isPickingUp; }
	
private:
	// 로컬 함수들 (기능 분리)
	HRESULT		Ready_Components(void* pArg);
	HRESULT		Bind_ShaderResources();

	void		Update_PickingUp(_float fTimeDelta);

private:
	// 로컬 변수들 (타입, 컴포넌트 등..)
	_bool			m_isPickingUp = false;
	_float			m_fPickingElapsedTime = 0.f;
	const _float	m_fPickingMaxTime = 0.3f;
	CGameObject*	m_pCollByTarget = nullptr;

	CWeapon_Gun::GUNINFO_DESC m_tGunInfoDesc = {};

	_vector			m_pThrowDir = {};	// 던져진 경우 Transform의 변화
	_vector			m_pThrowRot = {};	// 던져진 경우 Transform의 변화

public:
	static CCustomObj_Pickupable*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCustomObj_Pickupable*			Clone(void* pArg)		override;
	void							Free()					override;


};

NS_END

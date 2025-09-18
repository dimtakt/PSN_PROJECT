#pragma once
#include "Weapon_Gun.h"

class CWeapon_Pistol final : public CWeapon_Gun
{
private:
	CWeapon_Pistol(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CWeapon_Pistol(const CWeapon_Pistol& Prototype);
	virtual ~CWeapon_Pistol() = default;

public:
	virtual HRESULT Initialize_Prototype()				override;
	virtual HRESULT Initialize(void* pArg)				override;
	virtual void Priority_Update(_float fTimeDelta)		override;
	virtual void Update(_float fTimeDelta)				override;
	virtual void Late_Update(_float fTimeDelta)			override;
	virtual HRESULT Render()							override;

public:
	virtual void Shot(_vector* pDir, _uint iObjTypeIndex) override;
	virtual void Throw(_vector* pDir, _float fThrowPower, _vector vRot, _uint iObjTypeIndex) override;

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();


public:
	static CWeapon_Pistol* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};


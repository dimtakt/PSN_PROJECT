#pragma once
#include "Weapon.h"



class CWeapon_Gun abstract : public CWeapon
{
	CWeapon_Gun(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CWeapon_Gun(const CWeapon_Gun& Prototype);
	virtual ~CWeapon_Gun() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};


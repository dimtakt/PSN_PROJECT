//#pragma once
//#include "Weapon_Gun.h"
//
//class CWeapon_Shotgun abstract : public CWeapon_Gun
//{
//	CWeapon_Shotgun(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
//	CWeapon_Shotgun(const CWeapon_Gun& Prototype);
//	virtual ~CWeapon_Shotgun() = default;
//
//public:
//	virtual HRESULT Initialize_Prototype();
//	virtual HRESULT Initialize(void* pArg);
//	virtual void Priority_Update(_float fTimeDelta);
//	virtual void Update(_float fTimeDelta);
//	virtual void Late_Update(_float fTimeDelta);
//	virtual HRESULT Render();
//
//public:
//	static CWeapon_Shotgun* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
//	virtual CGameObject* Clone(void* pArg) override;
//	virtual void Free() override;
//};

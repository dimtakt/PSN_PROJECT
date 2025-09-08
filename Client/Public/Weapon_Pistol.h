//#pragma once
//#include "Weapon_Gun.h"
//
//class CWeapon_Pistol abstract : public CWeapon_Gun
//{
//	CWeapon_Pistol(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
//	CWeapon_Pistol(const CWeapon_Gun& Prototype);
//	virtual ~CWeapon_Pistol() = default;
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
//	static CWeapon_Pistol* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
//	virtual CGameObject* Clone(void* pArg) override;
//	virtual void Free() override;
//};

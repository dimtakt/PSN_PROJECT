#pragma once
#include "Weapon_Gun.h"

class CWeapon_Karabin final : public CWeapon_Gun
{
private:
	CWeapon_Karabin(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CWeapon_Karabin(const CWeapon_Karabin& Prototype);
	virtual ~CWeapon_Karabin() = default;

public:
	virtual HRESULT Initialize_Prototype()				override;
	virtual HRESULT Initialize(void* pArg)				override;
	virtual void Priority_Update(_float fTimeDelta)		override;
	virtual void Update(_float fTimeDelta)				override;
	virtual void Late_Update(_float fTimeDelta)			override;
	virtual HRESULT Render()							override;

public:
	virtual void Shot(_vector vDir, _uint iObjTypeIndex) override;
	virtual void Throw(_vector vDir, _vector vRot, _uint iObjTypeIndex) override;

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();


public:
	static CWeapon_Karabin* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};


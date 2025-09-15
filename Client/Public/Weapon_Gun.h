#pragma once
#include "Weapon.h"

class CWeapon_Gun abstract : public CWeapon
{
protected:
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
	virtual void Shot(_vector vDir, _uint iObjTypeIndex);		// 필요에 따라 하위ㅣ오브젝트에서 반복문으로 불러오도록.
	virtual void Throw(_vector vDir, _vector vRot, _uint iObjTypeIndex);	// 우클릭으로 드랍하는 것 구현

protected:
	_uint	m_iMaxBullets = {};		// 획득시 채워진 기본 탄 갯수
	_uint	m_iCurBullets = {};		// 잔탄 갯수

	_float	m_fShotRandRange = {};	// 영점거리 기준 탄퍼짐 범위 (지름)
	_float	m_fZeroDst = {};		// 영점 기준 거리
	_float	m_fShotCD = {};			// 격발 CoolDown

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};


#pragma once
#include "Weapon.h"

class CWeapon_Gun abstract : public CWeapon
{
public:
	typedef struct tWeaponGunInfoDesc {
		_uint iCurLeftBullets = UINT_MAX;
	}GUNINFO_DESC;

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
	virtual void Shot(_vector* pDir, _uint iObjTypeIndex);							// 어떤 총알을 발사할 것인가?
	virtual void Throw(_vector* pDir, _vector vRot, _uint iObjTypeIndex) override;	// 우클릭으로 드랍하는 것 구현.
									// 이후 파괴 및 픽업오브젝트로 잔탄, transform, deltaTransform정보 넘겨주어 생성
	virtual _float Get_CDRatio() { return m_fShotElapsed / m_fShotMaxCD; }

protected:
	_uint			m_iMaxBullets = {};		// 획득시 채워진 기본 탄 갯수
	_uint			m_iCurBullets = {};		// 잔탄 갯수

	_float			m_fShotRandRange = {};	// 영점거리 기준 탄퍼짐 범위 (지름)
	_float			m_fZeroDst = {};		// 영점 기준 거리
	
	_float			m_fShotElapsed = {};	// 격발 CoolDown
	const _float	m_fShotMaxCD = 0.6f;

	_bool			m_isOnCD = false;

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};


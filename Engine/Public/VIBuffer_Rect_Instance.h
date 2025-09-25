#pragma once

#include "VIBuffer_Instance.h"

NS_BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Rect_Instance final : public CVIBuffer_Instance
{
public:
	/* 파티클 인스턴싱을 위해서만 필요한 데이터.  */
	typedef struct tagRectInstanceDesc : public CVIBuffer_Instance::INSTANCE_DESC
	{
		_float3			vPivot;
		_float2			vSpeed;
		_float2			vLifeTime;
		_bool			isLoop;

		_bool			isTurn = false;
		_float2			vTurnSpeed = {};
	}RECT_INSTANCE_DESC;
private:
	CVIBuffer_Rect_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Rect_Instance(const CVIBuffer_Rect_Instance& Prototype);
	virtual ~CVIBuffer_Rect_Instance() = default;

public:
	virtual HRESULT Initialize_Prototype(const INSTANCE_DESC* pDesc) override;
	virtual HRESULT Initialize(void* pArg) override;

public:
	void Spread(_float fTimeDelta);
	void Spread_Turn(_float fTimeDelta);
	void Trail(_float fTimeDelta, class CGameObject* pOwner);

	void Drop(_float fTimeDelta);

public:
	_float Get_isLifeOver() { 
		if ((!m_isLoop) && (m_fElapsedTime >= m_fMaxLifetime))	return true;
		else													return false;
	}

private:
	_float3					m_vCenter = {};
	_float3					m_vRange = {};

	_float3					m_vPivot = {};
	_float*					m_pSpeeds = {};			// 얘는 Update중에도 써야 하니까 이렇게 저장
	_bool					m_isLoop = {};

	_bool					m_isTurn = false;
	_vector*				m_pAxises = {};
	_float*					m_pTurnSpeeds = {};
	_float*					m_pScales = {};

	_float					m_fElapsedTime = {};
	_float					m_fMaxLifetime = {};


	vector<_vector>			m_vTrailPoints = {};
	_uint					m_iTrailIndex = 0;

public:
	static CVIBuffer_Rect_Instance* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const INSTANCE_DESC* pDesc);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END
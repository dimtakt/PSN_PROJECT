#pragma once

#include "Component.h"

NS_BEGIN(Engine)

class CNavigation;

class ENGINE_DLL CTransform final : public CComponent
{
public:
	typedef struct tagTransformDesc
	{
		_float		fSpeedPerSec;
		_float		fRotationPerSec;
	}TRANSFORM_DESC;

private:
	CTransform(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTransform(const CTransform& Prototype) = delete;
	virtual ~CTransform() = default;

public:	
	_vector Get_State(STATE eState) const {
		return XMLoadFloat4(reinterpret_cast<const _float4*>(&m_WorldMatrix.m[ENUM_CLASS(eState)]));
	}	

	_float3 Get_Scaled() const {
		return _float3(
			XMVectorGetX(XMVector3Length(Get_State(STATE::RIGHT))), 
			XMVectorGetX(XMVector3Length(Get_State(STATE::UP))),
			XMVectorGetX(XMVector3Length(Get_State(STATE::LOOK)))
		);
	}

	_matrix Get_WorldMatrix() {
		return XMLoadFloat4x4(&m_WorldMatrix);
	}

	const _float4x4* Get_WorldMatrixPtr() {
		return &m_WorldMatrix;
	}

	_matrix Get_WorldMatrix_Inverse() {
		return XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_WorldMatrix));
	}

	void Set_WorldMatrix(_matrix matrix) {
		XMStoreFloat4x4(&m_WorldMatrix, matrix);
	}
	void Set_WorldMatrix(_float4x4 matrix) {
		m_WorldMatrix = matrix;
	}

	void Set_State(STATE eState, _fvector vState) {
		XMStoreFloat4(reinterpret_cast<_float4*>(&m_WorldMatrix.m[ENUM_CLASS(eState)]), vState);
	}

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);

public:
	HRESULT Bind_Shader_Resource(class CShader* pShader, const _char* pConstantName);

public:
	void Scale(_float3 vScale);						// 스케일 (절대적, 해당 수치 변경)
	void Scaling(_float3 vScale);					// 상대적 (상대적, 현재 대비 변경)
	void Go_Straight(_float fTimeDelta, CNavigation* pNavigation = nullptr);			// 앞으로
	void Go_Left(_float fTimeDelta, CNavigation* pNavigation = nullptr);				// 왼쪽으로
	void Go_Right(_float fTimeDelta, CNavigation* pNavigation = nullptr);				// 오른쪽으로
	void Go_Backward(_float fTimeDelta, CNavigation* pNavigation = nullptr);			// 뒤로
	void Go_Above(_float fTimeDelta);				// 위로
	void Go_Below(_float fTimeDelta);				// 아래로
	void Rotation(_fvector vAxis, _float fRadian);	// 회전 (절대적, 해당 수치 변경)
	void Turn(_fvector vAxis, _float fTimeDelta);	// 회전 (상대적, 현재 대비 변경)
	void LookAt(_fvector vAt);						// 해당 방향을 바라보도록 회전
	void Chase(_fvector vTargetPos, _float fTimeDelta, _float fLimit = 0.f);

private:
	_float4x4				m_WorldMatrix = {};
	_float					m_fSpeedPerSec = {};
	_float					m_fRotationPerSec = {};
	_float3					m_vAngles = {};
public:
	static CTransform* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg) { return nullptr; };
	virtual void Free() override;
};

NS_END
#pragma once

#include "Base.h"

NS_BEGIN(Engine)

// ==============================
// 
// DX9 까지는 디바이스를 통하여 뷰, 투영 행렬을 가져올 수 있었으나,
// DX11 부터는 해당 기능이 사라지고, 사용자가 직접 관리 및 셰이더에 전달해주어야 함.
// 
// 이러한 필요성에 의해 해당 클래스가 만들어짐.
// 뷰, 투영 행렬을 해당 클래스에 저장하고 필요에 따라 불러와 사용 가능하도록 하는 용도.
// 
// 사용례 : 카메라
// 
// ==============================

class CPipeLine final : public CBase
{
private:
	CPipeLine();
	virtual ~CPipeLine() = default;

public:
	_matrix Get_Transform_Matrix(D3DTS eTransformState) const;
	const _float4x4* Get_Transform_Float4x4(D3DTS eTransformState) const;
	_matrix Get_Transform_Matrix_Inverse(D3DTS eTransformState) const;
	const _float4x4* Get_Transform_Float4x4_Inverse(D3DTS eTransformState) const;
	const _float4* Get_CamPosition() const;

	void Set_Transform(D3DTS eTransformState, _fmatrix Matrix);
	void Set_Transform(D3DTS eTransformState, const _float4x4& Matrix);

public:
	HRESULT Initialize();		// 행렬들을 항등행렬로 초기화.
	void Update();				// 저장된 행렬을 통해 역행렬 계산, CamPosition 갱신.

private:
	// VIEW, PROJ, END 순. 해당 행렬 정보를 담음.
	_float4x4			m_TransformMatrices[ENUM_CLASS(D3DTS::END)] = {};
	_float4x4			m_TransformMatrixInverse[ENUM_CLASS(D3DTS::END)] = {};
	_float4				m_vCamPosition = {};

public:
	static CPipeLine* Create();
	virtual void Free() override;
};

NS_END
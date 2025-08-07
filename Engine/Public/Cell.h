#pragma once

#include "Base.h"

NS_BEGIN(Engine)

class CCell final : public CBase
{
private:
	CCell(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CCell() = default;

public:
	_vector Get_Point(CELLPOINT ePoint) {
		return XMLoadFloat3(&m_vPoints[ENUM_CLASS(ePoint)]);
	}

	void Set_Neighbor(CELLLINE eLine, CCell* pNeighborCell) {
		m_iNeighborIndices[ENUM_CLASS(eLine)] = pNeighborCell->m_iIndex;
	}

public:
	HRESULT Initialize(const _float3* pPoints, _int iIndex);
	_bool isIn(_fvector vPosition, _int* pNeighborIndex);

	_bool Compare_Points(_fvector vSourPoint, _fvector vDestPoint);
	_float Compute_Height(_fvector vLocalPos);

#ifdef _DEBUG
public:
	HRESULT Render();
#endif

private:
	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };

	_float3					m_vPoints[ENUM_CLASS(CELLPOINT::END)];
	_float3					m_vNormals[ENUM_CLASS(CELLLINE::END)];
	_int					m_iIndex = {};
	_int					m_iNeighborIndices[ENUM_CLASS(CELLLINE::END)] = { -1, -1, -1 };

#ifdef _DEBUG
private:
	class CVIBuffer_Cell*	m_pVIBuffer = { nullptr };

#endif

public:
	static CCell* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _float3* pPoints, _int iIndex);
	virtual void Free() override;
};

NS_END
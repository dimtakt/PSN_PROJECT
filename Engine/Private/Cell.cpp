#include "Cell.h"

#include "VIBuffer_Cell.h"

CCell::CCell(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice{ pDevice }
	, m_pContext{ pContext }
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CCell::Initialize(const _float3* pPoints, _int iIndex)
{
	m_iIndex = iIndex;

	memcpy(m_vPoints, pPoints, sizeof(_float3) * ENUM_CLASS(CELLPOINT::END));

	_vector		vLine = {};

	vLine = XMLoadFloat3(&m_vPoints[ENUM_CLASS(CELLPOINT::B)]) - XMLoadFloat3(&m_vPoints[ENUM_CLASS(CELLPOINT::A)]);
	m_vNormals[ENUM_CLASS(CELLLINE::AB)] = _float3(XMVectorGetZ(vLine) * -1.f, 0.f, XMVectorGetX(vLine));

	vLine = XMLoadFloat3(&m_vPoints[ENUM_CLASS(CELLPOINT::C)]) - XMLoadFloat3(&m_vPoints[ENUM_CLASS(CELLPOINT::B)]);
	m_vNormals[ENUM_CLASS(CELLLINE::BC)] = _float3(XMVectorGetZ(vLine) * -1.f, 0.f, XMVectorGetX(vLine));

	vLine = XMLoadFloat3(&m_vPoints[ENUM_CLASS(CELLPOINT::A)]) - XMLoadFloat3(&m_vPoints[ENUM_CLASS(CELLPOINT::C)]);
	m_vNormals[ENUM_CLASS(CELLLINE::CA)] = _float3(XMVectorGetZ(vLine) * -1.f, 0.f, XMVectorGetX(vLine));


#ifdef _DEBUG
	m_pVIBuffer = CVIBuffer_Cell::Create(m_pDevice, m_pContext, pPoints);
	if (nullptr == m_pVIBuffer)
		return E_FAIL;
#endif

	return S_OK;
}

_bool CCell::isIn(_fvector vPosition, _int* pNeighborIndex)
{
	// 작동 순서
	// 0. 플레이어 위치와 검사할 방향의 Cell 인덱스를 받아옴
	// 1. [플레이어 위치 -> Cell의 버텍스(1, 2, 3)] 방향을 계산함
	// 2. [변의 법선(1->2, 2->3, 3->1)] 방향을 가져옴
	// 3. 플레이어 위치가 변의 법선방향의 반대(즉 밖에 있음) 인지 확인함.
	// 4. 밖에 있다면 false 반환 + 해당 방향의 Cell을 인덱스 인자로 반환 (다음 검사를 위함)
	// ㄴ	한 변의 각도가 90도 이상인 경우 또는 한번에 여러 개의 셀을 넘은 등
	//		해당 방향의 셀이 무조건이 아닐 수 있음. 해당 경우를 방지
	// 5. 모든 변에서 안쪽이면 true 반환 (즉, Cell 내부에 위치)


	for (_uint i = 0; i < ENUM_CLASS(CELLLINE::END); ++i)
	{
		// 1번
		_vector	vDir = XMVector3Normalize(vPosition - XMVectorSetW(XMLoadFloat3(&m_vPoints[i]), 1.f));
		// 2번
		_vector vNormal = XMVector3Normalize(XMLoadFloat3(&m_vNormals[i]));

		// 3번
		if (0 < XMVectorGetX(XMVector3Dot(vDir, vNormal)))	// 두 벡터가 90도 초과 차이. (서로 반대 방향을 의미) acos -> rad2deg 하면 각도가 됨
		{													// 이로써 플레이어가 위치한 방향의 cell을 이웃 셀로 반환하면서 가까운 곳으로 탐색하게끔 작동.
			// 4번
			*pNeighborIndex = m_iNeighborIndices[i];

			return false;
		}
	}
	// 5번
	return true;
}

_bool CCell::isNear_onSlide(_fvector vPosition, _int* pNeighborIndex)
{
	// 1. 현재 이 Cell 이 오차범위 이내로 가까운 점을 포함하는지 검증
	// 2-1 (포함O) true 반환
	// 2-2 (포함X) isIn 호출로 플레이어와 가까운 Cell로 이동.. 을 의도했으나 이를 통해 반환되는 인덱스는 이웃 Cell이 없는 선분이므로 대부분 -1이 반환됨


	const _float EPS = 0.3f; // 오차 허용 범위

	// 겹치는 점이 있다면, 해당 점을 포함한 이웃 셀을 현재 cell로.
	for (_uint i = 0; i < ENUM_CLASS(CELLPOINT::END); ++i)
	{
		_float fDiff_fromPoint = FLT_MAX;

		fDiff_fromPoint = XMVectorGetX(XMVector3Length(XMLoadFloat3(&m_vPoints[i]) - vPosition));

		if (fDiff_fromPoint <= EPS)	// 충분히 가까이 있으면, 이웃 갯수를 탐색. 2개 이하면 OK.
		{
			_uint inumNeighbors = 0;
			for (_uint i = 0; i < ENUM_CLASS(CELLLINE::END); i++)
				if (m_iNeighborIndices[i] != -1)
					inumNeighbors++;

			if (inumNeighbors < 3)	// OK? 좋아 이걸로 가. 현재 인덱스도 네비에서 갱신.
			{
				return true;		// 겹치는 점 있음!	// 이거 지금 Cell 이 계속 걸림. 그럼 CurrentCell을 무조건 Pass 하게 해야 하나?
			}
			else					// OK가 아냐? 그럼 다른 Cell 탐색해.
			{
				isIn(vPosition, pNeighborIndex); // 플레이랑 가까운 방향의 cell 탐색으로 갱신하는 역할
				return false;		// 겹치는 점 없음!
			}
		}
	}

	// 없다면, isIn 함수를 이용 주변의 다른 cell로 pNeighborIndex 를 전환. 이웃하는 셀이 없어서, pNeighborIndex 에 -1이 들어가는 게 문제. 
	isIn(vPosition, pNeighborIndex);
	return false;
}



_bool CCell::Compare_Points(_fvector vSourPoint, _fvector vDestPoint)
{
	if (true == XMVector3Equal(XMLoadFloat3(&m_vPoints[ENUM_CLASS(CELLPOINT::A)]), vSourPoint))
	{
		if (true == XMVector3Equal(XMLoadFloat3(&m_vPoints[ENUM_CLASS(CELLPOINT::B)]), vDestPoint))
			return true;
		if (true == XMVector3Equal(XMLoadFloat3(&m_vPoints[ENUM_CLASS(CELLPOINT::C)]), vDestPoint))
			return true;
	}

	if (true == XMVector3Equal(XMLoadFloat3(&m_vPoints[ENUM_CLASS(CELLPOINT::B)]), vSourPoint))
	{
		if (true == XMVector3Equal(XMLoadFloat3(&m_vPoints[ENUM_CLASS(CELLPOINT::C)]), vDestPoint))
			return true;
		if (true == XMVector3Equal(XMLoadFloat3(&m_vPoints[ENUM_CLASS(CELLPOINT::A)]), vDestPoint))
			return true;
	}

	if (true == XMVector3Equal(XMLoadFloat3(&m_vPoints[ENUM_CLASS(CELLPOINT::C)]), vSourPoint))
	{
		if (true == XMVector3Equal(XMLoadFloat3(&m_vPoints[ENUM_CLASS(CELLPOINT::A)]), vDestPoint))
			return true;
		if (true == XMVector3Equal(XMLoadFloat3(&m_vPoints[ENUM_CLASS(CELLPOINT::B)]), vDestPoint))
			return true;
	}

	return false;
}

_float CCell::Compute_Height(_fvector vLocalPos)
{
	// y = (-ax - cz - d) / b;

	_vector		vPlane = XMPlaneFromPoints(
		XMVectorSetW(XMLoadFloat3(&m_vPoints[ENUM_CLASS(CELLPOINT::A)]), 1.f),
		XMVectorSetW(XMLoadFloat3(&m_vPoints[ENUM_CLASS(CELLPOINT::B)]), 1.f),
		XMVectorSetW(XMLoadFloat3(&m_vPoints[ENUM_CLASS(CELLPOINT::C)]), 1.f)
	);

	return (XMVectorGetX(vPlane) * -1.f * XMVectorGetX(vLocalPos) - vPlane.m128_f32[2] * vLocalPos.m128_f32[2] - vPlane.m128_f32[3]) / vPlane.m128_f32[1];
}

#ifdef _DEBUG
HRESULT CCell::Render()
{
	m_pVIBuffer->Bind_Resources();
	m_pVIBuffer->Render();

	return S_OK;
}
#endif

CCell* CCell::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _float3* pPoints, _int iIndex)
{
	CCell* pInstance = new CCell(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pPoints, iIndex)))
	{
		MSG_BOX(TEXT("Failed to Created : CCell"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCell::Free()
{
	__super::Free();

#ifdef _DEBUG
	Safe_Release(m_pVIBuffer);
#endif

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}

#include "Navigation.h"
#include "Cell.h"


#include "GameInstance.h"

_float4x4 CNavigation::m_WorldMatrix = {};

CNavigation::CNavigation(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent{ pDevice, pContext }
{
}

CNavigation::CNavigation(const CNavigation& Prototype)
	: CComponent{ Prototype }
	, m_Cells{ Prototype.m_Cells }
#ifdef _DEBUG	
	, m_pShader{ Prototype.m_pShader }
#endif
{
	for (auto& pCell : m_Cells)
		Safe_AddRef(pCell);

#ifdef _DEBUG	
	Safe_AddRef(m_pShader);
#endif
}

HRESULT CNavigation::Initialize_Prototype(const _tchar* pNavigationFilePath)
{
	_ulong			dwByte = { };
	HANDLE			hFile = CreateFile(pNavigationFilePath, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	_uint iNumTris = 0;
	ReadFile(hFile, &iNumTris, sizeof(_uint), &dwByte, nullptr);

	while (true)
	{
		_float3		vPoints[3];

		ReadFile(hFile, vPoints, sizeof(_float3) * 3, &dwByte, nullptr);
		if (0 == dwByte)
			break;

		CCell* pCell = CCell::Create(m_pDevice, m_pContext, vPoints, m_Cells.size());
		if (nullptr == pCell)
			return E_FAIL;

		m_Cells.push_back(pCell);
	}

	SetUp_Neighbors();

#ifdef _DEBUG
	m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Engine_Shader_Cell.hlsl"), VTXPOS::Elements, VTXPOS::iNumElements);
	if (nullptr == m_pShader)
		return E_FAIL;
#endif

	return S_OK;
}

HRESULT CNavigation::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return S_OK;

	NAVIGATION_DESC* pDesc = static_cast<NAVIGATION_DESC*>(pArg);

	m_iCurrentCellIndex = pDesc->iCurrentCellIndex;
	m_iPastCellIndex = m_iCurrentCellIndex;

	return S_OK;
}

void CNavigation::Update(_fmatrix WorldMatrix)
{
	XMStoreFloat4x4(&m_WorldMatrix, WorldMatrix);

}

_bool CNavigation::isMove(_fvector vDestPos, _vector vOriginPos, _vector* pOutPos)
{
	// 작동 순서
	// 1. 네비메쉬 로컬좌표 기준 플레이어의 상대좌표를 구함. 이를 기준으로 확인할 것
	// ㄴ 처음에는 iNeighborIndex 값을 더미로 둠.
	// ㄴ 현재 cell에서 검사 후 여기에 없으면 인접 방향 cell의 인덱스가 할당.
	// 2. "현재 Cell  " 에 있는지 검사.		있으면 True / 없으면 3번으로.
	// 3. "이웃 Cell들" 에 있는지 검사.		있으면 True / 없으면 False.


	// ==============================
	// || 안에 있는지 확인
	// ==============================

	_bool	isInCell = false;

	// 1번
	_vector vLocalDestPos = XMVector3TransformCoord(vDestPos, XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_WorldMatrix)));
	_vector vLocalOriginPos = XMVector3TransformCoord(vOriginPos, XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_WorldMatrix)));

	_int		iNeighborIndex = { -1 };

	// 2번
	if (true == m_Cells[m_iCurrentCellIndex]->isIn(vLocalDestPos, &iNeighborIndex)) // 현재위치 그대로임
		isInCell = true;
	// 3번
	else
	{
		if (-1 != iNeighborIndex)		// 이웃 있음
		{
			// 계속 검사
			while (true)
			{	
				// 모든 Cell을 돌았으나 어디에도 없음
				if (-1 == iNeighborIndex)
				{
					isInCell = false;
					break;
				}
				// 인접 셀로 계속 퍼지며 검사. iNeighborIndex 는 내부에서 계속 변화함
				if (true == m_Cells[iNeighborIndex]->isIn(vLocalDestPos, &iNeighborIndex))
				{
					m_iPastCellIndex = m_iCurrentCellIndex;
					m_iCurrentCellIndex = iNeighborIndex;

					isInCell = true;
					break;
				}
			}
		}
		else							// 이웃 없음
			isInCell = false;
	}


	if (isInCell)
	{
		*pOutPos = vLocalDestPos;
		return true;
	}

	// ==============================
	// || 안에 없으면 선타도록
	// ==============================

	// 1. 가장자리 Cell 들 탐색
	vector<_uint> vecEdgeCellIndices = {};
	
	for (_uint i = 0; i < m_Cells.size(); i++)
	{
		for (_uint j = 0; j < ENUM_CLASS(CELLLINE::END); j++)
			if (m_Cells[i]->Get_Neighbor()[j] == -1)
			{
				vecEdgeCellIndices.push_back(i);
				break;
			}
	}

	// 2. destpos 와 가장 가까운 가장자리 선분을 찾음
	//
	// - 점과 선분 사이의 최소 거리를 구함
	// - 만약 해당 선분과 외곽라인이 겹치지 않는다면, 가까운 점을 기준으로 탐색함

	_uint iNearest_CellIndex			= UINT_MAX;		// 가장 가까운 Edge를 가진 Cell의 Index
	_uint iNearest_CellEdgeIndex		= UINT_MAX;		// 가장 가까운 Edge를 가진 Cell 내에서 가까운 Edge 자체의 인덱스

	_float fNearest_EdgeDist			= FLT_MAX;		// 비교 후 갱신을 위한 임시 거리저장

	_vector vNearest_Point				= XMVectorZero();

	for (auto& cellIndex : vecEdgeCellIndices)
	{
		CCell* pTargetCell = m_Cells[cellIndex];	// 현재 인덱스의 Cell
		vector<_uint> vecEdgeIndices = {};			// Cell 내의 가장자리 선분 인덱스

		for (_uint i = 0; i < ENUM_CLASS(CELLLINE::END); i++)
			if (m_Cells[cellIndex]->Get_Neighbor()[i] == -1)
				vecEdgeIndices.push_back(i);

		// 한 Cell 내에서. 가장 가까운 가장자리 선분을 찾음
		// ksta : 외곽 edge가 두개 이상일 때에 문제 발생함
		_uint iNearestEdgeIndex = UINT_MAX;
		_float fNearestDist		= FLT_MAX;
		_vector vNearPoint		= XMVectorZero();

		for (auto& edgeIndex : vecEdgeIndices)
		{
			_vector vPoint_LineStart	= pTargetCell->Get_Point(static_cast<CELLPOINT>(edgeIndex));
			_vector vPoint_LineEnd		= pTargetCell->Get_Point(static_cast<CELLPOINT>((edgeIndex + 1) % ENUM_CLASS(CELLPOINT::END)));

			_vector vPoint_DestPos		= vLocalDestPos;
			_vector vPoint_Intersect	= XMVectorZero();

			_float fDist = Calc_NearistDist(vPoint_LineStart, vPoint_LineEnd, vPoint_DestPos, &vPoint_Intersect);


			if (fNearestDist > fDist)
			{
				fNearestDist = fDist;
				iNearestEdgeIndex = edgeIndex;

				vNearPoint = vPoint_Intersect;
			}
		}

		// 현재 값보다 더 가까우면 갱신
		if (fNearest_EdgeDist > fNearestDist)
		{
			iNearest_CellIndex		= cellIndex;
			iNearest_CellEdgeIndex	= iNearestEdgeIndex;

			fNearest_EdgeDist		= fNearestDist;

			vNearest_Point			= vNearPoint;
		}
	}


	// 3. 해당 위치로 이동
	if (iNearest_CellIndex != UINT_MAX)
	{
		m_iCurrentCellIndex = iNearest_CellIndex;
		*pOutPos = vNearest_Point;
		return true;
	}
	else
		return false;
}

_vector CNavigation::Compute_OnCell(_fvector vPosition)
{
	_vector vLocalPos = XMVector3TransformCoord(vPosition, XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_WorldMatrix)));

	_float		fHeight = m_Cells[m_iCurrentCellIndex]->Compute_Height(vLocalPos);

	vLocalPos = XMVectorSetY(vLocalPos, fHeight);

	return XMVector3TransformCoord(vLocalPos, XMLoadFloat4x4(&m_WorldMatrix));
}

_float CNavigation::Calc_NearistDist(_vector vLineStart, _vector vLineEnd, _vector vPoint, _vector* vNearPointOut)
{
    _vector AB = XMVectorSubtract(vLineEnd, vLineStart);
	_vector AP = XMVectorSubtract(vPoint, vLineStart);

    _float ab_ab = XMVectorGetX(XMVector3Dot(AB, AB));
    _float ap_ab = XMVectorGetX(XMVector3Dot(AP, AB));

	_float t = ap_ab / ab_ab;

    if (t < 0.0f) {
        // A가 가장 가까움
		*vNearPointOut = vLineStart;
        return XMVectorGetX(XMVector3Length(XMVectorSubtract(vPoint, vLineStart)));
    }
    else if (t > 1.0f) {
        // B가 가장 가까움
		*vNearPointOut = vLineEnd;
        return XMVectorGetX(XMVector3Length(XMVectorSubtract(vPoint, vLineEnd)));
    }
    else {
        // 선분 위에 교점 존재
		*vNearPointOut = XMVectorAdd(vLineStart, XMVectorScale(AB, t));
        return XMVectorGetX(XMVector3Length(XMVectorSubtract(vPoint, *vNearPointOut)));
    }
}

#ifdef _DEBUG

HRESULT CNavigation::Render()
{
	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
		return E_FAIL;

	_float4		vColor = {};

	_float4x4	WorldMatrix = m_WorldMatrix;

	if (-1 == m_iCurrentCellIndex)
	{
		vColor = _float4(0.f, 1.f, 0.f, 1.f);

		if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &WorldMatrix)))
			return E_FAIL;

		m_pShader->Bind_RawValue("g_vColor", &vColor, sizeof(_float4));

		m_pShader->Begin(0);

		for (auto& pCell : m_Cells)
			pCell->Render();
	}

	else
	{
		vColor = _float4(1.f, 0.f, 0.f, 1.f);

		WorldMatrix._42 += 0.1f;

		if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &WorldMatrix)))
			return E_FAIL;

		m_pShader->Bind_RawValue("g_vColor", &vColor, sizeof(_float4));

		m_pShader->Begin(0);

		m_Cells[m_iCurrentCellIndex]->Render();
	}



	return S_OK;
}

#endif

void CNavigation::SetUp_Neighbors()
{
	for (auto& pSourCell : m_Cells)
	{
		for (auto& pDestCell : m_Cells)
		{
			if (pSourCell == pDestCell)
				continue;

			if (true == pDestCell->Compare_Points(pSourCell->Get_Point(CELLPOINT::A), pSourCell->Get_Point(CELLPOINT::B)))
			{
				pSourCell->Set_Neighbor(CELLLINE::AB, pDestCell);
			}
			if (true == pDestCell->Compare_Points(pSourCell->Get_Point(CELLPOINT::B), pSourCell->Get_Point(CELLPOINT::C)))
			{
				pSourCell->Set_Neighbor(CELLLINE::BC, pDestCell);
			}
			if (true == pDestCell->Compare_Points(pSourCell->Get_Point(CELLPOINT::C), pSourCell->Get_Point(CELLPOINT::A)))
			{
				pSourCell->Set_Neighbor(CELLLINE::CA, pDestCell);
			}
		}
	}
}

CNavigation* CNavigation::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pNavigationFilePath)
{
	CNavigation* pInstance = new CNavigation(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pNavigationFilePath)))
	{
		MSG_BOX(TEXT("Failed to Created : CNavigation"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CNavigation::Clone(void* pArg)
{
	CNavigation* pInstance = new CNavigation(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CNavigation"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CNavigation::Free()
{
	__super::Free();

	for (auto& pCell : m_Cells)
		Safe_Release(pCell);

	m_Cells.clear();

#ifdef _DEBUG	
	Safe_Release(m_pShader);
#endif
}

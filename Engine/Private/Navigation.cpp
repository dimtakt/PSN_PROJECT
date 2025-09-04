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
	for (auto& cellIndex : vecEdgeCellIndices)
	{
		CCell* pTargetCell = m_Cells[cellIndex];	// 현재 인덱스의 Cell
		vector<_uint> vecEdgeIndices = {};			// Cell 내의 가장자리 선분 인덱스

		for (_uint i = 0; i < ENUM_CLASS(CELLLINE::END); i++)
			if (m_Cells[i]->Get_Neighbor()[i] == -1)
				vecEdgeIndices.push_back(i);



	}

	// 3. 해당 위치로 이동


#pragma region oldBackup
	/*
	
	

	// 이동 벡터
	_vector vMoveDir = vLocalDestPos - vLocalOriginPos;

	// 현재 Cell의 세 점
	_vector vPoint[ENUM_CLASS(CELLPOINT::END)] = {};
	
	for (_uint i = 0; i < ENUM_CLASS(CELLPOINT::END); i++)
		vPoint[i] = m_Cells[m_iCurrentCellIndex]->Get_Point(static_cast<CELLPOINT>(i));

	// AB, BC, CA 순서로 시계방향으로 그려짐을 이용
	//
	// 선분은 교차하는 선분을 대상으로 함
	// 
	// 1. 점의 좌표 순서를 통해 선의 바깥쪽 방향 벡터를 구함
	// 2. 선분에 수직하는 좌표의 범위를 계산
	// 3. 플레이어의 destPos 가 범위 내에 있는지 검사
	// 3-1. 있다면) 벡터분리를 통해 선을 타도록
	// 3-2. 없다면) 이동할 Cell 탐색.
	//				기준은 이웃 Cell이 3개가 아닌 가장자리 Cell인지, 현재 내 위치와 가까운 Point를 가지고 있는지
	// 3-2-1.	대상 Cell이 2개이상이라면)	목적지 좌표가 해당 Cell 내부인지 확인. 맞으면 거기로. 다만 반례가 존재.
	// 3-2-1-2.		(반례 대응)				목적지 좌표가 가장자리 CellLine의 Point와 가까운 Cell인지. 맞으면 거기로.
	// 3-2-2.	1개 이상이라면)				그냥 거기로 이동.
	

	// 0. OriginPos -> DestPos 와 교차하는 Cell의 선분을 구함
	
	// 찾은 선분의 정보가 담길 변수
	_vector vStart = XMVectorZero();
	_vector vEnd = XMVectorZero();
	_bool isFindLine = false;

	_uint iMaxCellLine = ENUM_CLASS(CELLLINE::END);
	for (_uint i = 0; i < iMaxCellLine; i++)
	{
		_vector vTmpStart = vPoint[i];
		_vector vTmpEnd = vPoint[(i + 1) % iMaxCellLine];

		Vec2 CellLine2DPos[2] = {
			{XMVectorGetX(vTmpStart), XMVectorGetZ(vTmpStart)},
			{XMVectorGetX(vTmpEnd), XMVectorGetZ(vTmpEnd)}
		};
		Vec2 Player2DPos[2] = {
			{XMVectorGetX(vLocalOriginPos), XMVectorGetZ(vLocalOriginPos)},
			{XMVectorGetX(vLocalDestPos), XMVectorGetZ(vLocalDestPos)}
		};

		if (IsIntersect(CellLine2DPos[0], CellLine2DPos[1], Player2DPos[0], Player2DPos[1]))	// 겹치는 선분을 발견 시
		{
			vStart = vTmpStart;
			vEnd = vTmpEnd;

			isFindLine = true;
			break;
		}		
	}

	if (!isFindLine)
	{
		std::cout << "Cannot Find Line!" << std::endl;
		return false;
	}

	// 1. y축 단위벡터와 0에서 구했던 선분을 외적하여, xz축 상의 바깥쪽 법선을 구함

	_vector vOutsideNormal = XMVectorZero();

	_vector vCellDir = XMVector3Normalize(vEnd - vStart);
	_vector vYDir = XMVectorSet(0.f, 1.f, 0.f, 0.f);

	vOutsideNormal = XMVector3Cross(vCellDir, vYDir);
	_float4 Stored;
	XMStoreFloat4(&Stored, vOutsideNormal);


	// 2. 선분과 겹치는 법선이 존재가능한 범위를 계산
	// 3. 플레이어의 destPos 가 2번의 범위 내에 있는지 조건을 검사

	_vector vEdge = vEnd - vStart;
	_vector vDiff = vLocalDestPos - vStart;

	_bool isInRange = false;

	_float det = XMVectorGetX(vOutsideNormal) * XMVectorGetZ(-vEdge) - XMVectorGetZ(vOutsideNormal) * XMVectorGetX(-vEdge);
	if (fabs(det) < 1e-6f)
		isInRange = false;
	else
	{
		_float u = (XMVectorGetZ(vOutsideNormal) * XMVectorGetX(vDiff) - XMVectorGetX(vOutsideNormal) * XMVectorGetZ(vDiff)) / det;

		if (u >= 0.f && u <= 1.f)
			isInRange = true;  // 교차
		else
			isInRange = false;     // 교차 안 함
	}


	// 3-1. 있다) 벡터분리를 통해 선을 타도록
	if (isInRange)
	{
		// 교차 좌표 구하기
		_float u = (XMVectorGetZ(vOutsideNormal) * XMVectorGetX(vDiff) - XMVectorGetX(vOutsideNormal) * XMVectorGetZ(vDiff)) / det;

		_vector vHitPos = vStart + u * vEdge;

		// 교차점 좌표로 보정
		*pOutPos = vHitPos;

		std::cout << "Try Sliding" << std::endl;
		return true;
	}
	// 3-2. 없다) 이동할 Cell 탐색.
	else
	{
		// 1. 일단 전체 Cell 순회하되, 포인트 겹치고 이웃 갯수 3개 미만인 Cell 탐색
		// 2. 해당 Cell로 이동판정 내림


		const _float EPS = 0.4f; // 오차 허용 범위
		vector<_uint> vecFindIndex = {};

		for (_uint i = 0; i < m_Cells.size(); i++)
		{
			CCell* cell = m_Cells[i];


			// 일단 현재 Cell은 패스. 넘어갈 Cell의 탐색 과정이므로
			if (m_iCurrentCellIndex == i)
				continue;
			

			// 이웃하는 Cell이 3개 미만인지
			_uint iNumNeighbors = 0;

			_int* pNeighbors = cell->Get_Neighbor();
            _int iNeighbors[3] = { pNeighbors[0], pNeighbors[1], pNeighbors[2] };


			for (auto& neighbor : iNeighbors)
				if (neighbor != -1)
					iNumNeighbors++;

			if (iNumNeighbors >= 3)
				continue;


			// 일치하는 점이 있는지
			_bool isFind = false;
			for (_uint i = 0; i < ENUM_CLASS(CELLPOINT::END); i++)
			{
				_vector cellPointPos = cell->Get_Point(static_cast<CELLPOINT>(i));
				_float fDistance = XMVectorGetX(XMVector3Length(vLocalDestPos - cellPointPos));

				if (fDistance <= EPS)
				{
					isFind = true;
					break;
				}
			}

			if (isFind)
				vecFindIndex.push_back(i);
		}


		// 3-2-1
		if (vecFindIndex.size() >= 2)
		{
			std::cout << "이동 후보 Cell에 사이즈 2 이상 들어감." << std::endl;
			_int iTmpIndex = { -1 };

			// 이동 후보 Cell 내에 있는지 확인. 있다면 해당 Cell로.
			for (auto& index : vecFindIndex)
				if (m_Cells[index]->isIn(vLocalDestPos, &iTmpIndex))
				{
					m_iCurrentCellIndex = index;
					std::cout << "2] Moved to Other Cells. when sliding." << std::endl;
					// 교차 좌표 구하기
					_float u = (XMVectorGetZ(vOutsideNormal) * XMVectorGetX(vDiff) - XMVectorGetX(vOutsideNormal) * XMVectorGetZ(vDiff)) / det;

					_vector vHitPos = vStart + u * vEdge;

					// 교차점 좌표로 보정
					*pOutPos = vHitPos;
					return true;
				}

			// 3-2-1-2
			// 이동 후보 Cell 의 이웃 없는 선분의 꼭짓점이 현재 캐릭터의 위치랑 일치하는지 여부
			// 
			// (일치하지 않는다면, 가장자리 Cell은 맞으나 캐릭터의 이동방향에 슬라이딩이 불가능한 Cell을 뜻하므로 pass)
			// 해당 반례 이미지 : "../../Client/Bin/Resources/_dummyInfoTexture_ForDebug/mspaint_2025-09-04_13-30-15.png";
			for (auto& index : vecFindIndex)
			{
				auto targetCell = m_Cells[index];

				vector<_int>	vecTargetLineIndices = {};	// 이웃이 없는 라인 인덱스를 담음
				_int* pNeighborIndices = targetCell->Get_Neighbor();
				
				for (_int i = 0; i < ENUM_CLASS(CELLPOINT::END); i++)
				{
					if (pNeighborIndices[i] == -1)
						vecTargetLineIndices.push_back(i);
				}

				for (auto& lineIndex : vecTargetLineIndices)
				{
					_vector vTargetPoints[2] = {};

					vTargetPoints[0] = targetCell->Get_Point(static_cast<CELLPOINT>(lineIndex));
					vTargetPoints[1] = targetCell->Get_Point(static_cast<CELLPOINT>((lineIndex + 1) % ENUM_CLASS(CELLPOINT::END)));

					for (auto& point : vTargetPoints)
					{
						_float fDistance = XMVectorGetX(XMVector3Length(point - vLocalDestPos));
						if (fDistance <= EPS)
						{
							m_iCurrentCellIndex = index;
							std::cout << "2-2]Moved to Other Cells. when sliding." << std::endl;
							// 교차 좌표 구하기
							// 현재 cell 기준으로 작동해서 그런가? outsidenormal 이랑 diff 이런걸 넘어간 cell의 해당 면 것으로 다시 계산해줘야 할 듯

							_float u = (XMVectorGetZ(vOutsideNormal) * XMVectorGetX(vDiff) - XMVectorGetX(vOutsideNormal) * XMVectorGetZ(vDiff)) / det;

							_vector vHitPos = vStart + u * vEdge;

							// 교차점 좌표로 보정
							*pOutPos = vHitPos;
							return true;
						}
					}
				}

			}
		}
		// 3-2-2
		if (vecFindIndex.size() >= 1)
		{
			// 조건이 2개인데 위에서 처리되지 않으면 에러 발생
			if (vecFindIndex.size() >= 2)
				assert(false);

			
			m_iCurrentCellIndex = vecFindIndex[0];	
			std::cout << "Moved to Other Cells. when sliding." << std::endl;
			// 교차 좌표 구하기
			_float u = (XMVectorGetZ(vOutsideNormal) * XMVectorGetX(vDiff) - XMVectorGetX(vOutsideNormal) * XMVectorGetZ(vDiff)) / det;

			_vector vHitPos = vStart + u * vEdge;

			// 교차점 좌표로 보정
			*pOutPos = vHitPos;
			return true;
		}
		
	

		if (XMVectorGetX(XMVector3LengthSq(vLocalDestPos - vStart)) < XMVectorGetX(XMVector3LengthSq(vLocalDestPos - vEnd)))
			*pOutPos = vStart;
		else
			*pOutPos = vEnd;

		std::cout << "Edge Point." << std::endl;
		return true;
	}

	
	
	
	*/
#pragma endregion














}

_vector CNavigation::Compute_OnCell(_fvector vPosition)
{
	_vector vLocalPos = XMVector3TransformCoord(vPosition, XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_WorldMatrix)));

	_float		fHeight = m_Cells[m_iCurrentCellIndex]->Compute_Height(vLocalPos);

	vLocalPos = XMVectorSetY(vLocalPos, fHeight);

	return XMVector3TransformCoord(vLocalPos, XMLoadFloat4x4(&m_WorldMatrix));
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

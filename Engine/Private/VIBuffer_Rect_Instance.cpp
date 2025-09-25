#include "VIBuffer_Rect_Instance.h"

#include "GameInstance.h"
#include "GameObject.h"

CVIBuffer_Rect_Instance::CVIBuffer_Rect_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer_Instance{ pDevice, pContext }
{
}

CVIBuffer_Rect_Instance::CVIBuffer_Rect_Instance(const CVIBuffer_Rect_Instance& Prototype)
	: CVIBuffer_Instance{ Prototype }
	, m_vPivot{ Prototype.m_vPivot }
	, m_pSpeeds{ Prototype.m_pSpeeds }
	, m_isLoop{ Prototype.m_isLoop }
	, m_vCenter{ Prototype.m_vCenter }
	, m_vRange{ Prototype.m_vRange }
	, m_isTurn{ Prototype.m_isTurn }
	, m_pTurnSpeeds{ Prototype.m_pTurnSpeeds }
	, m_pAxises{ Prototype.m_pAxises }
	, m_fMaxLifetime{ Prototype.m_fMaxLifetime }
	, m_pScales{ Prototype.m_pScales }
{
}

HRESULT CVIBuffer_Rect_Instance::Initialize_Prototype(const INSTANCE_DESC* pDesc)
{
	const RECT_INSTANCE_DESC* pRectDesc = static_cast<const RECT_INSTANCE_DESC*>(pDesc);

	m_vPivot = pRectDesc->vPivot;
	m_isLoop = pRectDesc->isLoop;

	m_vCenter = pRectDesc->vCenter;
	m_vRange = pRectDesc->vRange;
	m_isTurn = pRectDesc->isTurn;

	m_fMaxLifetime = pRectDesc->vLifeTime.y;


	m_iNumIndexPerInstance = 6;
	m_iInstanceVertexStride = sizeof(VTXINSTANCE_PARTICLE);
	m_iNumInstance = pRectDesc->iNumInstance;
	m_iNumVertices = 4;
	m_iVertexStride = sizeof(VTXPOSTEX);
	m_iNumIndices = 6;
	m_iIndexStride = 2;
	m_iNumVertexBuffers = 2;
	m_eIndexFormat = DXGI_FORMAT_R16_UINT;
	m_ePrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	D3D11_BUFFER_DESC		VBDesc{};
	VBDesc.ByteWidth = m_iNumVertices * m_iVertexStride;
	VBDesc.Usage = D3D11_USAGE_DEFAULT;
	VBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	VBDesc.CPUAccessFlags = 0;
	VBDesc.MiscFlags = 0;
	VBDesc.StructureByteStride = m_iVertexStride;

	VTXPOSTEX* pVertices = new VTXPOSTEX[m_iNumVertices];

	pVertices[0].vPosition = _float3(-0.5f, 0.5f, 0.f);
	pVertices[0].vTexcoord = _float2(0.f, 0.f);

	pVertices[1].vPosition = _float3(0.5f, 0.5f, 0.f);
	pVertices[1].vTexcoord = _float2(1.f, 0.f);

	pVertices[2].vPosition = _float3(0.5f, -0.5f, 0.f);
	pVertices[2].vTexcoord = _float2(1.f, 1.f);

	pVertices[3].vPosition = _float3(-0.5f, -0.5f, 0.f);
	pVertices[3].vTexcoord = _float2(0.f, 1.f);

	D3D11_SUBRESOURCE_DATA	VBInitialData{};
	VBInitialData.pSysMem = pVertices;

	if (FAILED(m_pDevice->CreateBuffer(&VBDesc, &VBInitialData, &m_pVB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

	D3D11_BUFFER_DESC		IBDesc{};
	IBDesc.ByteWidth = m_iNumIndices * m_iIndexStride;
	IBDesc.Usage = D3D11_USAGE_DEFAULT;
	IBDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	IBDesc.CPUAccessFlags = 0;
	IBDesc.MiscFlags = 0;
	IBDesc.StructureByteStride = m_iIndexStride;

	_ushort* pIndices = new _ushort[m_iNumIndices];

	pIndices[0] = 0;
	pIndices[1] = 1;
	pIndices[2] = 2;

	pIndices[3] = 0;
	pIndices[4] = 2;
	pIndices[5] = 3;

	D3D11_SUBRESOURCE_DATA	IBInitialData{};
	IBInitialData.pSysMem = pIndices;

	if (FAILED(m_pDevice->CreateBuffer(&IBDesc, &IBInitialData, &m_pIB)))
		return E_FAIL;

	Safe_Delete_Array(pIndices);


	/* 복제되는 렉트인스턴스버퍼는 각각 m_pVBInstance독립적으로 가지길 바란다. */
	/* 실질적인 인스턴스 정점버퍼의 경우에는 사본이 생성될때 할당되게 만들어줄 것이다. */
	/* dx11에서는 인스턴스 정점버퍼를 할당할 경우 복사해서 채워줄 정점의 데이터가 사전에 필요하다 */

	/* 복사해서 채워줄 정점의 사전데이터다.(사본마다 하나씩 만들어줄 이유강벗으나가ㅓ  */
	m_VBInstanceDesc.ByteWidth = m_iNumInstance * m_iInstanceVertexStride;
	m_VBInstanceDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_VBInstanceDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_VBInstanceDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_VBInstanceDesc.MiscFlags = 0;
	m_VBInstanceDesc.StructureByteStride = m_iInstanceVertexStride;

	m_pInstanceVertices = new VTXINSTANCE_PARTICLE[m_iNumInstance];
	m_pSpeeds			= new _float[m_iNumInstance];
	m_pTurnSpeeds		= new _float[m_iNumInstance];
	m_pAxises			= new _vector[m_iNumInstance];
	m_pScales			= new _float[m_iNumInstance];

	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		VTXINSTANCE_PARTICLE* pInstanceVertices = static_cast<VTXINSTANCE_PARTICLE*>(m_pInstanceVertices);

		_float		fScale = m_pGameInstance->Rand(pRectDesc->vSize.x, pRectDesc->vSize.y);
		_float		fLifeTime = m_pGameInstance->Rand(pRectDesc->vLifeTime.x, pRectDesc->vLifeTime.y);
		m_pSpeeds[i] = m_pGameInstance->Rand(pRectDesc->vSpeed.x, pRectDesc->vSpeed.y);


		if (m_isTurn)
#pragma region Initialize Scale & Rotation
		{
			// ===== 최초 회전값 지정 =====
			_float randYaw = m_pGameInstance->Rand(0.f, XM_2PI);
			_float randPitch = m_pGameInstance->Rand(0.f, XM_2PI);
			_float randRoll = m_pGameInstance->Rand(0.f, XM_2PI);
			_matrix matRot = XMMatrixRotationRollPitchYaw(randPitch, randYaw, randRoll);
			_matrix matSca = XMMatrixScaling(fScale, fScale, fScale);
			_matrix matResult = matSca * matRot;
			XMStoreFloat4(&pInstanceVertices[i].vRight, matResult.r[0]);
			XMStoreFloat4(&pInstanceVertices[i].vUp, matResult.r[1]);
			XMStoreFloat4(&pInstanceVertices[i].vLook, matResult.r[2]);

			// ===== 회전방향 지정 =====
			_float3 vRotAxis = {};
			vRotAxis.x = m_pGameInstance->Rand(0.f, 1.f);
			vRotAxis.y = m_pGameInstance->Rand(0.f, 1.f);
			vRotAxis.z = m_pGameInstance->Rand(0.f, 1.f);
			m_pAxises[i] = XMVector3Normalize(XMLoadFloat3(&vRotAxis));

			// ===== 회전속도 지정 =====
			m_pTurnSpeeds[i] = m_pGameInstance->Rand(pRectDesc->vTurnSpeed.x, pRectDesc->vTurnSpeed.y);

			// ==============================
		}
#pragma endregion
		else
#pragma region Initialize Scale
		{
			m_pScales[i] = m_pGameInstance->Rand(pRectDesc->vSize.x, pRectDesc->vSize.y);

			pInstanceVertices[i].vRight = _float4(fScale, 0.f, 0.f, 0.f);
			pInstanceVertices[i].vUp = _float4(0.f, fScale, 0.f, 0.f);
			pInstanceVertices[i].vLook = _float4(0.f, 0.f, fScale, 0.f);
		}
#pragma endregion



#pragma region Initialize Position (old)

		// ===== 최초 위치값 지정 =====
		// .. 인데 매 프레임마다 갱신시켜서 위치 완전히 랜덤이도록 변경함
		
		//pInstanceVertices[i].vTranslation = _float4(
		//	m_pGameInstance->Rand(pRectDesc->vCenter.x - pRectDesc->vRange.x * 0.5f, pRectDesc->vCenter.x + pRectDesc->vRange.x * 0.5f),
		//	m_pGameInstance->Rand(pRectDesc->vCenter.y - pRectDesc->vRange.y * 0.5f, pRectDesc->vCenter.y + pRectDesc->vRange.y * 0.5f),
		//	m_pGameInstance->Rand(pRectDesc->vCenter.z - pRectDesc->vRange.z * 0.5f, pRectDesc->vCenter.z + pRectDesc->vRange.z * 0.5f),
		//	1.f
		//);

#pragma endregion


		pInstanceVertices[i].vLifeTime = _float2(0.f, fLifeTime);
	}

	return S_OK;
}

HRESULT CVIBuffer_Rect_Instance::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	// 위치는 생성 시 마다 재정의
	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		VTXINSTANCE_PARTICLE* pInstanceVertices = static_cast<VTXINSTANCE_PARTICLE*>(m_pInstanceVertices);

		pInstanceVertices[i].vTranslation = _float4(
			m_pGameInstance->Rand(m_vCenter.x - m_vRange.x * 0.5f, m_vCenter.x + m_vRange.x * 0.5f),
			m_pGameInstance->Rand(m_vCenter.y - m_vRange.y * 0.5f, m_vCenter.y + m_vRange.y * 0.5f),
			m_pGameInstance->Rand(m_vCenter.z - m_vRange.z * 0.5f, m_vCenter.z + m_vRange.z * 0.5f),
			1.f
		);
	}

	return S_OK;
}

void CVIBuffer_Rect_Instance::Spread(_float fTimeDelta)
{
	m_fElapsedTime += fTimeDelta;

	D3D11_MAPPED_SUBRESOURCE	SubResource{};

	VTXINSTANCE_PARTICLE* pInstanceVertices = static_cast<VTXINSTANCE_PARTICLE*>(m_pInstanceVertices);

	/*m_pVB->Lock(0, 0, (void**)&pVertex, 0);*/

	// 컨텍스트로부터 현재 인스턴스 버퍼의 정보를 받아옴. 이는 GPU도 셰이더를 위해 참조하는 것.
	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	VTXINSTANCE_PARTICLE* pVertices = static_cast<VTXINSTANCE_PARTICLE*>(SubResource.pData);

	// 단순히 인스턴스의 생성 위치에 따라, 중앙 위치의 반대 방향으로 이동하는 코드
	// 그 외의 세부사항은 셰이더 및 desc 값에 따라 조절

	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		// 단순 방사 방향 구함
		_vector	vMoveDir = XMVector3Normalize(XMVectorSetW(XMLoadFloat4(&pVertices[i].vTranslation) - XMLoadFloat3(&m_vPivot), 0.f));

		XMStoreFloat4(&pVertices[i].vTranslation, XMLoadFloat4(&pVertices[i].vTranslation) + vMoveDir * m_pSpeeds[i] * fTimeDelta);
		pVertices[i].vLifeTime.x += fTimeDelta;

		if (true == m_isLoop)
		{
			if (pVertices[i].vLifeTime.x >= pVertices[i].vLifeTime.y)		// ElapsedTime 이 MaxTime 넘기면 다시 돌아가서 루프
			{
				pVertices[i].vLifeTime.x = 0.f;
				pVertices[i].vTranslation = pInstanceVertices[i].vTranslation;
			}
		}
	}

	m_pContext->Unmap(m_pVBInstance, 0);
}

void CVIBuffer_Rect_Instance::Spread_Turn(_float fTimeDelta)
{
	m_fElapsedTime += fTimeDelta;

	D3D11_MAPPED_SUBRESOURCE	SubResource{};

	VTXINSTANCE_PARTICLE* pInstanceVertices = static_cast<VTXINSTANCE_PARTICLE*>(m_pInstanceVertices);

	/*m_pVB->Lock(0, 0, (void**)&pVertex, 0);*/

	// 컨텍스트로부터 현재 인스턴스 버퍼의 정보를 받아옴. 이는 GPU도 셰이더를 위해 참조하는 것.
	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	VTXINSTANCE_PARTICLE* pVertices = static_cast<VTXINSTANCE_PARTICLE*>(SubResource.pData);

	// 단순히 인스턴스의 생성 위치에 따라, 중앙 위치의 반대 방향으로 이동하는 코드
	// 그 외의 세부사항은 셰이더 및 desc 값에 따라 조절
	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		// 단순 방사 방향 구함... : 현재 피벗(기준점) 대비 위치에 따라 방향을 정함.
		_vector	vMoveDir = XMVector3Normalize(XMVectorSetW(XMLoadFloat4(&pVertices[i].vTranslation) - XMLoadFloat3(&m_vPivot), 0.f));

		_matrix matDeltaRot = XMMatrixRotationAxis(m_pAxises[i], TO_RAD(m_pTurnSpeeds[i] * fTimeDelta));

		//if (m_isTurn)
		//{
		//	// 현재 인스턴스의 로컬 회전 행렬 구성
		//	_matrix matCurrent = XMMatrixSet(
		//		pVertices[i].vRight.x	, pVertices[i].vRight.y	, pVertices[i].vRight.z	, 0.f,
		//		pVertices[i].vUp.x		, pVertices[i].vUp.y	, pVertices[i].vUp.z	, 0.f,
		//		pVertices[i].vLook.x	, pVertices[i].vLook.y	, pVertices[i].vLook.z	, 0.f,
		//		0.f, 0.f, 0.f, 1.f
		//	);
		//}


		// 시간이 지남에 따라 움직임.
		XMStoreFloat4(&pVertices[i].vTranslation, XMLoadFloat4(&pVertices[i].vTranslation) + vMoveDir * m_pSpeeds[i] * fTimeDelta);
		pVertices[i].vLifeTime.x += fTimeDelta;

		if (true == m_isLoop)
		{
			if (pVertices[i].vLifeTime.x >= pVertices[i].vLifeTime.y)		// ElapsedTime 이 MaxTime 넘기면 다시 돌아가서 루프
			{
				pVertices[i].vLifeTime.x = 0.f;
				pVertices[i].vTranslation = pInstanceVertices[i].vTranslation;
			}
		}

	}

	m_pContext->Unmap(m_pVBInstance, 0);
}

void CVIBuffer_Rect_Instance::Trail(_float fTimeDelta, CGameObject* pOwner)
{
	D3D11_MAPPED_SUBRESOURCE	SubResource{};

	VTXINSTANCE_PARTICLE* pInstanceVertices = static_cast<VTXINSTANCE_PARTICLE*>(m_pInstanceVertices);

	/*m_pVB->Lock(0, 0, (void**)&pVertex, 0);*/

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	VTXINSTANCE_PARTICLE* pVertices = static_cast<VTXINSTANCE_PARTICLE*>(SubResource.pData);


	CTransform* pOwnerTransformCom = dynamic_cast<CTransform*>(pOwner->Get_Component(L"Com_Transform"));
	m_pGameInstance->Get_CamPosition();

	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		pInstanceVertices[i].vTranslation = _float4(
			m_pGameInstance->Rand(m_vCenter.x - m_vRange.x * 0.5f, m_vCenter.x + m_vRange.x * 0.5f),
			m_pGameInstance->Rand(m_vCenter.y - m_vRange.y * 0.5f, m_vCenter.y + m_vRange.y * 0.5f),
			m_pGameInstance->Rand(m_vCenter.z - m_vRange.z * 0.5f, m_vCenter.z),
			1.f
		);

		// 인스턴스 기준 월드 위치 계산
		_vector vWorldPos = XMLoadFloat4(&pVertices[i].vTranslation) + pOwnerTransformCom->Get_Position();

		// 카메라 방향 계산
		_matrix mOwnerWorldInv = pOwnerTransformCom->Get_WorldMatrix_Inverse();	// 

		_vector vLook = XMVector3Normalize(XMLoadFloat4(m_pGameInstance->Get_CamPosition()) - vWorldPos);
		_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
		_vector vRight = XMVector3Normalize(XMVector3Cross(vUp, vLook));
		vUp = XMVector3Cross(vLook, vRight);

		vLook = XMVector3TransformNormal(vLook, mOwnerWorldInv);
		vRight = XMVector3TransformNormal(vRight, mOwnerWorldInv);
		vUp = XMVector3TransformNormal(vUp, mOwnerWorldInv);

		vRight	= vRight	* m_pScales[i];
		vUp		= vUp		* m_pScales[i];
		vLook	= vLook		* m_pScales[i];

		XMStoreFloat4(&pVertices[i].vRight, vRight);
		XMStoreFloat4(&pVertices[i].vUp, vUp);
		XMStoreFloat4(&pVertices[i].vLook, vLook);



		_vector	vMoveDir = XMVector3Normalize(XMVectorSetW(XMLoadFloat4(&pVertices[i].vTranslation) - XMLoadFloat3(&m_vPivot), 0.f));

		XMStoreFloat4(&pVertices[i].vTranslation, XMLoadFloat4(&pVertices[i].vTranslation) + vMoveDir * m_pSpeeds[i] * fTimeDelta);
		pVertices[i].vLifeTime.x += fTimeDelta;

		if (true == m_isLoop)
		{
			if (pVertices[i].vLifeTime.x >= pVertices[i].vLifeTime.y)
			{
				pVertices[i].vLifeTime.x = 0.f;
				pVertices[i].vTranslation = _float4{ pVertices[i].vTranslation.x, pVertices[i].vTranslation.y, pInstanceVertices[i].vTranslation.z, 1.f };
			}
		}
	}

	m_pContext->Unmap(m_pVBInstance, 0);
}


void CVIBuffer_Rect_Instance::Drop(_float fTimeDelta)
{
}

CVIBuffer_Rect_Instance* CVIBuffer_Rect_Instance::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const INSTANCE_DESC* pDesc)
{
	CVIBuffer_Rect_Instance* pInstance = new CVIBuffer_Rect_Instance(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pDesc)))
	{
		MSG_BOX(TEXT("Failed to Created : CVIBuffer_Rect_Instance"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CVIBuffer_Rect_Instance::Clone(void* pArg)
{
	CVIBuffer_Rect_Instance* pInstance = new CVIBuffer_Rect_Instance(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CVIBuffer_Rect_Instance"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_Rect_Instance::Free()
{
	__super::Free();

	if (false == m_isCloned)
	{
		Safe_Delete_Array(m_pSpeeds);
		Safe_Delete_Array(m_pTurnSpeeds);
		Safe_Delete_Array(m_pAxises);
		Safe_Delete_Array(m_pScales);
	}
}

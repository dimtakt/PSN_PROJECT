#include "Mesh.h"
#include "Bone.h"
#include "Shader.h"

#include "Engine_Struct.h"

CMesh::CMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer { pDevice, pContext }
{
}

CMesh::CMesh(const CMesh& Prototype)
	: CVIBuffer{ Prototype }
{
}

HRESULT CMesh::Initialize_Prototype(MODELTYPE eType, const aiMesh* pAIMesh, const vector<class CBone*>& Bones, _fmatrix PreTransformMatrix)
{
	// m_pVB 에 버텍스 버퍼 할당

	m_iMaterialIndex = pAIMesh->mMaterialIndex;
	m_iNumVertices = pAIMesh->mNumVertices;					// 정점의 총 개수
	//m_iVertexStride = sizeof(VTXMESH);						// 정점의 용량
	m_iNumIndices = pAIMesh->mNumFaces * 3;					// 정점 인덱스의 개수
	m_iIndexStride = 4;										// 정점 인덱스의 용량
	m_iNumVertexBuffers = 1;								// 정점 버퍼 개수
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;					// * 정점 인덱스의 용량이 32비트임
	m_ePrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;	// * 면이 삼각형임

	HRESULT			hr = MODELTYPE::NONANIM == eType ?
		Ready_Vertices_For_NonAnim(pAIMesh, PreTransformMatrix) :
		Ready_Vertices_For_Anim(pAIMesh, Bones);

	if (FAILED(hr))
		return E_FAIL;


	// m_pIB 에 인덱스 버퍼 할당

	D3D11_BUFFER_DESC		IBDesc{};
	IBDesc.ByteWidth = m_iNumIndices * m_iIndexStride;
	IBDesc.Usage = D3D11_USAGE_DEFAULT;
	IBDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	IBDesc.CPUAccessFlags = 0;
	IBDesc.MiscFlags = 0;
	IBDesc.StructureByteStride = m_iIndexStride;

	_uint* pIndices = new _uint[m_iNumIndices];

	_uint	iNumIndices = {};

	for (size_t i = 0; i < pAIMesh->mNumFaces; i++)
	{
		aiFace AIFace = pAIMesh->mFaces[i];

		pIndices[iNumIndices++] = AIFace.mIndices[0];
		pIndices[iNumIndices++] = AIFace.mIndices[1];
		pIndices[iNumIndices++] = AIFace.mIndices[2];
	}

	D3D11_SUBRESOURCE_DATA	IBInitialData{};
	IBInitialData.pSysMem = pIndices;

	if (FAILED(m_pDevice->CreateBuffer(&IBDesc, &IBInitialData, &m_pIB)))
		return E_FAIL;

	// ===== m_pIndices 정보 삽입
	m_pIndices = new _uint[m_iNumIndices];
	memcpy(m_pIndices, pIndices, sizeof(_uint) * m_iNumIndices);
	// ============================== 

	Safe_Delete_Array(pIndices);

	return S_OK;
}

HRESULT CMesh::Initialize_Prototype_Binary(MODELTYPE eType, const MESH_DESC tMeshDesc, const vector<BONE_DESC>* vecBones, _fmatrix PreTransformMatrix)
{
	m_iMaterialIndex = tMeshDesc.iMaterialIndex;
	m_iNumVertices = tMeshDesc.iNumVertices;
	//m_iVertexStride = sizeof(VTXMESH);
	m_iNumIndices = tMeshDesc.iNumFaces * 3;					// 정점 인덱스의 개수
	m_iIndexStride = 4;										// 정점 인덱스의 용량
	m_iNumVertexBuffers = 1;								// 정점 버퍼 개수
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;					// * 정점 인덱스의 용량이 32비트임
	m_ePrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;	// * 면이 삼각형임

	HRESULT			hr = MODELTYPE::NONANIM == eType ?
		Ready_Vertices_For_NonAnim_Binary(tMeshDesc, PreTransformMatrix) :
		Ready_Vertices_For_Anim_Binary(tMeshDesc, vecBones);

	if (FAILED(hr))
		return E_FAIL;


	// m_pIB 에 인덱스 버퍼 할당

	D3D11_BUFFER_DESC		IBDesc{};
	IBDesc.ByteWidth = m_iNumIndices * m_iIndexStride;
	IBDesc.Usage = D3D11_USAGE_DEFAULT;
	IBDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	IBDesc.CPUAccessFlags = 0;
	IBDesc.MiscFlags = 0;
	IBDesc.StructureByteStride = m_iIndexStride;

	_uint* pIndices = new _uint[m_iNumIndices];

	_uint	iNumIndices = {};

	for (size_t i = 0; i < tMeshDesc.iNumFaces; i++)
	{
		MeshFace tFace = tMeshDesc.vecFaces[i];

		pIndices[iNumIndices++] = tFace.iIndices[0];
		pIndices[iNumIndices++] = tFace.iIndices[1];
		pIndices[iNumIndices++] = tFace.iIndices[2];
	}

	D3D11_SUBRESOURCE_DATA	IBInitialData{};
	IBInitialData.pSysMem = pIndices;

	if (FAILED(m_pDevice->CreateBuffer(&IBDesc, &IBInitialData, &m_pIB)))
		return E_FAIL;

	// ===== m_pIndices 정보 삽입
	m_pIndices = new _uint[m_iNumIndices];
	memcpy(m_pIndices, pIndices, sizeof(_uint) * m_iNumIndices);
	// ============================== 

	Safe_Delete_Array(pIndices);

	return S_OK;
}

HRESULT CMesh::Initialize(void* pArg)
{
	return S_OK;
}

HRESULT CMesh::Bind_BoneMatrices(CShader* pShader, const _char* pConstantName, const vector<class CBone*>& Bones)
{
	for (size_t i = 0; i < m_iNumBones; i++)
	{
		XMStoreFloat4x4(&m_BoneMatrices[i],
			XMLoadFloat4x4(&m_OffsetMatrices[i]) * Bones[m_BoneIndices[i]]->Get_CombinedTransformationMatrix());
	}

	return pShader->Bind_Matrices(pConstantName, m_BoneMatrices, m_iNumBones);
}


HRESULT CMesh::Ready_Vertices_For_NonAnim(const aiMesh* pAIMesh, _fmatrix PreTransformMatrix)
{
	m_iVertexStride = sizeof(VTXMESH);

	D3D11_BUFFER_DESC		VBDesc{};
	VBDesc.ByteWidth = m_iNumVertices * m_iVertexStride;
	VBDesc.Usage = D3D11_USAGE_DEFAULT;
	VBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	VBDesc.CPUAccessFlags = 0;
	VBDesc.MiscFlags = 0;
	VBDesc.StructureByteStride = m_iVertexStride;

	VTXMESH* pVertices = new VTXMESH[m_iNumVertices];

	for (size_t i = 0; i < m_iNumVertices; i++)
	{
		memcpy(&pVertices[i].vPosition, &pAIMesh->mVertices[i], sizeof(_float3));
		XMStoreFloat3(&pVertices[i].vPosition, XMVector3TransformCoord(XMLoadFloat3(&pVertices[i].vPosition), PreTransformMatrix));

		memcpy(&pVertices[i].vNormal, &pAIMesh->mNormals[i], sizeof(_float3));
		XMStoreFloat3(&pVertices[i].vNormal, XMVector3TransformNormal(XMLoadFloat3(&pVertices[i].vNormal), PreTransformMatrix));

		memcpy(&pVertices[i].vTangent, &pAIMesh->mTangents[i], sizeof(_float3));
		memcpy(&pVertices[i].vBinormal, &pAIMesh->mBitangents[i], sizeof(_float3));

		memcpy(&pVertices[i].vTexcoord, &pAIMesh->mTextureCoords[0][i], sizeof(_float2));
	}

	D3D11_SUBRESOURCE_DATA	VBInitialData{};
	VBInitialData.pSysMem = pVertices;

	if (FAILED(m_pDevice->CreateBuffer(&VBDesc, &VBInitialData, &m_pVB)))
		return E_FAIL;

	// ===== m_pVertexPositions 정보 삽입
	m_pVertexPositions = new _float3[m_iNumVertices];
	for (_uint i = 0; i < m_iNumVertices; ++i)
		m_pVertexPositions[i] = pVertices[i].vPosition;
	// ==============================

	Safe_Delete_Array(pVertices);

	return S_OK;
}

HRESULT CMesh::Ready_Vertices_For_Anim(const aiMesh* pAIMesh, const vector<CBone*>& Bones)
{
	m_iVertexStride = sizeof(VTXANIMMESH);

	D3D11_BUFFER_DESC		VBDesc{};
	VBDesc.ByteWidth = m_iNumVertices * m_iVertexStride;
	VBDesc.Usage = D3D11_USAGE_DEFAULT;
	VBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	VBDesc.CPUAccessFlags = 0;
	VBDesc.MiscFlags = 0;
	VBDesc.StructureByteStride = m_iVertexStride;

	VTXANIMMESH* pVertices = new VTXANIMMESH[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXANIMMESH) * m_iNumVertices);

	for (size_t i = 0; i < m_iNumVertices; i++)
	{
		memcpy(&pVertices[i].vPosition, &pAIMesh->mVertices[i], sizeof(_float3));
		memcpy(&pVertices[i].vNormal, &pAIMesh->mNormals[i], sizeof(_float3));
		memcpy(&pVertices[i].vTangent, &pAIMesh->mTangents[i], sizeof(_float3));
		memcpy(&pVertices[i].vBinormal, &pAIMesh->mBitangents[i], sizeof(_float3));
		memcpy(&pVertices[i].vTexcoord, &pAIMesh->mTextureCoords[0][i], sizeof(_float2));
	}

	m_iNumBones = pAIMesh->mNumBones;

	/*이 메시에 영향을 주는 뼈들을 하나씩 순회한다. */
	for (_uint i = 0; i < m_iNumBones; i++)
	{
		/* i번째 뼈가 영향을 주는 정점의 갯수 */
		aiBone* pAIBone = pAIMesh->mBones[i];

		_float4x4	OffsetMatrix;

		memcpy(&OffsetMatrix, &pAIBone->mOffsetMatrix, sizeof(_float4x4));

		XMStoreFloat4x4(&OffsetMatrix, XMMatrixTranspose(XMLoadFloat4x4(&OffsetMatrix)));

		m_OffsetMatrices.push_back(OffsetMatrix);

		_uint	iBoneIndex = { 0 };

		auto	iter = find_if(Bones.begin(), Bones.end(), [&](CBone* pBone)->_bool
			{
				if (true == pBone->Compare_Name(pAIBone->mName.data))
					return true;

				iBoneIndex++;

				return false;
			});

		m_BoneIndices.push_back(iBoneIndex);

		for (_uint j = 0; j < pAIBone->mNumWeights; j++)
		{
			aiVertexWeight	AIVertexWeight = pAIBone->mWeights[j];

			/* i번째 뼈가 영향을 주는 j번째 정점의 정점버퍼상의 인덱스 */
			if (0.f == pVertices[AIVertexWeight.mVertexId].vBlendWeight.x)
			{
				pVertices[AIVertexWeight.mVertexId].vBlendIndex.x = i;
				pVertices[AIVertexWeight.mVertexId].vBlendWeight.x = AIVertexWeight.mWeight;
			}
			else if (0.f == pVertices[AIVertexWeight.mVertexId].vBlendWeight.y)
			{
				pVertices[AIVertexWeight.mVertexId].vBlendIndex.y = i;
				pVertices[AIVertexWeight.mVertexId].vBlendWeight.y = AIVertexWeight.mWeight;
			}
			else if (0.f == pVertices[AIVertexWeight.mVertexId].vBlendWeight.z)
			{
				pVertices[AIVertexWeight.mVertexId].vBlendIndex.z = i;
				pVertices[AIVertexWeight.mVertexId].vBlendWeight.z = AIVertexWeight.mWeight;
			}
			else
			{
				pVertices[AIVertexWeight.mVertexId].vBlendIndex.w = i;
				pVertices[AIVertexWeight.mVertexId].vBlendWeight.w = AIVertexWeight.mWeight;
			}
		}
	}

	if (0 == m_iNumBones)
	{
		m_iNumBones = 1;

		_uint	iBoneIndex = { 0 };

		auto	iter = find_if(Bones.begin(), Bones.end(), [&](CBone* pBone)->_bool
			{
				if (true == pBone->Compare_Name(m_szName))
					return true;

				iBoneIndex++;

				return false;
			});

		m_BoneIndices.push_back(iBoneIndex);

		_float4x4		OffsetMatrix;
		XMStoreFloat4x4(&OffsetMatrix, XMMatrixIdentity());

		m_OffsetMatrices.push_back(OffsetMatrix);
	}

	D3D11_SUBRESOURCE_DATA	VBInitialData{};
	VBInitialData.pSysMem = pVertices;

	if (FAILED(m_pDevice->CreateBuffer(&VBDesc, &VBInitialData, &m_pVB)))
		return E_FAIL;

	// ===== m_pVertexPositions 정보 삽입
	m_pVertexPositions = new _float3[m_iNumVertices];
	for (_uint i = 0; i < m_iNumVertices; ++i)
		m_pVertexPositions[i] = pVertices[i].vPosition;
	// ==============================

	Safe_Delete_Array(pVertices);

	return S_OK;
}

HRESULT CMesh::Ready_Vertices_For_NonAnim_Binary(const MESH_DESC tMeshDesc, _fmatrix PreTransformMatrix)
{
	m_iVertexStride = sizeof(VTXMESH);

	D3D11_BUFFER_DESC		VBDesc{};
	VBDesc.ByteWidth = m_iNumVertices * m_iVertexStride;
	VBDesc.Usage = D3D11_USAGE_DEFAULT;
	VBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	VBDesc.CPUAccessFlags = 0;
	VBDesc.MiscFlags = 0;
	VBDesc.StructureByteStride = m_iVertexStride;

	VTXMESH* pVertices = new VTXMESH[m_iNumVertices];

	for (size_t i = 0; i < m_iNumVertices; i++)
	{
		memcpy(&pVertices[i].vPosition, &tMeshDesc.vecNonAnimVertices[i].vPosition, sizeof(_float3));
		XMStoreFloat3(&pVertices[i].vPosition, XMVector3TransformCoord(XMLoadFloat3(&pVertices[i].vPosition), PreTransformMatrix));

		memcpy(&pVertices[i].vNormal, &tMeshDesc.vecNonAnimVertices[i].vNormal, sizeof(_float3));
		XMStoreFloat3(&pVertices[i].vNormal, XMVector3TransformNormal(XMLoadFloat3(&pVertices[i].vNormal), PreTransformMatrix));

		memcpy(&pVertices[i].vTangent, &tMeshDesc.vecNonAnimVertices[i].vTangent, sizeof(_float3));
		memcpy(&pVertices[i].vBinormal, &tMeshDesc.vecNonAnimVertices[i].vBinormal, sizeof(_float3));

		memcpy(&pVertices[i].vTexcoord, &tMeshDesc.vecNonAnimVertices[i].vTexcoord, sizeof(_float2));
	}

	D3D11_SUBRESOURCE_DATA	VBInitialData{};
	VBInitialData.pSysMem = pVertices;

	if (FAILED(m_pDevice->CreateBuffer(&VBDesc, &VBInitialData, &m_pVB)))
		return E_FAIL;

	// ===== m_pVertexPositions 정보 삽입
	m_pVertexPositions = new _float3[m_iNumVertices];
	for (_uint i = 0; i < m_iNumVertices; ++i)
		m_pVertexPositions[i] = pVertices[i].vPosition;
	// ==============================

	Safe_Delete_Array(pVertices);

	return S_OK;
}

HRESULT CMesh::Ready_Vertices_For_Anim_Binary(const MESH_DESC tMeshDesc, const vector<BONE_DESC>* vecBones)
{

	// ksta : 8/1 14:38 작업중이었음. 원본 저장작업 우선 진행한 뒤 마저 진행하기

	/*

	어디까지 작업하던 중이었나?

	BinModel 에 Mesh Vertex 정보들 누락된 것 담는 것 완료
	해당 정보를 export 할 때 같이 빠지는 것 미완료
	import 할 때 Mesh.cpp 부분 Ready_Vertices_For_NonAnim_Binary 부분 진행하던 중이었음
	이후 유사 함수 애니메이션일 떄에 진행되도록 작업 및 나머지 import 작업 필요

	*/


	m_iVertexStride = sizeof(VTXANIMMESH);

	D3D11_BUFFER_DESC		VBDesc{};
	VBDesc.ByteWidth = m_iNumVertices * m_iVertexStride;
	VBDesc.Usage = D3D11_USAGE_DEFAULT;
	VBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	VBDesc.CPUAccessFlags = 0;
	VBDesc.MiscFlags = 0;
	VBDesc.StructureByteStride = m_iVertexStride;

	VTXANIMMESH* pVertices = new VTXANIMMESH[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXANIMMESH) * m_iNumVertices);

	for (size_t i = 0; i < m_iNumVertices; i++)
	{
		memcpy(&pVertices[i].vPosition, &tMeshDesc.vecAnimVertices[i].vPosition, sizeof(_float3));
		memcpy(&pVertices[i].vNormal, &tMeshDesc.vecAnimVertices[i].vNormal, sizeof(_float3));
		memcpy(&pVertices[i].vTangent, &tMeshDesc.vecAnimVertices[i].vTangent, sizeof(_float3));
		memcpy(&pVertices[i].vBinormal, &tMeshDesc.vecAnimVertices[i].vBinormal, sizeof(_float3));
		memcpy(&pVertices[i].vTexcoord, &tMeshDesc.vecAnimVertices[i].vTexcoord, sizeof(_float2));

		memcpy(&pVertices[i].vBlendIndex, &tMeshDesc.vecAnimVertices[i].vBlendIndex, sizeof(XMUINT4));
		memcpy(&pVertices[i].vBlendWeight, &tMeshDesc.vecAnimVertices[i].vBlendWeight, sizeof(_float4));
	}

	m_iNumBones = tMeshDesc.iNumUsingBones;

	for (_uint i = 0; i < m_iNumBones; i++)
	{
		_uint iBoneIndex = tMeshDesc.vecUsingBonesIndices[i];
		const BONE_DESC tBone = (*vecBones)[iBoneIndex];

		m_BoneIndices.push_back(iBoneIndex);
		m_OffsetMatrices.push_back(tBone.matOffset);
	}

	D3D11_SUBRESOURCE_DATA	VBInitialData{};
	VBInitialData.pSysMem = pVertices;

	if (FAILED(m_pDevice->CreateBuffer(&VBDesc, &VBInitialData, &m_pVB)))
		return E_FAIL;

	// ===== m_pVertexPositions 정보 삽입
	m_pVertexPositions = new _float3[m_iNumVertices];
	for (_uint i = 0; i < m_iNumVertices; ++i)
		m_pVertexPositions[i] = pVertices[i].vPosition;
	// ==============================

	Safe_Delete_Array(pVertices);

	return S_OK;
}


CMesh* CMesh::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, MODELTYPE eType, const aiMesh* pAIMesh, const vector<CBone*>& Bones, _fmatrix PreTransformMatrix)
{
	CMesh* pInstance = new CMesh(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(eType, pAIMesh, Bones, PreTransformMatrix)))
	{
		MSG_BOX(TEXT("Failed to Created : CMesh"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CMesh* CMesh::Create_Binary(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, MODELTYPE eType, const MESH_DESC tMeshDesc, const vector<BONE_DESC>* vecBones, _fmatrix PreTransformMatrix)
{
	CMesh* pInstance = new CMesh(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype_Binary(eType, tMeshDesc, vecBones, PreTransformMatrix)))
	{
		MSG_BOX(TEXT("Failed to Created : CMesh with Binary"));
		Safe_Release(pInstance);
	}

	return nullptr;
}

CComponent* CMesh::Clone(void* pArg)
{
	CMesh* pInstance = new CMesh(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CMesh"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMesh::Free()
{
	__super::Free();


}

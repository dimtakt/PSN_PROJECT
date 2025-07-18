#pragma once

#include "VIBuffer.h"

NS_BEGIN(Engine)

class ENGINE_DLL CMesh final : public CVIBuffer
{
private:
	CMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMesh(const CMesh& Prototype);
	virtual ~CMesh() = default;

public:
	_uint Get_MaterialIndex() const {
		return m_iMaterialIndex;
	}

public:
	virtual HRESULT Initialize_Prototype(MODELTYPE eType, const aiMesh* pAIMesh, _fmatrix PreTransformMatrix);
	virtual HRESULT Initialize(void* pArg) override;

private:
	_uint			m_iMaterialIndex = {};
	_uint			m_iNumBones = {};
	/* 이 메시에 영향을 주는 뼈들의 인덱스(전체뼈기준) 집합. */
	vector<_int>	m_BoneIndices;

private:
	HRESULT Ready_Vertices_For_NonAnim(const aiMesh* pAIMesh, _fmatrix PreTransformMatrix);
	HRESULT Ready_Vertices_For_Anim(const aiMesh* pAIMesh);

public:
	static CMesh* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, MODELTYPE eType, const aiMesh* pAIMesh, _fmatrix PreTransformMatrix);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END
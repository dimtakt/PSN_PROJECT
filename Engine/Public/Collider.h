#pragma once

#include "Component.h"

#include "Bounding_AABB.h"
#include "Bounding_OBB.h"
#include "Bounding_Sphere.h"

NS_BEGIN(Engine)

class ENGINE_DLL CCollider final : public CComponent
{
private:
	CCollider(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCollider(const CCollider& Prototype);
	virtual ~CCollider() = default;
public:
	virtual HRESULT Initialize_Prototype(COLLIDERTYPE eType);
	virtual HRESULT Initialize(void* pArg) override;
	void Update(_fmatrix WorldMatrix);

public:
	_bool Intersect(CCollider* pTarget);
	_bool Intersect_Ray(_vector vRayPos, _vector vRayDir, _float& fOutDist);

	COLLISION_DESC Get_ColDesc() { return m_tColDesc; };
	void Set_ColDesc(COLLISION_DESC desc) { m_tColDesc = desc; };

	void Set_isActive(_bool isActive)	{ m_tColDesc.isActive = isActive; }
	void Set_LayerIndex(_uint iLayer)	{ m_tColDesc.iLayerIndex = iLayer; }
	void Set_LayerMask(_uint iMask)		{ m_tColDesc.iMask = iMask; }


#ifdef _DEBUG
	virtual HRESULT Render() override;

#endif


private:
	COLLIDERTYPE			m_eType = { COLLIDERTYPE::END };
	class CBounding*		m_pBounding = { nullptr };
	_bool					m_isColl = { false };

	COLLISION_DESC			m_tColDesc = {};

#ifdef _DEBUG
private:
	PrimitiveBatch<VertexPositionColor>*		m_pBatch = { nullptr };
	BasicEffect*								m_pEffect = { nullptr };
	ID3D11InputLayout*							m_pInputLayout = { nullptr };
#endif

public:
	static CCollider* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, COLLIDERTYPE eType);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;

};

NS_END
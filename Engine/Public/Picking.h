#pragma once

#include "Base.h"

NS_BEGIN(Engine)

class CPicking final : public CBase
{
private:
	CPicking(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CPicking() = default;

public:
	HRESULT Initialize(HWND hWnd, _uint iWinSizeX, _uint iWinSizeY);

	/* 마우스의 윈도우상의 위치를 구하고 */
	/* 뷰스페이스 상에서 마우스 레이로 변형하여 */
	/* 월드까지 변환시켜서 보관하자. */
	void Update();

	_bool Picking_InWorld(_float3& vPickedPos, const _float3& vPointA, const _float3& vPointB, const _float3& vPointC);
	_bool Picking_InLocal(_float3& vPickedPos, const _float3& vPointA, const _float3& vPointB, const _float3& vPointC);

	//_bool Get_IntersectAtX(_float targetY, _float3& vIntersectPos);
	//_bool Get_IntersectAtY(_float targetY, _float3& vIntersectPos);
	//_bool Get_IntersectAtZ(_float targetY, _float3& vIntersectPos);

	void Transform_ToLocalSpace(const _float4x4& WorldMatrixInverse);



private:
	ID3D11Device*			m_pDevice		= { nullptr };
	ID3D11DeviceContext*	m_pContext		= { nullptr };

	class CGameInstance*	m_pGameInstance = { nullptr };

private:
	HWND			m_hWnd = {};
	_uint			m_iWinSizeX{}, m_iWinSizeY{};

	_float3         m_vMouseRay = {};
	_float3         m_vMousePos = {};

	_float3         m_vLocalMouseRay = {};
	_float3         m_vLocalMousePos = {};

public:
	static CPicking* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, HWND hWnd, _uint iWinSizeX, _uint iWinSizeY);
	virtual void Free() override;
};

NS_END
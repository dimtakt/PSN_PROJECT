#pragma once

#include "Prototype_Manager.h"

NS_BEGIN(Engine)

class ENGINE_DLL CGameInstance final : public CBase
{
	DECLARE_SINGLETON(CGameInstance)
private:
	CGameInstance();
	virtual ~CGameInstance() = default;



	// ==============================

#pragma region ENGINE
public:
	HRESULT Initialize_Engine(const ENGINE_DESC& EngineDesc, ID3D11Device** ppDevice, ID3D11DeviceContext** ppContext);
	void Update_Engine(_float fTimeDelta);
	HRESULT Clear_Resources(_uint iClearLevelID);
public:
	void Render_Begin(const _float4* pClearColor);
	HRESULT Draw();
	void Render_End(HWND hWnd = 0);
public:
	_float Rand_Normal();
	_float Rand(_float fMin, _float fMax);

#pragma endregion

	// ==============================

#pragma region LEVEL_MANAGER
public:
	HRESULT Open_Level(_uint iLevelID, class CLevel* pNewLevel);
#pragma endregion

	// ==============================

#pragma region PROTOTYPE_MANAGER
public:
	HRESULT Add_Prototype(_uint iPrototpyeLevelIndex, const _wstring& strPrototypeTag, class CBase* pPrototype);
	class CBase* Clone_Prototype(PROTOTYPE ePrototype, _uint iPrototpyeLevelIndex, const _wstring& strPrototypeTag, void* pArg = nullptr);
#pragma endregion

	// ==============================

#pragma region OBJECT_MANAGER
public:
	class CComponent* Find_Component(_uint iLayerLevelIndex, const _wstring& strLayerTag, const _wstring& strComponentTag, _uint iIndex = 0);
	HRESULT Add_GameObject_ToLayer(_uint iLayerLevelIndex, const _wstring& strLayerTag, _uint iPrototypeLevelIndex, const _wstring& strPrototypeTag, void* pArg = nullptr);
#pragma endregion

	// ==============================

#pragma region RENDERER
public:
	HRESULT Add_RenderGroup(RENDERGROUP eRenderGroup, class CGameObject* pRenderObject);
#pragma endregion

	// ==============================

#pragma region TIMER_MANAGER
public:
	_float	Get_TimeDelta(const _wstring& strTimerTag);
	HRESULT	Add_Timer(const _wstring& strTimerTag);
	void	Compute_TimeDelta(const _wstring& strTimerTag);
#pragma 

	// ==============================

#pragma region FONT_MANAGER
public:
	// (1:key값, 2:value값) 폰트를 추가합니다.
	HRESULT	Add_Font(const _wstring& strFontTag, const _tchar* pFontPath);
	// (1:key값, 2:출력텍스트, 3:출력위치; 4:출력색상, 5:회전, 6:크기및회전기준, 7:크기) 폰트를 출력합니다.
	HRESULT Render_Font(
		const _wstring& strFontTag,							// map에 넣은 key값
		const _tchar* pText,								// 출력할 텍스트
		const _float2& vPosition,							// 출력할 위치
		_fvector vColor = XMVectorSet(1.f, 1.f, 1.f, 1.f),	// 출력할 색상
		_float fRotation = 0.f,								// 회전 조정
		const _float2& vOrigin = _float2(0.f, 0.f),			// 회전 및 크기조정 기준
		_float fScale = 1.f									// 크기 조정
	);

#pragma endregion

	// ==============================

#pragma region PIPELINE
public:
	_matrix Get_Transform_Matrix(D3DTS eTransformState) const;
	const _float4x4* Get_Transform_Float4x4(D3DTS eTransformState) const;
	_matrix Get_Transform_Matrix_Inverse(D3DTS eTransformState) const;
	const _float4x4* Get_Transform_Float4x4_Inverse(D3DTS eTransformState) const;
	const _float4* Get_CamPosition() const;
	void Set_Transform(D3DTS eTransformState, _fmatrix Matrix);
	void Set_Transform(D3DTS eTransformState, const _float4x4& Matrix);
#pragma endregion

	// ==============================

#pragma region DX9 Legacy

	//
	//#pragma region PICKING 
	//	void Transform_Picking_ToLocalSpace(class CTransform* pTransformCom);
	//	_bool isPicked_InLocalSpace(const _float3& vPointA, const _float3& vPointB, const _float3& vPointC, _float3* pOut);
	//#pragma endregion


#pragma endregion

	// ==============================



private:
	class CGraphic_Device*		m_pGraphic_Device = { nullptr };
	class CLevel_Manager*		m_pLevel_Manager = { nullptr };
	class CObject_Manager*		m_pObject_Manager = { nullptr };
	class CPrototype_Manager*	m_pPrototype_Manager = { nullptr };
	class CRenderer*			m_pRenderer = { nullptr };
	class CTimer_Manager*		m_pTimer_Manager = { nullptr };
	class CFont_Manager*		m_pFont_Manager = { nullptr };
	class CPipeLine*			m_pPipeLine = { nullptr };
	class CPicking*				m_pPicking = { nullptr };

public:
	void Release_Engine();
	virtual void Free() override;
};

NS_END
#pragma once

#include "Prototype_Manager.h"

NS_BEGIN(Engine)
class CTransform;

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
	_uint Get_CurLevel();
	_uint Get_DestLevel();
	void Set_DestLevel(_uint iDestLevelIndex);
#pragma endregion

	// ==============================

#pragma region PROTOTYPE_MANAGER
public:
	HRESULT Add_Prototype(_uint iPrototypeLevelIndex, const _wstring& strPrototypeTag, class CBase* pPrototype);
	HRESULT Remove_Prototype(_uint iPrototypeLevelIndex, const _wstring& strPrototypeTag);
	CBase* Find_Prototype(_uint iPrototypeLevelIndex, const _wstring& strPrototypeTag);
	class CBase* Clone_Prototype(PROTOTYPE ePrototype, _uint iPrototypeLevelIndex, const _wstring& strPrototypeTag, void* pArg = nullptr);
#pragma endregion

	// ==============================

#pragma region OBJECT_MANAGER
public:
	class CComponent* Find_Component(_uint iLayerLevelIndex, const _wstring& strLayerTag, const _wstring& strComponentTag, _uint iIndex = 0);
	class CGameObject* Find_GameObject(_uint iLayerLevelIndex, const _wstring& strLayerTag, _uint iIndex = 0);
	HRESULT Add_GameObject_ToLayer(_uint iLayerLevelIndex, const _wstring& strLayerTag, _uint iPrototypeLevelIndex, const _wstring& strPrototypeTag, void* pArg = nullptr);
	HRESULT Remove_GameObject_FromLayer(_uint iLayerLevelIndex, const _wstring& strLayerTag, class CGameObject* pObject);
	class CGameObject* Get_LastGameObject(_uint iLayerLevelIndex, const _wstring& strLayerTag);
#pragma endregion

	// ==============================

#pragma region RENDERER
public:
	HRESULT Add_RenderGroup(RENDERGROUP eRenderGroup, class CGameObject* pRenderObject);
#ifdef _DEBUG
	HRESULT Add_DebugComponent(class CComponent* pComponent);
#endif
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
	// 폰트를 추가합니다. (1:key값, 2:경로 값) 
	HRESULT	Add_Font(const _wstring& strFontTag, const _tchar* pFontPath);
	// 폰트를 출력합니다. (1:key값, 2:출력텍스트, 3:출력위치; 4:출력색상, 5:회전, 6:크기및회전기준, 7:크기)
	// 함수 내에 Render_Start / End 포함
	HRESULT Render_Font_Immediately(
		const _wstring& strFontTag,							// map에 넣은 key값
		const _tchar* pText,								// 출력할 텍스트
		const _float2& vPosition,							// 출력할 위치
		_fvector vColor = XMVectorSet(1.f, 1.f, 1.f, 1.f),	// 출력할 색상
		_float fRotation = 0.f,								// 회전 조정
		const _float2& vOrigin = _float2(0.f, 0.f),			// 회전 및 크기조정 기준
		_float fScale = 1.f									// 크기 조정
	);
	// 폰트를 출력합니다. (1:key값, 2:출력텍스트, 3:출력위치; 4:출력색상, 5:회전, 6:크기및회전기준, 7:크기)
	// 함수 내에 Render_Begin / End 미포함. 호출 필요
	HRESULT Render_Font(
		const _wstring& strFontTag,							// map에 넣은 key값
		const _tchar* pText,								// 출력할 텍스트
		const _float2& vPosition,							// 출력할 위치
		_fvector vColor = XMVectorSet(1.f, 1.f, 1.f, 1.f),	// 출력할 색상
		_float fRotation = 0.f,								// 회전 조정
		const _float2& vOrigin = _float2(0.f, 0.f),			// 회전 및 크기조정 기준
		_float fScale = 1.f									// 크기 조정
	);
	HRESULT Render_Font_Begin(const _wstring& strFontTag);
	HRESULT Render_Font_End(const _wstring& strFontTag);

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

#pragma region INPUT_DEVICE
public:
	_byte	Get_DIKeyState(_ubyte byKeyID);				// ┌ 0 = 안눌림, 
	_byte	Get_DIMouseState(MOUSEKEYSTATE eMouse);		// └ 0x80 = 눌림.
	// 인자(X, Y, WHEEL)에 해당하는 마우스 움직임 크기를 반환. 휠은 올릴 시 양수반환
	_long	Get_DIMouseMove(MOUSEMOVESTATE eMouseState);	

	// Key_Manager와 유사하게..
public:
	_bool	Get_IsKeyDown(_ubyte byKeyID);
	_bool	Get_IsKeyDown(MOUSEKEYSTATE eMouse);

	_bool	Get_IsKeyUp(_ubyte byKeyID);
	_bool	Get_IsKeyUp(MOUSEKEYSTATE eMouse);

	_bool	Get_IsKeyPressing(_ubyte byKeyID);
	_bool	Get_IsKeyPressing(MOUSEKEYSTATE eMouse);
#pragma endregion

	// ==============================

#pragma region LIGHT_MANAGER
public:
	const LIGHT_DESC* Get_LightDesc(_uint iIndex) const;
	HRESULT Add_Light(const LIGHT_DESC& LightDesc);
	HRESULT Render_Lights(class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);

#pragma endregion

	// ==============================	

#pragma region PICKING : DX9 Legacy
public:
	void Transform_Picking_ToLocalSpace(CTransform* pTransformCom);
	_bool Picking_InWorld(_float3& vPickedPos, const _float3& vPointA, const _float3& vPointB, const _float3& vPointC);
	_bool Picking_InLocal(_float3& vPickedPos, const _float3& vPointA, const _float3& vPointB, const _float3& vPointC);

#pragma endregion

	// ==============================

#pragma region TARGET_MANAGER
	HRESULT Add_RenderTarget(const _wstring& strTargetTag, _uint iSizeX, _uint iSizeY, DXGI_FORMAT ePixelFormat, const _float4& vClearColor);
	HRESULT Add_MRT(const _wstring& strMRTTag, const _wstring& strTargetTag);
	HRESULT Begin_MRT(const _wstring& strMRTTag, ID3D11DepthStencilView* pDSV = nullptr);
	HRESULT End_MRT();
	HRESULT Bind_RT_ShaderResource(const _wstring& strTargetTag, class CShader* pShader, const _char* pConstantName);
	HRESULT Copy_RT_Resource(const _wstring& strTargetTag, ID3D11Texture2D* pSourTexture);

#ifdef _DEBUG
	HRESULT Ready_RT_Debug(const _wstring& strTargetTag, _float fX, _float fY, _float fSizeX, _float fSizeY);
	HRESULT Render_RT_Debug(class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);
#endif
#pragma endregion

	// ==============================

#pragma region SHADOW
	const _float4x4* Get_ShadowLight_Transform_Float4x4(D3DTS eTransformState) const;
	HRESULT Ready_ShadowLight(SHADOW_LIGHT_DESC LightDesc);
#pragma endregion

	// ==============================

#pragma region TIMESPEED_MANAGER

public:
	void Req_EditTimeSpeed(_float fEditValue, _bool isForceChange = false);
	_float Get_TimeSpeed();

#pragma endregion

	// ==============================

#pragma region COLLISION_MANAGER

public:
	HRESULT Add_Collider(CCollider* pCollider);
	// 콜라이더 매니저 내에 보관된 주소를 제거합니다.
	HRESULT Remove_Collider(CCollider* pCollider);	
	void Update_Collision();
	// 메뉴얼로 레이 정보를 전달하여 레이와 현존하는 콜라이더들과의 겹칩 검사를 진행합니다.
	_bool Check_RayCollisions(RAYCOLLISION_DESC* pRayDesc, CGameObject*& OutIntersectObj, _float& fOutDistance);

#pragma endregion
	// ==============================



private:
	class CGraphic_Device*		m_pGraphic_Device = { nullptr };
	class CInput_Device*		m_pInput_Device = { nullptr };
	class CLevel_Manager*		m_pLevel_Manager = { nullptr };
	class CObject_Manager*		m_pObject_Manager = { nullptr };
	class CPrototype_Manager*	m_pPrototype_Manager = { nullptr };
	class CRenderer*			m_pRenderer = { nullptr };
	class CTimer_Manager*		m_pTimer_Manager = { nullptr };
	class CFont_Manager*		m_pFont_Manager = { nullptr };
	class CPipeLine*			m_pPipeLine = { nullptr };
	class CPicking*				m_pPicking = { nullptr };
	class CLight_Manager*		m_pLight_Manager = { nullptr };
	class CTarget_Manager*		m_pTarget_Manager = { nullptr };
	class CShadow*				m_pShadow = { nullptr };

	class CTimeSpeed_Manager*	m_pTimeSpeed_Manager = { nullptr };
	class CCollision_Manager*	m_pCollision_Manager = { nullptr };

public:
	void Release_Engine();
	virtual void Free() override;
};

NS_END
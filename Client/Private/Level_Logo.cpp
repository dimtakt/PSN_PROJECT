#include "Level_Logo.h"

#include "GameInstance.h"

#include "Level_Loading.h"

CLevel_Logo::CLevel_Logo(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel { pDevice, pContext }
{
}

HRESULT CLevel_Logo::Initialize()
{

	/* 현재 레벨을 구성해주기 위한 객체들을 생성한다. */
	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
		return E_FAIL;

	
	return S_OK;
}

void CLevel_Logo::Update(_float fTimeDelta)
{
	// 엔터 시 GAMEPLAY 레벨로 
	
	if (m_pGameInstance->Get_IsKeyDown(DIK_RETURN))
	{
		if (FAILED(m_pGameInstance->Open_Level(static_cast<_uint>(LEVEL::LOADING), CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL::GAMEPLAY))))
			return;
		return;
	}
	else if (m_pGameInstance->Get_IsKeyDown(DIK_E))
	{
		if (FAILED(m_pGameInstance->Open_Level(static_cast<_uint>(LEVEL::LOADING), CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL::EDITOR))))
			return;
		return;
	}

	return;
}

HRESULT CLevel_Logo::Render()
{
	SetWindowText(g_hWnd, TEXT("Level : Logo"));

	// ksta : 폰트 그리기 테스트.
	// 가로 58, 세로 25

	// 수평/수직 선: ─ │
	// 모서리: ┌ ┐ └ ┘
	// 분기/교차: ├ ┤ ┬ ┴ ┼
	// 굵은 선 느낌: ━ ┃ ┏ ┓ ┗ ┛ ┣ ┫ ┳ ┻ ╋
	// 이중선:	═ ║ ╔ ╗ ╝ ╚ ╬

	_wstring strFontTag = L"Font_DOS";
	_uint iRenderStartX = 100.f;
	_uint iRenderStartY = 25.f;
	_uint iRenderSpaceY = 25.f;

	_vector vLoadColor = XMVectorSet(1.f, 1.f, 1.f, 1.f);

	m_pGameInstance->Render_Font_Begin(strFontTag);
	m_pGameInstance->Render_Font(strFontTag, L"┌────────────┬────────┬─────────────────────────────────┐", _float2(iRenderStartX, iRenderStartY += iRenderSpaceY), vLoadColor);
	m_pGameInstance->Render_Font(strFontTag, L"│superhot.exe│--FILE->│app: superhot.exe│--FILE-> 313   │", _float2(iRenderStartX, iRenderStartY += iRenderSpaceY), vLoadColor);
	m_pGameInstance->Render_Font(strFontTag, L"│editor.exe  │--FILE->│                                 │", _float2(iRenderStartX, iRenderStartY += iRenderSpaceY), vLoadColor);
	m_pGameInstance->Render_Font(strFontTag, L"│quit.exe    │--FILE->│                                 │", _float2(iRenderStartX, iRenderStartY += iRenderSpaceY), vLoadColor);
	m_pGameInstance->Render_Font(strFontTag, L"│------------│--------│                                 │", _float2(iRenderStartX, iRenderStartY += iRenderSpaceY), vLoadColor);

	m_pGameInstance->Render_Font(strFontTag, L"│            │        │                                 │", _float2(iRenderStartX, iRenderStartY += iRenderSpaceY), vLoadColor);
	m_pGameInstance->Render_Font(strFontTag, L"│            │        │                                 │", _float2(iRenderStartX, iRenderStartY += iRenderSpaceY), vLoadColor);
	m_pGameInstance->Render_Font(strFontTag, L"│            │        │                                 │", _float2(iRenderStartX, iRenderStartY += iRenderSpaceY), vLoadColor);
	m_pGameInstance->Render_Font(strFontTag, L"│            │        │                                 │", _float2(iRenderStartX, iRenderStartY += iRenderSpaceY), vLoadColor);
	m_pGameInstance->Render_Font(strFontTag, L"│            │        │                                 │", _float2(iRenderStartX, iRenderStartY += iRenderSpaceY), vLoadColor);

	m_pGameInstance->Render_Font(strFontTag, L"│            │        │                                 │", _float2(iRenderStartX, iRenderStartY += iRenderSpaceY), vLoadColor);
	m_pGameInstance->Render_Font(strFontTag, L"│            │        │                                 │", _float2(iRenderStartX, iRenderStartY += iRenderSpaceY), vLoadColor);
	m_pGameInstance->Render_Font(strFontTag, L"│            │        │                                 │", _float2(iRenderStartX, iRenderStartY += iRenderSpaceY), vLoadColor);
	m_pGameInstance->Render_Font(strFontTag, L"│            │        │                                 │", _float2(iRenderStartX, iRenderStartY += iRenderSpaceY), vLoadColor);
	m_pGameInstance->Render_Font(strFontTag, L"│            │        │                                 │", _float2(iRenderStartX, iRenderStartY += iRenderSpaceY), vLoadColor);

	m_pGameInstance->Render_Font(strFontTag, L"│            │        │                                 │", _float2(iRenderStartX, iRenderStartY += iRenderSpaceY), vLoadColor);
	m_pGameInstance->Render_Font(strFontTag, L"│            │        │                                 │", _float2(iRenderStartX, iRenderStartY += iRenderSpaceY), vLoadColor);
	m_pGameInstance->Render_Font(strFontTag, L"│            │        │                                 │", _float2(iRenderStartX, iRenderStartY += iRenderSpaceY), vLoadColor);
	m_pGameInstance->Render_Font(strFontTag, L"│            │        │                                 │", _float2(iRenderStartX, iRenderStartY += iRenderSpaceY), vLoadColor);
	m_pGameInstance->Render_Font(strFontTag, L"│            │        │                                 │", _float2(iRenderStartX, iRenderStartY += iRenderSpaceY), vLoadColor);

	m_pGameInstance->Render_Font(strFontTag, L"│            │        │                                 │", _float2(iRenderStartX, iRenderStartY += iRenderSpaceY), vLoadColor);
	m_pGameInstance->Render_Font(strFontTag, L"│            │        │                                 │", _float2(iRenderStartX, iRenderStartY += iRenderSpaceY), vLoadColor);
	m_pGameInstance->Render_Font(strFontTag, L"├────────────┴────────┴─────────────────────────────────┤", _float2(iRenderStartX, iRenderStartY += iRenderSpaceY), vLoadColor);
	m_pGameInstance->Render_Font(strFontTag, L"│C:\\                                                    │", _float2(iRenderStartX, iRenderStartY += iRenderSpaceY), vLoadColor);
	m_pGameInstance->Render_Font(strFontTag, L"└────────────────────────────────────Omni─piOS-v2.1.01p─┘", _float2(iRenderStartX, iRenderStartY += iRenderSpaceY), vLoadColor);


	m_pGameInstance->Render_Font_End(strFontTag);
	
	
	return S_OK;
}

HRESULT CLevel_Logo::Render_Focus_GamePlay()
{
	return E_NOTIMPL;
}

HRESULT CLevel_Logo::Render_Focus_Editor()
{
	return E_NOTIMPL;
}

HRESULT CLevel_Logo::Render_Focus_Quit()
{
	return E_NOTIMPL;
}

HRESULT CLevel_Logo::Ready_Layer_BackGround(const _wstring& strLayerTag)
{
	// CBackGround 프로토타입으로부터 Create 함
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::LOGO), strLayerTag,
		ENUM_CLASS(LEVEL::LOGO), TEXT("Prototype_GameObject_BackGround"))))
		return E_FAIL;

	return S_OK;
}


CLevel_Logo* CLevel_Logo::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_Logo* pInstance = new CLevel_Logo(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CLevel_Logo"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CLevel_Logo::Free()
{
	__super::Free();



}

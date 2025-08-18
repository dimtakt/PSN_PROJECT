#include "Level_Logo.h"

#include "GameInstance.h"

#include "Level_Loading.h"

#include "GameObject.h"

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

	if		(m_pGameInstance->Get_IsKeyDown(DIK_S))
	{
		if (m_iMenuFocusingIndex < ENUM_CLASS(LOGO_INDEX_MAIN::MAIN_END) - 1)
			m_iMenuFocusingIndex++;
	}
	else if (m_pGameInstance->Get_IsKeyDown(DIK_W))
	{
		if (m_iMenuFocusingIndex > 0)
			m_iMenuFocusingIndex--;
	}
	else if (m_pGameInstance->Get_IsKeyDown(DIK_RETURN))
	{
		switch (m_iMenuFocusingIndex)
		{
		case ENUM_CLASS(LOGO_INDEX_MAIN::MAIN_GAMEPLAY):
			if (FAILED(m_pGameInstance->Open_Level(static_cast<_uint>(LEVEL::LOADING), CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL::GAMEPLAY))))
				return;
			break;
		case ENUM_CLASS(LOGO_INDEX_MAIN::MAIN_EDITOR):
			if (FAILED(m_pGameInstance->Open_Level(static_cast<_uint>(LEVEL::LOADING), CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL::EDITOR))))
				return;
			break;
		case ENUM_CLASS(LOGO_INDEX_MAIN::MAIN_QUIT):
			PostQuitMessage(0);
			break;
		default:
			break;
		}
	}




	return;
}

HRESULT CLevel_Logo::Render()
{
	SetWindowText(g_hWnd, TEXT("Level : Logo"));

	Render_LogoScreen();
	
	return S_OK;
}

HRESULT CLevel_Logo::Render_LogoScreen()
{
	// 가로 58, 세로 25

	// 수평/수직 선: ─ │
	// 모서리: ┌ ┐ └ ┘
	// 분기/교차: ├ ┤ ┬ ┴ ┼
	// 굵은 선 느낌: ━ ┃ ┏ ┓ ┗ ┛ ┣ ┫ ┳ ┻ ╋
	// 이중선:	═ ║ ╔ ╗ ╝ ╚ ╬
	// 밝기 : █ ▓ ▒ ░ 

	_wstring strFontTag = L"Font_DOS";
	_uint iRenderStartX = 100.f;
	_uint iRenderStartY = 25.f;
	_uint iRenderSpaceY = 25.f;

	_vector vLoadColor = XMVectorSet(1.f, 1.f, 1.f, 1.f);
	_vector vLoadFocusedColor = XMVectorSet(.7f, 0.f, 0.f, 1.f);
	_vector vLoadUnFocusedColor = XMVectorSet(.6, .6, .6, 1.f);

	static _uint iStackedFrame = 0;
	static _uint iTimeFlick = 0;
	iStackedFrame++;

	if (iStackedFrame >= 30)
	{
		iStackedFrame = 0;
		iTimeFlick = ++iTimeFlick % 2;
	}




	m_pGameInstance->Render_Font_Begin(strFontTag);

	/* Selected Point*/
	m_pGameInstance->Render_Font(strFontTag, L" █████████████████████", _float2(iRenderStartX, iRenderStartY + iRenderSpaceY * (2 + m_iMenuFocusingIndex)), vLoadFocusedColor);


	/* Desc Text (app: superhot.exe 이부분) */
	_wstring strDescText = {};
	switch (m_iMenuFocusingIndex)
	{
	case ENUM_CLASS(LOGO_INDEX_MAIN::MAIN_GAMEPLAY):
		strDescText = L"                       app: superhot.exe  │--FILE-> 313";
		break;
	case ENUM_CLASS(LOGO_INDEX_MAIN::MAIN_EDITOR):
		strDescText = L"                       app: editor.exe    │--FILE-> 153";
		break;
	case ENUM_CLASS(LOGO_INDEX_MAIN::MAIN_QUIT):
		strDescText = L"                       app: quit.exe      │--FILE-> 1471";
		break;

	default:
		break;
	}

	/* Time */
	SYSTEMTIME st;
	GetLocalTime(&st);

	_tchar szTime[58];
	if (iTimeFlick == 0)	swprintf_s(szTime, L"                                                 %02d:%02d", st.wHour, st.wMinute);
	else					swprintf_s(szTime, L"                                                 %02d %02d", st.wHour, st.wMinute);

	m_pGameInstance->Render_Font(strFontTag, szTime, _float2(iRenderStartX, iRenderStartY + iRenderSpaceY), vLoadColor);


	m_pGameInstance->Render_Font(strFontTag, strDescText.c_str(), _float2(iRenderStartX, iRenderStartY + iRenderSpaceY * (2)), vLoadUnFocusedColor);


	/* Text UI */
	m_pGameInstance->Render_Font(strFontTag, L"┌────────────┬────────┬──────────────────────────     ──┐", _float2(iRenderStartX, iRenderStartY += iRenderSpaceY), vLoadColor);
	m_pGameInstance->Render_Font(strFontTag, L"│superhot.exe│--FILE->│                                 │", _float2(iRenderStartX, iRenderStartY += iRenderSpaceY), vLoadColor);
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

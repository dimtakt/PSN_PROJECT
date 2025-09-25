#include "Level_Loading.h"

#include "Loader.h"
#include "GameInstance.h"

#include "Level_Logo.h"

//#ifdef _DEBUG
#include "Level_Editor.h"
//#endif

#include "Level_GamePlay.h"

#include "Stage_Test1.h"

#include "Stage_01Kick.h"
#include "Stage_09FightC.h"
#include "Stage_10Desper.h"


#include "BackGround.h"

CLevel_Loading::CLevel_Loading(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel { pDevice, pContext }
{
}

HRESULT CLevel_Loading::Initialize(LEVEL eNextLevelID)
{
	m_eNextLevelID = eNextLevelID;	 
	Update_DestLevel(m_eNextLevelID);	// Level매니저에서 다음 레벨을 갱신. 프로토타입/객체 생성시에 해당 변수를 이용할 것.

	/* 현재 레벨을 구성해주기 위한 객체들을 생성한다. */
	if (FAILED(Ready_GameObjects()))
		return E_FAIL;

	/* 다음 레벨을 위한 로딩작업을 시작 한다. */
	if (FAILED(Ready_LoadingThread()))
		return E_FAIL;

	if (m_eNextLevelID != LEVEL::LOGO)
		m_pGameInstance->PlaySoundFixed(L"R_terminal_newline_1.ogg", ENUM_CLASS(SOUNDCH::SOUND_MAINUI));
	
	return S_OK;
}

void CLevel_Loading::Update(_float fTimeDelta)
{
	if (m_pLoader->isFinished() && 
		(GetKeyState(VK_SPACE) & 0x8000 || GetKeyState(VK_RETURN) & 0x8000 || m_eNextLevelID == LEVEL::LOGO))
	{
		CLevel* pNewLevel = { nullptr };

		switch (m_eNextLevelID)
		{
		case LEVEL::LOGO:
			pNewLevel = CLevel_Logo::Create(m_pDevice, m_pContext);
			break;
		case LEVEL::EDITOR:
//#ifdef _DEBUG
			pNewLevel = CLevel_Editor::Create(m_pDevice, m_pContext);
//#endif
			break;
		case LEVEL::GAMEPLAY:
			pNewLevel = CLevel_GamePlay::Create(m_pDevice, m_pContext);
			break;

		case LEVEL::TEST_EXTRA1:
			pNewLevel = CStage_Test1::Create(m_pDevice, m_pContext);
			break;
		case LEVEL::CH01_KICK:
			pNewLevel = CStage_01Kick::Create(m_pDevice, m_pContext);
			break;
		case LEVEL::CH09_FIGHTC:
			pNewLevel = CStage_09FightC::Create(m_pDevice, m_pContext);
			break;
		case LEVEL::CH10_DESPER:
			pNewLevel = CStage_10Desper::Create(m_pDevice, m_pContext);
			break;
		}

		if (FAILED(m_pGameInstance->Open_Level(static_cast<_uint>(m_eNextLevelID), pNewLevel)))
			return;		
	}
}

HRESULT CLevel_Loading::Render()
{
	/* 생성해놓은 객체들을 렌더한다. */
	m_pLoader->Show_LoadingText();

	Render_LoadingScreen();

	return S_OK;
}

HRESULT CLevel_Loading::Ready_GameObjects()
{
	// Black Background Image for Loading Screen.
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::LOADING), TEXT("Prototype_Component_Texture_Black"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/_SUPERHOT/Background/black.png"), 1))))
		return E_FAIL;

	/* Prototype_GameObject_BackGround */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::LOADING), TEXT("Prototype_GameObject_BackGround"),
		CBackGround::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	// CBackGround 프로토타입으로부터 Create 함
	CGameObject::GAMEOBJECT_DESC tDesc = {};
	tDesc.iCustomLoadLevelIndex = ENUM_CLASS(LEVEL::LOADING);

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::LOADING), L"Layer_BackGround",
		ENUM_CLASS(LEVEL::LOADING), TEXT("Prototype_GameObject_BackGround"), &tDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Loading::Ready_LoadingThread()
{
	m_pLoader = CLoader::Create(m_pDevice, m_pContext, m_eNextLevelID);
	if (nullptr == m_pLoader)
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Loading::Update_DestLevel(LEVEL eLevel)
{
	m_pGameInstance->Set_DestLevel(ENUM_CLASS(eLevel));

	return S_OK;
}

HRESULT CLevel_Loading::Render_LoadingScreen()
{
	// 가로 58, 세로 25
	// 가로 15까지 공백, 16부터 30칸이 로딩 창 크기

	// 수평/수직 선: ─ │
	// 모서리: ┌ ┐ └ ┘
	// 분기/교차: ├ ┤ ┬ ┴ ┼
	// 굵은 선 느낌: ━ ┃ ┏ ┓ ┗ ┛ ┣ ┫ ┳ ┻ ╋
	// 이중선:	═ ║ ╔ ╗ ╝ ╚ ╬
	// 밝기 : █ ▓ ▒ ░ 



	/* Loading Glyph Control */
	static _uint iStackedFrame = 0;
	static _uint iLoadingGlyphIndex = 0;
	iStackedFrame++;

	if (iStackedFrame >= 5)
	{
		iStackedFrame = 0;
		iLoadingGlyphIndex = ++iLoadingGlyphIndex % 4;
	}

	_wstring strLoadingGlyph = L"";
	if		(iLoadingGlyphIndex == 0)	strLoadingGlyph = L"                                  \\";
	else if (iLoadingGlyphIndex == 1)	strLoadingGlyph = L"                                  |";
	else if (iLoadingGlyphIndex == 2)	strLoadingGlyph = L"                                  /";
	else if (iLoadingGlyphIndex == 3)	strLoadingGlyph = L"                                  -";



	_wstring strFontTag = L"Font_DOS";
	_uint iRenderStartX = 100.f;
	_uint iRenderStartY = 25.f;
	_uint iRenderSpaceY = 25.f;

	_vector vLoadColor = XMVectorSet(.7f, 0.f, 0.f, 1.f);

	m_pGameInstance->Render_Font_Begin(strFontTag);

	

	/* Text UI */
	iRenderStartY += iRenderSpaceY * 10;

	if (!m_pLoader->isFinished())
	{
		m_pGameInstance->Render_Font(strFontTag, L"                         LOADING", _float2(iRenderStartX, iRenderStartY + iRenderSpaceY * 3), vLoadColor);
		m_pGameInstance->Render_Font(strFontTag, strLoadingGlyph.c_str(), _float2(iRenderStartX, iRenderStartY + iRenderSpaceY * 3), vLoadColor);

		m_pGameInstance->Render_Font(strFontTag, L"                ──────────────────────────── ", _float2(iRenderStartX, iRenderStartY + iRenderSpaceY * 5), vLoadColor);
	}
	else
	{
		m_pGameInstance->Render_Font(strFontTag, L"                       LOAD COMPLETE", _float2(iRenderStartX, iRenderStartY + iRenderSpaceY * 3), vLoadColor);

		m_pGameInstance->Render_Font(strFontTag, L"                ────────────────press─ENTER─ ", _float2(iRenderStartX, iRenderStartY + iRenderSpaceY * 5), vLoadColor);
	}

	m_pGameInstance->Render_Font(strFontTag, L"               ┌────────────────────────────┐", _float2(iRenderStartX, iRenderStartY += iRenderSpaceY), vLoadColor);
	m_pGameInstance->Render_Font(strFontTag, L"               │                            │", _float2(iRenderStartX, iRenderStartY += iRenderSpaceY), vLoadColor);
	m_pGameInstance->Render_Font(strFontTag, L"               │                            │", _float2(iRenderStartX, iRenderStartY += iRenderSpaceY), vLoadColor);
	m_pGameInstance->Render_Font(strFontTag, L"               │                            │", _float2(iRenderStartX, iRenderStartY += iRenderSpaceY), vLoadColor);
	m_pGameInstance->Render_Font(strFontTag, L"               └                            ┘", _float2(iRenderStartX, iRenderStartY += iRenderSpaceY), vLoadColor);



	m_pGameInstance->Render_Font_End(strFontTag);

	return S_OK;
}

CLevel_Loading* CLevel_Loading::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eNextLevelID)
{
	CLevel_Loading* pInstance = new CLevel_Loading(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eNextLevelID)))
	{
		MSG_BOX(TEXT("Failed to Created : CLevel_Loading"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CLevel_Loading::Free()
{
	__super::Free();

	Safe_Release(m_pLoader);

}

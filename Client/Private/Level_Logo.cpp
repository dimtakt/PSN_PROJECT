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

	_wstring strFontTag = L"Font_DOS";
	m_pGameInstance->Render_Font_Begin(strFontTag);
	m_pGameInstance->Render_Font(strFontTag, L"Test", _float2(100.f, 100.f), XMVectorSet(1.f, 1.f, 1.f, 1.f));
	m_pGameInstance->Render_Font(strFontTag, L"==============================", _float2(100.f, 125.f), XMVectorSet(1.f, 1.f, 1.f, 1.f));
	m_pGameInstance->Render_Font(strFontTag, L"Enter : GamePlay", _float2(100.f, 150.f), XMVectorSet(1.f, 1.f, 1.f, 1.f));
	m_pGameInstance->Render_Font(strFontTag, L"E : Editor", _float2(100.f, 175.f), XMVectorSet(1.f, 1.f, 1.f, 1.f));
	m_pGameInstance->Render_Font_End(strFontTag);
	
	
	//m_pGameInstance->Render_Font(L"Font_Default", L"Hello World!", _float2(100.f, 100.f), XMVectorSet(1.f, 0.f, 0.f, 1.f));

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

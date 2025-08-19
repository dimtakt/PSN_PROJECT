
#include "MainApp.h"
#include "GameInstance.h"

// Levels
#include "Level_Loading.h"

// Static Models

#include "Player.h"
#include "Enemy.h"
#include "Props_Pot.h"
#include "Props_Fotel.h"
#include "Props_ServerRack1.h"
#include "Props_ServerRack2.h"


CMainApp::CMainApp()
	: m_pGameInstance{ CGameInstance::GetInstance() }
{
	// D3D11_SAMPLER_DESC

	Safe_AddRef(m_pGameInstance);
}

HRESULT CMainApp::Initialize()
{
	ENGINE_DESC		EngineDesc{};

	EngineDesc.hInst = g_hInst;
	EngineDesc.hWnd = g_hWnd;
	EngineDesc.eWinMode = WINMODE::WIN;
	EngineDesc.iWinSizeX = g_iWinSizeX;
	EngineDesc.iWinSizeY = g_iWinSizeY;
	EngineDesc.iNumLevels = ENUM_CLASS(LEVEL::END);

	if(FAILED(m_pGameInstance->Initialize_Engine(EngineDesc, &m_pDevice, &m_pContext)))
		return E_FAIL;

	if (FAILED(Ready_Prototype_ForStatic()))
		return E_FAIL;

	if (FAILED(Ready_Font()))
		return E_FAIL;

	if (FAILED(Start_Level(LEVEL::LOGO)))
		return E_FAIL;	

	return S_OK;
}

void CMainApp::Update(_float fTimeDelta)
{
	m_pGameInstance->Update_Engine(fTimeDelta);
}

HRESULT CMainApp::Render()
{
	_float4		vClearColor = _float4(0.f, 0.f, 1.f, 1.f);

	m_pGameInstance->Render_Begin(&vClearColor);

	m_pGameInstance->Draw();

	// ksta : 임시 폰트 출력
	//m_pGameInstance->Render_Font(TEXT("Font_DOS"), L"Test", _float2(0.f, 0.f), XMVectorSet(1.f, 1.f, 1.f, 1.f));

	m_pGameInstance->Render_End();

	return S_OK;
}


HRESULT CMainApp::Ready_Prototype_ForStatic()
{
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxPosTex.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
		CVIBuffer_Rect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Enemy"),
		CEnemy::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Player"),
		CPlayer::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Props_Pot"),
		CProps_Pot::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Props_Fotel"),
		CProps_Fotel::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Props_ServerRack1"),
		CProps_ServerRack1::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Props_ServerRack2"),
		CProps_ServerRack2::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	// 모델의 방향을 올바르게 로드하기 위해 사전에 변환용 벡터 정의
	_matrix		PreTransformMatrix = XMMatrixIdentity();
	PreTransformMatrix = XMMatrixRotationY(XMConvertToRadians(180.0f));

	/* Prototype_Component_Model_Enemy */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_Enemy"),
		CModel::Create(m_pDevice, m_pContext, MODELTYPE::ANIM, "../Bin/Resources/_SUPERHOT/Models/Enemy/Enemy.fbx", PreTransformMatrix))))
		return E_FAIL;

	/* Prototype_Component_Model_NonAnim Things */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_Props_Pot"),
		CModel::Create(m_pDevice, m_pContext, MODELTYPE::NONANIM, "../Bin/Resources/_SUPERHOT/Models/Props/pot/pot.fbx", PreTransformMatrix))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_Props_Fotel"),
		CModel::Create(m_pDevice, m_pContext, MODELTYPE::NONANIM, "../Bin/Resources/_SUPERHOT/Models/Props/fotel/fotel.fbx", PreTransformMatrix))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_Props_ServerRack1"),
		CModel::Create(m_pDevice, m_pContext, MODELTYPE::NONANIM, "../Bin/Resources/_SUPERHOT/Models/Props/ServerRack/ServerRack1.fbx", PreTransformMatrix))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_Props_ServerRack2"),
		CModel::Create(m_pDevice, m_pContext, MODELTYPE::NONANIM, "../Bin/Resources/_SUPERHOT/Models/Props/ServerRack/ServerRack2.fbx", PreTransformMatrix))))
		return E_FAIL;

	

	/* Texture */

	// Crosshair Images for Gameplay. 거의 항상 떠있어야 하므로 Static으로 둠.
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Crosshair_BasicHand"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/_SUPERHOT/Textures/Crosshair/140601 04-basic.png"), 1))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Crosshair_BasicDot"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/_SUPERHOT/Textures/Crosshair/140601 08-basic.png"), 1))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Crosshair_BasicPunch"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/_SUPERHOT/Textures/Crosshair/150109 punch 2.png"), 1))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Crosshair_Gun"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/_SUPERHOT/Textures/Crosshair/150702 celowniczek.png"), 1))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Crosshair_Katana"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/_SUPERHOT/Textures/Crosshair/150717 celownik katana.png"), 1))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Crosshair_HotswitchHover"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/_SUPERHOT/Textures/Crosshair/151028-hotswitch-crosshair-hover.png"), 1))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Crosshair_HotswitchIdle"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/_SUPERHOT/Textures/Crosshair/151028-hotswitch-crosshair-idle.png"), 1))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Crosshair_HotswitchWait"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/_SUPERHOT/Textures/Crosshair/151028-hotswitch-crosshair-wait.png"), 1))))
		return E_FAIL;



	//if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Transform"),
	//	CTransform::Create(m_pDevice, m_pContext))))
	//	return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Ready_Font()
{
	// 경로 잘못됨 등의 이유로 로드 실패 시..
	// CFont::Initialize_Font 단계에서 런타임 에러가 발생함.

	// 영어 기본폰트 로드
	if (FAILED(m_pGameInstance->Add_Font(
			L"Font_DOS",
			L"../Bin/Resources/_SUPERHOT/Fonts/PerfectDOSVGA437Win.spritefont"
	)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Font(
		L"Font_Default",
		L"../Bin/Resources/Fonts/140.spritefont"
	)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Start_Level(LEVEL eStartLevelID)
{
	if (FAILED(m_pGameInstance->Open_Level(static_cast<_uint>(LEVEL::LOADING), CLevel_Loading::Create(m_pDevice, m_pContext, eStartLevelID))))
		return E_FAIL;

	return S_OK;
}

CMainApp* CMainApp::Create()
{
	CMainApp* pInstance = new CMainApp();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CMainApp"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMainApp::Free()
{
	__super::Free();

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);

	m_pGameInstance->Release_Engine();

	Safe_Release(m_pGameInstance);
}

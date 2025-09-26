#include "Stage_ENDTower.h"
#include "Level_Loading.h"

#include "GameInstance.h"
#include "Camera_Player.h"
#include "Terrain.h"
#include "CustomObj_Pickupable.h"

#include "CustomObj.h"
#include "Enemy.h"

#include "UI_ScreenText.h"

CStage_ENDTower::CStage_ENDTower(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel_Stage{ pDevice, pContext }
{
}

HRESULT CStage_ENDTower::Initialize()
{
	if (FAILED(Ready_Lights()))
		return E_FAIL;

	/* 현재 레벨을 구성해주기 위한 객체들을 생성한다. */
	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_UI(TEXT("Layer_UI"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Player(TEXT("Layer_Player"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Monster(TEXT("Layer_Monster"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Effect(TEXT("Layer_Effect"))))
		return E_FAIL;

	_wstring strLoadPath = L"../Bin/Resources/_SUPERHOT/_BinaryLevels/StageEnd_Tower.datmap";
	if (FAILED(Load_BinaryMap(&strLoadPath)))
		return E_FAIL;

	// 맵 데이터를 통해 로드된 오브젝트들을 참고하여, 픽업 오브젝트로써 재정의 (로드 후 기존 것 삭제)
	if (FAILED(Ready_Pickup_Objects(TEXT("Layer_Pickup"))))
		return E_FAIL;

	return S_OK;
}

void CStage_ENDTower::Update(_float fTimeDelta)
{
	_uint iDestLevel = m_pGameInstance->Get_DestLevel();
	CGameObject* pFrontEnemy = m_pGameInstance->Find_GameObject(iDestLevel, L"Layer_Monster");

	// 시작 -> 1페이즈 -> 2페이즈 -> 다음레벨
	// 페이즈 전환 기준은 적이 없어질 때 마다
	if (pFrontEnemy == nullptr)
	{
		if (m_iPhase < 2)
		{
			m_iPhase++;
			m_iElapsedPhase++;

			if (m_iPhase >= 2)
				m_iPhase = 0;

			if (FAILED(Ready_Layer_Monster(TEXT("Layer_Monster"))))
				MSG_BOX(L"[CStage_ENDTower:Update] Monster Spawn Failed. Cur Phase : " + m_iPhase);

			return;
		}
	}

	//Update_TriggerOnce();

	if (!m_isEndLevelStandby)
		Update_Trigger_OnTime(fTimeDelta);


	// 일정 페이즈가 지났으면
	if (m_iElapsedPhase > m_iTriggerPhase &&
		!m_isHSTriggered)
	{
		m_isHSTriggered = true;
		CGameObject* pPlayerObj = m_pGameInstance->Find_GameObject(iDestLevel, L"Layer_Player");
		CPlayer* pPlayer = dynamic_cast<CPlayer*>(pPlayerObj);

		pPlayer->Set_HotSwitchOn();
		m_pUI_ScreenText->Show_ScreenText(ENUM_CLASS(SCREENTEXT_INDEX::LVLEND_HSINTO));
	}

	// 레벨 종료 조건 검사
	//Update_CheckEndLevel(fTimeDelta, LEVEL::CH09_FIGHTC);
}

HRESULT CStage_ENDTower::Render()
{
	SetWindowText(g_hWnd, TEXT("Level : CH01_KICK (Temp)"));

	return S_OK;
}

HRESULT CStage_ENDTower::Ready_Lights()
{
	// ksta : 테스트 종료 후 활성화
	return S_OK;

	LIGHT_DESC			LightDesc{};

	//(LightDesc.Diffuse * MtrlDesc.Diffuse) * (fShade(0 ~ 1) + (LightDesc.Ambient * MtrlDesc.Ambient))

	LightDesc.eType = LIGHT_DESC::TYPE::DIRECTIONAL;
	LightDesc.vDirection = _float4(0.5f, -1.f, 0.5f, 0.f);	// Light 방향
	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.0f);		// Light 색상 및 밝기의 세기
	LightDesc.vAmbient = _float4(0.4f, 0.4f, 0.4f, 1.f);	// Light 환경광으로 가정. 최소 밝기 보장에 관여.
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);		// Light 반사광.


	if (FAILED(m_pGameInstance->Add_Light(LightDesc)))
		return E_FAIL;


	SHADOW_LIGHT_DESC			ShadowLightDesc{};

	//ShadowLightDesc.vEye = _float4(-20.f, 20.f, -20.f, 1.f);
	//ShadowLightDesc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
	//ShadowLightDesc.fFovy = XMConvertToRadians(60.f);
	//ShadowLightDesc.fNear = 0.1f;
	//ShadowLightDesc.fFar = 1000.f;
	ShadowLightDesc.vEye = _float4(0.1f, -1.f, 0.1f, 0.f);
	ShadowLightDesc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
	ShadowLightDesc.fFovy = XMConvertToRadians(60.f);
	ShadowLightDesc.fNear = 0.1f;
	ShadowLightDesc.fFar = 1000.f;

	if (FAILED(m_pGameInstance->Ready_ShadowLight(ShadowLightDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CStage_ENDTower::Ready_Layer_Camera(const _wstring& strLayerTag)
{
	_uint iDestLevel = m_pGameInstance->Get_DestLevel();

	CCamera_Player::CAMERA_PLAYER_DESC		CameraDesc{};
	CameraDesc.vEye = _float4(0.f, 20.f, -15.f, 1.f);
	CameraDesc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
	CameraDesc.fFovy = XMConvertToRadians(60.0f);
	CameraDesc.fNear = 0.1f; // 0.1f;
	CameraDesc.fFar = 500.f;
	CameraDesc.fSpeedPerSec = 10.f;
	CameraDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	CameraDesc.fMouseSensor = .2f;

	//if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::TEST_EXTRA3), strLayerTag,
	//	ENUM_CLASS(LEVEL::TEST_EXTRA3), TEXT("Prototype_GameObject_Camera_Free"), &CameraDesc)))
	//	return E_FAIL;
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(iDestLevel, strLayerTag,
		ENUM_CLASS(LEVEL::TEST_EXTRA3), TEXT("Prototype_GameObject_Camera_Player"), &CameraDesc)))
		return E_FAIL;
	m_pCameraPlayer = dynamic_cast<CCamera_Player*>(m_pGameInstance->Get_LastGameObject(iDestLevel, strLayerTag));

	return S_OK;
}

HRESULT CStage_ENDTower::Ready_Layer_BackGround(const _wstring& strLayerTag)
{
	//if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::TEST_EXTRA3), strLayerTag,
	//	ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Terrain"))))
	//	return E_FAIL; .// !! 필요없을듯>

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::TEST_EXTRA3), strLayerTag,
		ENUM_CLASS(LEVEL::TEST_EXTRA3), TEXT("Prototype_GameObject_Skybox"))))
		return E_FAIL; // !!

	return S_OK;
}

HRESULT CStage_ENDTower::Ready_Layer_UI(const _wstring& strLayerTag)
{
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::TEST_EXTRA3), strLayerTag + L"_Crosshair",
		ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_UI_Crosshair"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::TEST_EXTRA3), strLayerTag + L"_ScreenText",
		ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_UI_ScreenText"))))
		return E_FAIL;

	m_pUI_ScreenText = dynamic_cast<CUI_ScreenText*>(m_pGameInstance->Get_LastGameObject(ENUM_CLASS(LEVEL::TEST_EXTRA3), strLayerTag + L"_ScreenText"));
	CLevel_Stage::m_pUI_ScreenText = m_pUI_ScreenText;

	return S_OK;
}

HRESULT CStage_ENDTower::Ready_Layer_Player(const _wstring& strLayerTag)
{
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::TEST_EXTRA3), strLayerTag,
		ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Player"))))
		return E_FAIL;
	CGameObject* pPlayer = m_pGameInstance->Get_LastGameObject(ENUM_CLASS(LEVEL::TEST_EXTRA3), strLayerTag);

	_float4 pPos = { 153.648f, 95.f, 29.7477f, 1.f };
	dynamic_cast<CTransform*>(pPlayer->Get_Component(L"Com_Transform"))->Set_State(STATE::POSITION, XMLoadFloat4(&pPos));
	dynamic_cast<CTransform*>(pPlayer->Get_Component(L"Com_Transform"))->Rotation(XMVectorSet(0.f, 1.f, 0.f, 1.f), TO_RAD(0.762081f));

	return S_OK;
}

HRESULT CStage_ENDTower::Ready_Layer_Monster(const _wstring& strLayerTag)
{
	// ksta : Enemy는 모든 레벨에서 사용할 것이라, 프로토타입은 Static으로 주는 게 좋을 듯.
	// 이 부분은 레이어 추가 부분이기에 이미 선언된 프로토타입을 이용하는 것임.

	//if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), strLayerTag,
	//	ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Monster"))))
	//	return E_FAIL;

	_uint iDestLevel = m_pGameInstance->Get_DestLevel();

	CEnemy::ENEMY_DESC EnemyDesc{};

	EnemyDesc.fRotationPerSec = XMConvertToRadians(180.f);
	EnemyDesc.fSpeedPerSec = 15.f;

	_float4 vStartPos = {};
	_float	fStartRotDeg = {};
	CGameObject* pEnemy = nullptr;



	switch (m_iPhase)
	{
	case 0:	// At Start, Loop
	{
		// 왼쪽
		vStartPos = { 151.965f, 5.f, 8.95373f, 1.f };
		EnemyDesc.iFirstCellIndex = 31;
		fStartRotDeg = 4.0185f;

		EnemyDesc.vecPremovePoses = {
			{ 113.542f, 85.f, 75.0491f },
			{ 127.053f, 85.f, 71.5461f }
		};
		EnemyDesc.iDefaultWeaponObjType = ENUM_CLASS(GAMEOBJ_TYPE::WEAPON_RANGED_PISTOL);

		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::TEST_EXTRA3), strLayerTag,
			ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Enemy"), &EnemyDesc)))
			return E_FAIL;
		pEnemy = m_pGameInstance->Get_LastGameObject(iDestLevel, strLayerTag);
		dynamic_cast<CTransform*>(pEnemy->Get_Component(L"Com_Transform"))->Set_State(STATE::POSITION, XMLoadFloat4(&vStartPos));
		dynamic_cast<CTransform*>(pEnemy->Get_Component(L"Com_Transform"))->Rotation(XMVectorSet(0.f, 1.f, 0.f, 1.f), TO_RAD(fStartRotDeg));

		// 오른쪽
		vStartPos = { 189.007f, 85.f, 35.3903f, 1.f };
		EnemyDesc.iFirstCellIndex = 176;
		fStartRotDeg = 0.129672f;

		EnemyDesc.vecPremovePoses = {
			{ 187.397f, 85.f, 72.8008f  },
			{ 175.751f, 85.f, 81.0559f  }
		};
		EnemyDesc.iDefaultWeaponObjType = ENUM_CLASS(GAMEOBJ_TYPE::WEAPON_RANGED_PISTOL);

		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::TEST_EXTRA3), strLayerTag,
			ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Enemy"), &EnemyDesc)))
			return E_FAIL;
		pEnemy = m_pGameInstance->Get_LastGameObject(iDestLevel, strLayerTag);
		dynamic_cast<CTransform*>(pEnemy->Get_Component(L"Com_Transform"))->Set_State(STATE::POSITION, XMLoadFloat4(&vStartPos));
		dynamic_cast<CTransform*>(pEnemy->Get_Component(L"Com_Transform"))->Rotation(XMVectorSet(0.f, 1.f, 0.f, 1.f), TO_RAD(fStartRotDeg));
	}break;

	case 1:	// Second. Next Loop. After End, handover to 0
		// 왼쪽
		vStartPos = { 63.4545f, 85.f, 138.1f, 1.f };
		EnemyDesc.iFirstCellIndex = 78;
		fStartRotDeg = -172.895f;

		EnemyDesc.vecPremovePoses = {
			{ 68.4156f, 85.f, 97.5013f },
			{ 88.7193f, 85.f, 81.1137f  }
		};
		EnemyDesc.iDefaultWeaponObjType = ENUM_CLASS(GAMEOBJ_TYPE::WEAPON_RANGED_KARABIN);

		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::TEST_EXTRA3), strLayerTag,
			ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Enemy"), &EnemyDesc)))
			return E_FAIL;
		pEnemy = m_pGameInstance->Get_LastGameObject(iDestLevel, strLayerTag);
		dynamic_cast<CTransform*>(pEnemy->Get_Component(L"Com_Transform"))->Set_State(STATE::POSITION, XMLoadFloat4(&vStartPos));
		dynamic_cast<CTransform*>(pEnemy->Get_Component(L"Com_Transform"))->Rotation(XMVectorSet(0.f, 1.f, 0.f, 1.f), TO_RAD(fStartRotDeg));

		// 오른쪽
		vStartPos = { 215.181f, 85.f, 155.584f, 1.f };
		EnemyDesc.iFirstCellIndex = 142;
		fStartRotDeg = -90.2501f;

		EnemyDesc.vecPremovePoses = {
			{ 175.298f, 85.f, 96.5804f  }
		};
		EnemyDesc.iDefaultWeaponObjType = ENUM_CLASS(GAMEOBJ_TYPE::WEAPON_RANGED_SHOTGUN);

		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::TEST_EXTRA3), strLayerTag,
			ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Enemy"), &EnemyDesc)))
			return E_FAIL;
		pEnemy = m_pGameInstance->Get_LastGameObject(iDestLevel, strLayerTag);
		dynamic_cast<CTransform*>(pEnemy->Get_Component(L"Com_Transform"))->Set_State(STATE::POSITION, XMLoadFloat4(&vStartPos));
		dynamic_cast<CTransform*>(pEnemy->Get_Component(L"Com_Transform"))->Rotation(XMVectorSet(0.f, 1.f, 0.f, 1.f), TO_RAD(fStartRotDeg));
	}

	return S_OK;
}

HRESULT CStage_ENDTower::Ready_Layer_Effect(const _wstring& strLayerTag)
{
	//if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::TEST_EXTRA3), strLayerTag,
	//	ENUM_CLASS(LEVEL::TEST_EXTRA3), TEXT("Prototype_GameObject_Particle_TriEffect"))))
	//	return E_FAIL;

	_uint iDestLevel = m_pGameInstance->Get_DestLevel();

	//if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(iDestLevel, L"Layer_Particle_HitEffect",
	//	iDestLevel, TEXT("Prototype_GameObject_Particle_HitEffect"))))
	//	return E_FAIL;

	return S_OK;
}

HRESULT CStage_ENDTower::Ready_Pickup_Objects(const _wstring& strLayerTag)
{
	_wstring strLoadedTag = L"Layer_Loaded_Object";		// 로드된 오브젝트들이 담긴 레이어 태그

	_uint iIndex = 0;
	_uint iDestLevel = m_pGameInstance->Get_DestLevel();

	// 현재 맵에 무기 오브젝트들이 없는 경우에는 해당 모델이 로드되어 있지 않음으로 인한 문제가 생기므로 미리 로드
	_matrix		PreTransformMatrix = XMMatrixIdentity();
	PreTransformMatrix = XMMatrixRotationY(XMConvertToRadians(180.0f));

	if FAILED(Add_Prototype_Direct(iDestLevel, L"Weapon_Karabin_Fixed", &PreTransformMatrix))
	{
#ifdef _DEBUG
		std::cout << "[CStage_ENDTower::Ready_Pickup_Objects] Prototype Create Failed. (Weapon_Karabin_Fixed)" << std::endl;
#endif
	}
	if FAILED(Add_Prototype_Direct(iDestLevel, L"Weapon_Pistol_Fixed", &PreTransformMatrix))
	{
#ifdef _DEBUG
		std::cout << "[CStage_ENDTower::Ready_Pickup_Objects] Prototype Create Failed. (Weapon_Pistol_Fixed)" << std::endl;
#endif
	}
	if FAILED(Add_Prototype_Direct(iDestLevel, L"Weapon_Shotgun_Fixed", &PreTransformMatrix))
	{
#ifdef _DEBUG
		std::cout << "[CStage_ENDTower::Ready_Pickup_Objects] Prototype Create Failed. (Weapon_Shotgun_Fixed)" << std::endl;
#endif
	}


	// 오브젝트 검사 순회. 
	// 로드된 오브젝트들 중 무기와 같이 pickupable 해야하는 오브젝트들은 해당 오브젝트로 교체 (타입 재생성 후 원본삭제)
	while (true)
	{
		CGameObject* pTargetObject = m_pGameInstance->Find_GameObject(iDestLevel, strLoadedTag, iIndex);
		if (pTargetObject == nullptr)
			break;

		_uint iObjType = pTargetObject->Get_ObjType();



		CCustomObj_Pickupable::THROWN_PICKUPOBJ_DESC tDesc = {};

		switch (iObjType)
		{
			// 특정 타입이라면..
			// 여기서 1. 새로 pickupobj 추가, 2. pickupobj의 모티브가 됐던 오브젝트 제거 구현할 것
		case ENUM_CLASS(GAMEOBJ_TYPE::WEAPON_RANGED_KARABIN):
		{
			tDesc.strModelComPrototypeTag = L"Prototype_Component_Model_Custom_Weapon_Karabin_Fixed";
			tDesc.iGameObjType = pTargetObject->Get_ObjType();

			if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(iDestLevel, L"Layer_Loaded_Object_Pickupable",
				ENUM_CLASS(LEVEL::STATIC), L"Prototype_GameObject_Pickupable", &tDesc)))
				return E_FAIL;
			CTransform* pObjTransformCom = static_cast<CTransform*>((m_pGameInstance->Get_LastGameObject(iDestLevel, L"Layer_Loaded_Object_Pickupable")->Get_Component(L"Com_Transform")));
			pObjTransformCom->Set_WorldMatrix(static_cast<CTransform*>(pTargetObject->Get_Component(L"Com_Transform"))->Get_WorldMatrix());
			if (FAILED(m_pGameInstance->Remove_GameObject_FromLayer(iDestLevel, L"Layer_Loaded_Object", pTargetObject)))
				return E_FAIL;

		}break;
		case ENUM_CLASS(GAMEOBJ_TYPE::WEAPON_RANGED_PISTOL):
		{
			tDesc.strModelComPrototypeTag = L"Prototype_Component_Model_Custom_Weapon_Pistol_Fixed";
			tDesc.iGameObjType = pTargetObject->Get_ObjType();

			if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(iDestLevel, L"Layer_Loaded_Object_Pickupable",
				ENUM_CLASS(LEVEL::STATIC), L"Prototype_GameObject_Pickupable", &tDesc)))
				return E_FAIL;
			CTransform* pObjTransformCom = static_cast<CTransform*>((m_pGameInstance->Get_LastGameObject(iDestLevel, L"Layer_Loaded_Object_Pickupable")->Get_Component(L"Com_Transform")));
			pObjTransformCom->Set_WorldMatrix(static_cast<CTransform*>(pTargetObject->Get_Component(L"Com_Transform"))->Get_WorldMatrix());
			if (FAILED(m_pGameInstance->Remove_GameObject_FromLayer(iDestLevel, L"Layer_Loaded_Object", pTargetObject)))
				return E_FAIL;
		}break;
		case ENUM_CLASS(GAMEOBJ_TYPE::WEAPON_RANGED_SHOTGUN):
		{
			tDesc.strModelComPrototypeTag = L"Prototype_Component_Model_Custom_Weapon_Shotgun_Fixed";
			tDesc.iGameObjType = pTargetObject->Get_ObjType();

			if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(iDestLevel, L"Layer_Loaded_Object_Pickupable",
				ENUM_CLASS(LEVEL::STATIC), L"Prototype_GameObject_Pickupable", &tDesc)))
				return E_FAIL;
			CTransform* pObjTransformCom = static_cast<CTransform*>((m_pGameInstance->Get_LastGameObject(iDestLevel, L"Layer_Loaded_Object_Pickupable")->Get_Component(L"Com_Transform")));
			pObjTransformCom->Set_WorldMatrix(static_cast<CTransform*>(pTargetObject->Get_Component(L"Com_Transform"))->Get_WorldMatrix());
			if (FAILED(m_pGameInstance->Remove_GameObject_FromLayer(iDestLevel, L"Layer_Loaded_Object", pTargetObject)))
				return E_FAIL;
		}break;
		case ENUM_CLASS(GAMEOBJ_TYPE::WEAPON_MELEE_KNIFE):
		{
			int test = 10;
		}

		default:
			iIndex++;
			break;
		}


	}

	return S_OK;
}

void CStage_ENDTower::Update_Trigger_OnTime(_float fTimeDelta)
{
	//m_fTimeEventDeltaTime += m_pGameInstance->Get_RawTimeDelta();


	//if ((m_fTimeEventDeltaTime >= 0.8f) && (m_iStartTextIndex == 0))
	//{
	//	m_pUI_ScreenText->Show_ScreenText(ENUM_CLASS(SCREENTEXT_INDEX::LVLSTART_10_1));
	//	m_iStartTextIndex++;
	//}
	//else if ((m_fTimeEventDeltaTime >= 1.5f) && (m_iStartTextIndex == 1))
	//{
	//	m_pUI_ScreenText->Show_ScreenText(ENUM_CLASS(SCREENTEXT_INDEX::LVLSTART_10_2));
	//	m_iStartTextIndex++;
	//}


	//if (m_iPhase == 2 && m_iStartTextIndex == 2)
	//{
	//	m_fTimeEventDeltaTime = 0.f;
	//	m_iStartTextIndex++;
	//}
	//else if ((m_fTimeEventDeltaTime >= 0.f) && (m_iStartTextIndex == 3))
	//{
	//	m_pUI_ScreenText->Show_ScreenText(ENUM_CLASS(SCREENTEXT_INDEX::LVLMID_10_1));
	//	m_iStartTextIndex++;
	//}
	//else if ((m_fTimeEventDeltaTime >= 0.5f) && (m_iStartTextIndex == 4))
	//{
	//	m_pUI_ScreenText->Show_ScreenText(ENUM_CLASS(SCREENTEXT_INDEX::LVLMID_10_2));
	//	m_iStartTextIndex++;
	//}
	//else if ((m_fTimeEventDeltaTime >= 0.9f) && (m_iStartTextIndex == 5))
	//{
	//	m_pUI_ScreenText->Show_ScreenText(ENUM_CLASS(SCREENTEXT_INDEX::LVLMID_10_3));
	//	m_iStartTextIndex++;
	//}
}

CStage_ENDTower* CStage_ENDTower::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CStage_ENDTower* pInstance = new CStage_ENDTower(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CStage_ENDTower"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CStage_ENDTower::Free()
{
	__super::Free();



}

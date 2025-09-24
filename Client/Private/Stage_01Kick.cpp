#include "Stage_01Kick.h"
#include "Level_Loading.h"

#include "GameInstance.h"
#include "Camera_Player.h"
#include "Terrain.h"
#include "CustomObj_Pickupable.h"

#include "CustomObj.h"
#include "Enemy.h"

#include "UI_ScreenText.h"

CStage_01Kick::CStage_01Kick(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel_Stage{ pDevice, pContext }
{
}

HRESULT CStage_01Kick::Initialize()
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

	_wstring strLoadPath = L"../Bin/Resources/_SUPERHOT/_BinaryLevels/Stage_01Kick.datmap";
	if (FAILED(Load_BinaryMap(&strLoadPath)))
		return E_FAIL;

	// 맵 데이터를 통해 로드된 오브젝트들을 참고하여, 픽업 오브젝트로써 재정의 (로드 후 기존 것 삭제)
	if (FAILED(Ready_Pickup_Objects(TEXT("Layer_Pickup"))))
		return E_FAIL;

	return S_OK;
}

void CStage_01Kick::Update(_float fTimeDelta)
{
	// 레벨 시작 시 최초 이벤트
	Update_TriggerOnce();

	// 레벨 시작 후 n초 뒤 일어날 이벤트
	if (!m_isUIEventTriggered)
	{
		m_fUIEventDeltaTime += fTimeDelta;

		if (m_fUIEventDeltaTime >= 0.8f)
		{
			m_pUIScreenText->Show_ScreenText(ENUM_CLASS(SCREENTEXT_INDEX::TIMEMOVES));
			m_isUIEventTriggered = true;
		}
	}

	// 레벨 종료 조건 검사
	Update_CheckEndLevel(fTimeDelta);
}

HRESULT CStage_01Kick::Render()
{
	SetWindowText(g_hWnd, TEXT("Level : CH01_KICK (Temp)"));

	return S_OK;
}

HRESULT CStage_01Kick::Ready_Lights()
{
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

HRESULT CStage_01Kick::Ready_Layer_Camera(const _wstring& strLayerTag)
{
	CCamera_Player::CAMERA_PLAYER_DESC		CameraDesc{};
	CameraDesc.vEye = _float4(0.f, 20.f, -15.f, 1.f);
	CameraDesc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
	CameraDesc.fFovy = XMConvertToRadians(60.0f);
	CameraDesc.fNear = 0.1f; // 0.1f;
	CameraDesc.fFar = 500.f;
	CameraDesc.fSpeedPerSec = 10.f;
	CameraDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	CameraDesc.fMouseSensor = .2f;

	//if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::CH01_KICK), strLayerTag,
	//	ENUM_CLASS(LEVEL::CH01_KICK), TEXT("Prototype_GameObject_Camera_Free"), &CameraDesc)))
	//	return E_FAIL;
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::CH01_KICK), strLayerTag,
		ENUM_CLASS(LEVEL::CH01_KICK), TEXT("Prototype_GameObject_Camera_Player"), &CameraDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CStage_01Kick::Ready_Layer_BackGround(const _wstring& strLayerTag)
{
	//if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::CH01_KICK), strLayerTag,
	//	ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Terrain"))))
	//	return E_FAIL; .// !! 필요없을듯>

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::CH01_KICK), strLayerTag,
		ENUM_CLASS(LEVEL::CH01_KICK), TEXT("Prototype_GameObject_Skybox"))))
		return E_FAIL; // !!

	return S_OK;
}

HRESULT CStage_01Kick::Ready_Layer_UI(const _wstring& strLayerTag)
{
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::CH01_KICK), strLayerTag + L"_Crosshair",
		ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_UI_Crosshair"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::CH01_KICK), strLayerTag + L"_ScreenText",
		ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_UI_ScreenText"))))
		return E_FAIL;

	m_pUIScreenText = dynamic_cast<CUI_ScreenText*>(m_pGameInstance->Get_LastGameObject(ENUM_CLASS(LEVEL::CH01_KICK), strLayerTag + L"_ScreenText"));
	CLevel_Stage::m_pUIScreenText = m_pUIScreenText;

	return S_OK;
}

HRESULT CStage_01Kick::Ready_Layer_Player(const _wstring& strLayerTag)
{
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::CH01_KICK), strLayerTag,
		ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Player"))))
		return E_FAIL;
	CGameObject* pPlayer = m_pGameInstance->Get_LastGameObject(ENUM_CLASS(LEVEL::CH01_KICK), strLayerTag);

	_float4 pPos = { 49.f, 96.f, 80.f, 1.f };
	dynamic_cast<CTransform*>(pPlayer->Get_Component(L"Com_Transform"))->Set_State(STATE::POSITION, XMLoadFloat4(&pPos));
	dynamic_cast<CTransform*>(pPlayer->Get_Component(L"Com_Transform"))->Rotation(XMVectorSet(0.f, 1.f, 0.f, 1.f), TO_RAD(114));

	return S_OK;
}

HRESULT CStage_01Kick::Ready_Layer_Monster(const _wstring& strLayerTag)
{
	// ksta : Enemy는 모든 레벨에서 사용할 것이라, 프로토타입은 Static으로 주는 게 좋을 듯.
	// 이 부분은 레이어 추가 부분이기에 이미 선언된 프로토타입을 이용하는 것임.

	//if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), strLayerTag,
	//	ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Monster"))))
	//	return E_FAIL;

	CEnemy::ENEMY_DESC EnemyDesc{};

	EnemyDesc.fRotationPerSec = XMConvertToRadians(180.f);
	EnemyDesc.fSpeedPerSec = 15.f;

	_float4 vStartPos = {};
	CGameObject* pEnemy = nullptr;

	
	// 1번 적 - 멀리서 뛰어옴.
	vStartPos = { 77.289f, 115.175f, 10.144f, 1.f };
	EnemyDesc.vecPremovePoses = {
		_float3{156.4f, 96.7f, 11.5f},
		_float3{156.9f, 96.7f, 49.7f},
		_float3{144.1f, 96.7f, 53.0f}
	};
	EnemyDesc.iDefaultWeaponObjType = ENUM_CLASS(GAMEOBJ_TYPE::END);

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::CH01_KICK), strLayerTag,
		ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Enemy"), &EnemyDesc)))
		return E_FAIL;
	pEnemy = m_pGameInstance->Get_LastGameObject(ENUM_CLASS(LEVEL::CH01_KICK), strLayerTag);

	dynamic_cast<CTransform*>(pEnemy->Get_Component(L"Com_Transform"))->Set_State(STATE::POSITION, XMLoadFloat4(&vStartPos));
	dynamic_cast<CTransform*>(pEnemy->Get_Component(L"Com_Transform"))->Rotation(XMVectorSet(0.f, 1.f, 0.f, 1.f), TO_RAD(180));


	// 2번 적 - 원거리에서 사격.
	vStartPos = { 158.f, 96.f, 58.f, 1.f };
	EnemyDesc.vecPremovePoses = {};
	EnemyDesc.iDefaultWeaponObjType = ENUM_CLASS(GAMEOBJ_TYPE::WEAPON_RANGED_PISTOL);

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::CH01_KICK), strLayerTag,
		ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Enemy"), &EnemyDesc)))
		return E_FAIL;
	pEnemy = m_pGameInstance->Get_LastGameObject(ENUM_CLASS(LEVEL::CH01_KICK), strLayerTag);

	dynamic_cast<CTransform*>(pEnemy->Get_Component(L"Com_Transform"))->Set_State(STATE::POSITION, XMLoadFloat4(&vStartPos));
	dynamic_cast<CTransform*>(pEnemy->Get_Component(L"Com_Transform"))->Rotation(XMVectorSet(0.f, 1.f, 0.f, 1.f), TO_RAD(180));


	// 3번 적 - 시작하자마자 죽는 해당 위치
	vStartPos = { 72.f, 96.f, 71.f, 1.f };
	EnemyDesc.vecPremovePoses = {};
	EnemyDesc.iDefaultWeaponObjType = ENUM_CLASS(GAMEOBJ_TYPE::END);

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::CH01_KICK), strLayerTag,
		ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Enemy"), &EnemyDesc)))
		return E_FAIL;
	pEnemy = m_pGameInstance->Get_LastGameObject(ENUM_CLASS(LEVEL::CH01_KICK), strLayerTag);

	dynamic_cast<CTransform*>(pEnemy->Get_Component(L"Com_Transform"))->Set_State(STATE::POSITION, XMLoadFloat4(&vStartPos));
	dynamic_cast<CTransform*>(pEnemy->Get_Component(L"Com_Transform"))->Rotation(XMVectorSet(0.f, 1.f, 0.f, 1.f), TO_RAD(180));

	return S_OK;
}

HRESULT CStage_01Kick::Ready_Layer_Effect(const _wstring& strLayerTag)
{
	//if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::CH01_KICK), strLayerTag,
	//	ENUM_CLASS(LEVEL::CH01_KICK), TEXT("Prototype_GameObject_Particle_TriEffect"))))
	//	return E_FAIL;

	_uint iDestLevel = m_pGameInstance->Get_DestLevel();

	//if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(iDestLevel, L"Layer_Particle_HitEffect",
	//	iDestLevel, TEXT("Prototype_GameObject_Particle_HitEffect"))))
	//	return E_FAIL;

	return S_OK;
}

HRESULT CStage_01Kick::Ready_Pickup_Objects(const _wstring& strLayerTag)
{
	_wstring strLoadedTag = L"Layer_Loaded_Object";		// 로드된 오브젝트들이 담긴 레이어 태그

	_uint iIndex = 0;
	_uint iDestLevel = m_pGameInstance->Get_DestLevel();

	// 현재 맵에 무기 오브젝트들이 없는 경우에는 해당 모델이 로드되어 있지 않음으로 인한 문제가 생기므로 미리 로드
	_matrix		PreTransformMatrix = XMMatrixIdentity();
	PreTransformMatrix = XMMatrixRotationY(XMConvertToRadians(180.0f));

	if FAILED(Add_Prototype_Direct(iDestLevel, L"Weapon_Karabin_Fixed", &PreTransformMatrix))
		std::cout << "[CStage_01Kick::Ready_Pickup_Objects] Prototype Create Failed. (Weapon_Karabin_Fixed)" << std::endl;
	if FAILED(Add_Prototype_Direct(iDestLevel, L"Weapon_Pistol_Fixed", &PreTransformMatrix))
		std::cout << "[CStage_01Kick::Ready_Pickup_Objects] Prototype Create Failed. (Weapon_Pistol_Fixed)" << std::endl;
	if FAILED(Add_Prototype_Direct(iDestLevel, L"Weapon_Shotgun_Fixed", &PreTransformMatrix))
		std::cout << "[CStage_01Kick::Ready_Pickup_Objects] Prototype Create Failed. (Weapon_Shotgun_Fixed)" << std::endl;


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
		default:
			iIndex++;
			break;
		}


	}

	return S_OK;
}

void CStage_01Kick::Update_TriggerOnce()
{
	if (m_isTriggered)
		return;

	// ==============================
	_uint iDestLevel = m_pGameInstance->Get_DestLevel();
	CPlayer* pPlayer = dynamic_cast<CPlayer*> (m_pGameInstance->Find_GameObject(iDestLevel, L"Layer_Player"));
	CEnemy* pFirstEnemy = dynamic_cast<CEnemy*> (m_pGameInstance->Find_GameObject(iDestLevel, L"Layer_Monster", 2U));

	_vector vStartPos = XMLoadFloat4(m_pGameInstance->Get_CamPosition());
	_vector vTargetPos = static_cast<CTransform*>(pFirstEnemy->CGameObject::Get_Component(L"Com_Transform"))->Get_Position();
	vTargetPos = XMVectorSetY(vTargetPos, XMVectorGetY(vTargetPos) + 5.f);


	//_vector vDir = m_pGameInstance->Get_Transform_Matrix_Inverse(D3DTS::VIEW).r[2];
	_vector vDir = vTargetPos - vStartPos;
	vDir = XMVector3Normalize(vDir);

	static_cast<CWeapon_Gun*>(pPlayer->Get_WeaponPart())->Shot(&vDir, ENUM_CLASS(GAMEOBJ_TYPE::PLAYERBULLET));
	m_pGameInstance->Req_EditTimeSpeed(1.0f, true);
	m_pGameInstance->Req_EditTimeSpeed(0.01f);
	// ==============================

	m_isTriggered = true;
}

CStage_01Kick* CStage_01Kick::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CStage_01Kick* pInstance = new CStage_01Kick(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CStage_01Kick"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CStage_01Kick::Free()
{
	__super::Free();



}

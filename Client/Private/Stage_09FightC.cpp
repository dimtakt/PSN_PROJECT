#include "Stage_09FightC.h"

#include "GameInstance.h"
#include "Camera_Player.h"
#include "Terrain.h"
#include "CustomObj_Pickupable.h"

#include "CustomObj.h"
#include "Enemy.h"

#include "UI_ScreenText.h"

CStage_09FightC::CStage_09FightC(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel_Stage{ pDevice, pContext }
{
}

HRESULT CStage_09FightC::Initialize()
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

	_wstring strLoadPath = L"../Bin/Resources/_SUPERHOT/_BinaryLevels/Stage_09CageFight.datmap";
	if (FAILED(Load_BinaryMap(&strLoadPath)))
		return E_FAIL;

	// 맵 데이터를 통해 로드된 오브젝트들을 참고하여, 픽업 오브젝트로써 재정의 (로드 후 기존 것 삭제)
	if (FAILED(Ready_Pickup_Objects(TEXT("Layer_Pickup"))))
		return E_FAIL;



	return S_OK;
}

void CStage_09FightC::Update(_float fTimeDelta)
{




	Update_CheckEndLevel(fTimeDelta, LEVEL::CH10_DESPER);	// case 정의 필요
}

HRESULT CStage_09FightC::Render()
{
	SetWindowText(g_hWnd, TEXT("Level : CH09_FIGHTC (Temp)"));

	return S_OK;
}

HRESULT CStage_09FightC::Ready_Lights()
{
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

HRESULT CStage_09FightC::Ready_Layer_Camera(const _wstring& strLayerTag)
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

	//if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::CH09_FIGHTC), strLayerTag,
	//	ENUM_CLASS(LEVEL::CH09_FIGHTC), TEXT("Prototype_GameObject_Camera_Free"), &CameraDesc)))
	//	return E_FAIL;
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::CH09_FIGHTC), strLayerTag,
		ENUM_CLASS(LEVEL::CH09_FIGHTC), TEXT("Prototype_GameObject_Camera_Player"), &CameraDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CStage_09FightC::Ready_Layer_BackGround(const _wstring& strLayerTag)
{
	//if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::CH09_FIGHTC), strLayerTag,
	//	ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Terrain"))))
	//	return E_FAIL; .// !! 필요없을듯>

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::CH09_FIGHTC), strLayerTag,
		ENUM_CLASS(LEVEL::CH09_FIGHTC), TEXT("Prototype_GameObject_Skybox"))))
		return E_FAIL; // !!

	return S_OK;
}

HRESULT CStage_09FightC::Ready_Layer_UI(const _wstring& strLayerTag)
{
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::CH09_FIGHTC), strLayerTag + L"_Crosshair",
		ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_UI_Crosshair"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::CH09_FIGHTC), strLayerTag + L"_ScreenText",
		ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_UI_ScreenText"))))
		return E_FAIL;

	m_pUI_ScreenText = dynamic_cast<CUI_ScreenText*>(m_pGameInstance->Get_LastGameObject(ENUM_CLASS(LEVEL::CH09_FIGHTC), strLayerTag + L"_ScreenText"));
	CLevel_Stage::m_pUI_ScreenText = m_pUI_ScreenText;

	return S_OK;
}

HRESULT CStage_09FightC::Ready_Layer_Player(const _wstring& strLayerTag)
{
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::CH09_FIGHTC), strLayerTag,
		ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Player"))))
		return E_FAIL;
	CGameObject* pPlayer = m_pGameInstance->Get_LastGameObject(ENUM_CLASS(LEVEL::CH09_FIGHTC), strLayerTag);

	_float4 pPos = { 77.f, 0.5f, 178.f, 1.f };
	dynamic_cast<CTransform*>(pPlayer->Get_Component(L"Com_Transform"))->Set_State(STATE::POSITION, XMLoadFloat4(&pPos));
	dynamic_cast<CTransform*>(pPlayer->Get_Component(L"Com_Transform"))->Rotation(XMVectorSet(0.f, 1.f, 0.f, 1.f), TO_RAD(180));

	return S_OK;
}

HRESULT CStage_09FightC::Ready_Layer_Monster(const _wstring& strLayerTag)
{
	// ksta : Enemy는 모든 레벨에서 사용할 것이라, 프로토타입은 Static으로 주는 게 좋을 듯.
	// 이 부분은 레이어 추가 부분이기에 이미 선언된 프로토타입을 이용하는 것임.

	//if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), strLayerTag,
	//	ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Monster"))))
	//	return E_FAIL;

	CEnemy::ENEMY_DESC EnemyDesc{};

	EnemyDesc.fRotationPerSec = XMConvertToRadians(180.f);
	EnemyDesc.fSpeedPerSec = 15.f;
	//EnemyDesc.iDefaultWeaponObjType = ENUM_CLASS(GAMEOBJ_TYPE::WEAPON_RANGED_PISTOL);

	vector<_uint> vecEnemyDefaultWeaponTypes = {
		ENUM_CLASS(GAMEOBJ_TYPE::WEAPON_RANGED_PISTOL),
		ENUM_CLASS(GAMEOBJ_TYPE::END),
		ENUM_CLASS(GAMEOBJ_TYPE::END),
		ENUM_CLASS(GAMEOBJ_TYPE::END),
		ENUM_CLASS(GAMEOBJ_TYPE::END),
		ENUM_CLASS(GAMEOBJ_TYPE::END)
	};

	vector<_float4> vecEnemyPos = {
		_float4{ 83.2244f, 0.499977f, 71.7733f, 1.f},
		_float4{ 74.2833f, 0.499982f, 90.4307f , 1.f},
		_float4{ 83.946f, 0.499993f, 119.088f , 1.f},
		_float4{ 70.7839f, 0.500004f, 148.576f , 1.f},
		_float4{ 64.0938f, 0.500011f, 165.41f , 1.f},
		_float4{ 89.4751f, 0.500007f, 158.567f, 1.f }
	};

	vector<_uint> vecEnemyNavIndices = { 7, 6, 7, 3, 4, 1 };



	for (_uint i = 0; i < vecEnemyPos.size(); i++)
	{
		EnemyDesc.iFirstCellIndex = vecEnemyNavIndices[i];
		EnemyDesc.iDefaultWeaponObjType = vecEnemyDefaultWeaponTypes[i];

		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::CH09_FIGHTC), strLayerTag,
			ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Enemy"), &EnemyDesc)))
			return E_FAIL;
		CGameObject* pEnemy = m_pGameInstance->Get_LastGameObject(ENUM_CLASS(LEVEL::CH09_FIGHTC), strLayerTag);

		dynamic_cast<CTransform*>(pEnemy->Get_Component(L"Com_Transform"))->Set_State(STATE::POSITION, XMLoadFloat4(&vecEnemyPos[i]));
		dynamic_cast<CTransform*>(pEnemy->Get_Component(L"Com_Transform"))->Rotation(XMVectorSet(0.f, 1.f, 0.f, 1.f), TO_RAD(180));
	}



	return S_OK;
}

HRESULT CStage_09FightC::Ready_Layer_Effect(const _wstring& strLayerTag)
{
	//if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::CH09_FIGHTC), strLayerTag,
	//	ENUM_CLASS(LEVEL::CH09_FIGHTC), TEXT("Prototype_GameObject_Particle_TriEffect"))))
	//	return E_FAIL;

	_uint iDestLevel = m_pGameInstance->Get_DestLevel();

	//if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(iDestLevel, L"Layer_Particle_HitEffect",
	//	iDestLevel, TEXT("Prototype_GameObject_Particle_HitEffect"))))
	//	return E_FAIL;

	return S_OK;
}

HRESULT CStage_09FightC::Ready_Pickup_Objects(const _wstring& strLayerTag)
{
	_wstring strLoadedTag = L"Layer_Loaded_Object";		// 로드된 오브젝트들이 담긴 레이어 태그

	_uint iIndex = 0;
	_uint iDestLevel = m_pGameInstance->Get_DestLevel();

	// 현재 맵에 무기 오브젝트들이 없는 경우에는 해당 모델이 로드되어 있지 않음으로 인한 문제가 생기므로 미리 로드
	_matrix		PreTransformMatrix = XMMatrixIdentity();
	PreTransformMatrix = XMMatrixRotationY(XMConvertToRadians(180.0f));

	if FAILED(Add_Prototype_Direct(iDestLevel, L"Weapon_Karabin_Fixed", &PreTransformMatrix))
		std::cout << "[CStage_09FightC::Ready_Pickup_Objects] Prototype Create Failed. (Weapon_Karabin_Fixed)" << std::endl;
	if FAILED(Add_Prototype_Direct(iDestLevel, L"Weapon_Pistol_Fixed", &PreTransformMatrix))
		std::cout << "[CStage_09FightC::Ready_Pickup_Objects] Prototype Create Failed. (Weapon_Pistol_Fixed)" << std::endl;
	if FAILED(Add_Prototype_Direct(iDestLevel, L"Weapon_Shotgun_Fixed", &PreTransformMatrix))
		std::cout << "[CStage_09FightC::Ready_Pickup_Objects] Prototype Create Failed. (Weapon_Shotgun_Fixed)" << std::endl;

	
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


CStage_09FightC* CStage_09FightC::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CStage_09FightC* pInstance = new CStage_09FightC(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CStage_09FightC"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CStage_09FightC::Free()
{
	__super::Free();



}

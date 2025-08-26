#include "Stage_Test1.h"

#include "GameInstance.h"
#include "Camera_Free.h"
//#include "Camera_Player.h"
#include "Terrain.h"

CStage_Test1::CStage_Test1(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel_Stage{ pDevice, pContext }
{
}

HRESULT CStage_Test1::Initialize()
{
	if (FAILED(Ready_Lights()))
		return E_FAIL;
	  
	/* 현재 레벨을 구성해주기 위한 객체들을 생성한다. */
	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
		return E_FAIL;
	
	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
		return E_FAIL;
	
	if (FAILED(Ready_Layer_Player(TEXT("Layer_Player"))))
		return E_FAIL;
	
	if (FAILED(Ready_Layer_Monster(TEXT("Layer_Monster"))))
		return E_FAIL;
	
	if (FAILED(Ready_Layer_Effect(TEXT("Layer_Effect"))))
		return E_FAIL;

	_wstring strLoadPath = L"../Bin/Resources/_SUPERHOT/_BinaryLevels/Stage_Test1.datmap";
	if (FAILED(Load_BinaryMap(&strLoadPath)))
		return E_FAIL;



	return S_OK;
}

void CStage_Test1::Update(_float fTimeDelta)
{

}

HRESULT CStage_Test1::Render()
{
	SetWindowText(g_hWnd, TEXT("Level : Stage_Test1 (Temp)"));

	return S_OK;
}

HRESULT CStage_Test1::Ready_Lights()
{
	LIGHT_DESC			LightDesc{};

	//(LightDesc.Diffuse * MtrlDesc.Diffuse) * (fShade(0 ~ 1) + (LightDesc.Ambient * MtrlDesc.Ambient))

	LightDesc.eType = LIGHT_DESC::TYPE::DIRECTIONAL;
	LightDesc.vDirection = _float4(1.f, -1.f, 1.f, 0.f);	// Light 방향
	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);		// Light 색상 및 밝기의 세기
	LightDesc.vAmbient = _float4(0.4f, 0.4f, 0.4f, 1.f);	// Light 환경광으로 가정. 최소 밝기 보장에 관여.
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);		// Light 반사광.


	if (FAILED(m_pGameInstance->Add_Light(LightDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CStage_Test1::Ready_Layer_Camera(const _wstring& strLayerTag)
{
	CCamera_Free::CAMERA_FREE_DESC		CameraDesc{};
	CameraDesc.vEye = _float4(0.f, 20.f, -15.f, 1.f);
	CameraDesc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
	CameraDesc.fFovy = XMConvertToRadians(60.0f);
	CameraDesc.fNear = 1.f; // 0.1f;
	CameraDesc.fFar = 500.f;
	CameraDesc.fSpeedPerSec = 10.f;
	CameraDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	CameraDesc.fMouseSensor = .2f;

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::TEST_EXTRA1), strLayerTag,
		ENUM_CLASS(LEVEL::TEST_EXTRA1), TEXT("Prototype_GameObject_Camera_Player"), &CameraDesc)))
		return E_FAIL;

	//if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), strLayerTag,
	//	ENUM_CLASS(LEVEL::TEST_EXTRA1), TEXT("Prototype_GameObject_Camera_Player"), &CameraDesc)))
	//	return E_FAIL;

	return S_OK;
}

HRESULT CStage_Test1::Ready_Layer_BackGround(const _wstring& strLayerTag)
{
	//if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::TEST_EXTRA1), strLayerTag,
	//	ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Terrain"))))
	//	return E_FAIL; .// !! 필요없을듯>

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::TEST_EXTRA1), strLayerTag,
		ENUM_CLASS(LEVEL::TEST_EXTRA1), TEXT("Prototype_GameObject_Skybox"))))
		return E_FAIL; // !!

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::TEST_EXTRA1), strLayerTag,
		ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_UI_Crosshair"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CStage_Test1::Ready_Layer_Player(const _wstring& strLayerTag)
{
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::TEST_EXTRA1), strLayerTag,
		ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Player"))))
		return E_FAIL;
	CGameObject* pPlayer = m_pGameInstance->Get_LastGameObject(ENUM_CLASS(LEVEL::TEST_EXTRA1), strLayerTag);

	_float3 pPos = {86.f, 0.f, 60.f};
	dynamic_cast<CTransform*>(pPlayer->Get_Component(L"Com_Transform"))->Set_State(STATE::POSITION, XMLoadFloat3(&pPos));

	return S_OK;
}

HRESULT CStage_Test1::Ready_Layer_Monster(const _wstring& strLayerTag)
{
	// ksta : Enemy는 모든 레벨에서 사용할 것이라, 프로토타입은 Static으로 주는 게 좋을 듯.
	// 이 부분은 레이어 추가 부분이기에 이미 선언된 프로토타입을 이용하는 것임.

	//if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), strLayerTag,
	//	ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Monster"))))
	//	return E_FAIL;

	CGameObject::GAMEOBJECT_DESC GameObjDesc{};

	GameObjDesc.fRotationPerSec = XMConvertToRadians(90.f);
	GameObjDesc.fSpeedPerSec = 1.f;

	//if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::GAMEPLAY), strLayerTag,
	//	ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Enemy"), &GameObjDesc)))
	//	return E_FAIL;

	return S_OK;
}

HRESULT CStage_Test1::Ready_Layer_Effect(const _wstring& strLayerTag)
{

	return S_OK;
}

CStage_Test1* CStage_Test1::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CStage_Test1* pInstance = new CStage_Test1(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CStage_Test1"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CStage_Test1::Free()
{
	__super::Free();



}

#include "Level_Editor.h"

#include "ImGui_Manager.h"
//#include "ImGuiFileDialog.h"
//#include "ImGuiFileDialogConfig.h"

#include "GameInstance.h"
#include "Layer.h"
#include "GameObject.h"
#include "Terrain.h"
//#include "Client_Struct.h"
//#include "Camera_Free.h"
#include "Camera_Editor.h"

#include "Level_Loading.h"

CLevel_Editor::CLevel_Editor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_Editor::Initialize()
{
	if (FAILED(Ready_Lights()))
		return E_FAIL;

	if (FAILED(Ready_ImGui(g_hWnd, m_pDevice, m_pContext)))
		return E_FAIL;

	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
		return E_FAIL;

	//if (FAILED(Ready_Terrain_Texture_Info()))
	//	return E_FAIL;

	//if (FAILED(Ready_Texture_Info()))
	//	return E_FAIL;

	//if (FAILED(Ready_Interaction_Texture_Info()))
	//	return E_FAIL;

	return S_OK;
}

void CLevel_Editor::Update(_float fTimeDelta)
{
	if (!ImGui::GetIO().WantCaptureMouse)
		Picking_Check();

	//if (isOn_DeployMode)
		//Deploy_Object();

}

HRESULT CLevel_Editor::Render()
{
	SetWindowText(g_hWnd, TEXT("Level : Editor"));
	//m_pImgui_Manage->Render();
	this->ImGui_Render();

	return S_OK;
}

HRESULT CLevel_Editor::Ready_Lights()
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

HRESULT CLevel_Editor::Ready_ImGui(HWND hWnd, ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{

	m_pImGui_Manager = CImGui_Manager::Create(hWnd, pDevice, pContext);
	if (m_pImGui_Manager == nullptr)
	{
		MSG_BOX(TEXT("Failed to Created : CImgui_Manager"));
		return E_FAIL;
	}
	//MessageBox(0, TEXT("Imgui 매니저 생성 성공"), TEXT("Debug"), MB_OK);
	return S_OK;
}

HRESULT CLevel_Editor::Ready_Layer_Camera(const _wstring& strLayerTag)
{
	// ksta : 마우스 카메라가 있어야 제대로 될 듯. 아래는 임시용
	CCamera_Editor::CAMERA_EDITOR_DESC		CameraDesc{};

	CameraDesc.vEye = _float4(0.f, 20.f, -15.f, 1.f);
	CameraDesc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
	CameraDesc.fFovy = XMConvertToRadians(60.0f);
	CameraDesc.fNear = 0.1f;
	CameraDesc.fFar = 500.f;
	CameraDesc.fSpeedPerSec = 20.f;
	CameraDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	CameraDesc.fMouseSensor = .2f;

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::EDITOR), strLayerTag,
		ENUM_CLASS(LEVEL::EDITOR), TEXT("Prototype_GameObject_Camera_Editor"), &CameraDesc)))
		return E_FAIL;

	return S_OK;
}

void CLevel_Editor::ImGui_Render()
{
	m_pImGui_Manager->GUI_Render_Begin();
	// 그리기 시작


	ImGui_MainMenu();				// 최상단 메뉴

	if (isOn_GUITerrainEditor)
		ImGui_TerrainEditor();		// 터레인 에디터

	if (isOn_ModelDeployer)
		ImGui_ModelDeployer();		// 메쉬 배치기


	// 그리기 끝
	m_pImGui_Manager->GUI_Render_End();
}

void CLevel_Editor::ImGui_MenuBar_Render()
{
	//if (ImGui::BeginMenuBar())
	//{
	//}
}

void CLevel_Editor::Picking_Check()
{
	if (m_pGameInstance->Get_IsKeyDown(MOUSEKEYSTATE::LB))
		m_isPicking = true;
	else
		m_isPicking = false;
}

CLevel_Editor* CLevel_Editor::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_Editor* pInstance = new CLevel_Editor(pDevice, pContext);
	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CLevel_Editor"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Editor::Free()
{
	if (m_pImGui_Manager)
	{
		delete m_pImGui_Manager;
		m_pImGui_Manager = nullptr;
	}

	__super::Free();
}


// ==============================
// || Custom GUI Windows..
// ==============================

void CLevel_Editor::ImGui_MainMenu()
{
	// 메인 창
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Open..", "Ctrl+O")) { /* Do stuff */ }
			if (ImGui::MenuItem("Save", "Ctrl+S")) { /* Do stuff */ }
			//if (ImGui::MenuItem("Close", "Ctrl+W")) { isOn_GUITerrain = false; }
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Window"))
		{
			if (ImGui::MenuItem("Terrain Editor", nullptr))
				isOn_GUITerrainEditor = !isOn_GUITerrainEditor;
			if (ImGui::MenuItem("Model Deployer", nullptr))
				isOn_ModelDeployer = !isOn_ModelDeployer;

			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}
}

void CLevel_Editor::ImGui_TerrainEditor()
{
	// 터레인 에디터
	ImGui::Begin("Terrain Editor", &isOn_GUITerrainEditor);

	static _float	fPosX = 0;
	static _float	fPosY = 0;
	static _float	fPosZ = 0;
	
	static _int		iSize = 1024;
	_float			fTerrainDefaultSize = 1024.f;

	_float			fMultiplier = iSize / fTerrainDefaultSize;



	if (ImGui::BeginMenu("Edit"))
	{
		if (ImGui::MenuItem("Reset Position"))	{ fPosX = 0.f; fPosY = 0.f; fPosZ = 0.f; }
		if (ImGui::MenuItem("Reset Size"))		{ iSize = 1024; }

		ImGui::EndMenu();
	}
	ImGui::Separator();

	ImGui::Text("Create Menu");
	ImGui::Separator();
	

	// Position
	ImGui::BeginGroup();

	ImGui::PushItemWidth(60);
	ImGui::Text("Position");

	ImGui::DragFloat("X##pos", &fPosX, 0.1f);
	ImGui::SameLine();
	ImGui::DragFloat("Y##pos", &fPosY, 0.1f);
	ImGui::SameLine();
	ImGui::DragFloat("Z##pos", &fPosZ, 0.1f);
	ImGui::PopItemWidth();

	ImGui::EndGroup();
	ImGui::Separator();


	// Size
	ImGui::BeginGroup();

	ImGui::PushItemWidth(90);
	ImGui::Text("Size");

	ImGui::DragInt("X##Size", &iSize, 1.0f, 0, 10000);
	ImGui::SameLine();
	ImGui::DragInt("Z##Size", &iSize, 1.0f, 0, 10000);
	ImGui::PopItemWidth();

	ImGui::EndGroup();
	ImGui::Separator();
	

	// Create
	static _int iIndex = 0;

	if (ImGui::Button("Create Terrain"))
	{
		// 생성 코드..
		_wstring strLayerTag = L"Layer_Editor_Terrain";
		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::EDITOR), strLayerTag,
			ENUM_CLASS(LEVEL::EDITOR), TEXT("Prototype_GameObject_Terrain"))))
			return;

		CGameObject* pGameObject = m_pGameInstance->Get_LastGameObject(ENUM_CLASS(LEVEL::EDITOR), strLayerTag);
		m_pTerrainObject.push_back(pGameObject);

		_float fTiling = 50.f * (iSize / fTerrainDefaultSize);
		CShader* pShaderCom = static_cast<CShader*>(m_pGameInstance->Find_Component(ENUM_CLASS(LEVEL::EDITOR), strLayerTag, L"Com_Shader", iIndex));
		//if (FAILED(pShaderCom->Bind_RawValue("g_fTiling", &fTiling, sizeof(_float))))
		//	return;
		
		CTransform* pTransform = static_cast<CTransform*>(m_pGameInstance->Find_Component(ENUM_CLASS(LEVEL::EDITOR), strLayerTag, L"Com_Transform", iIndex));
		pTransform->Scaling(_float3(fMultiplier, fMultiplier, fMultiplier));
		pTransform->Set_State(STATE::POSITION, XMVectorSet(fPosX, fPosY, fPosZ, 1));
		
		iIndex++;
	}
	

	//ImGui::Separator();
	//if (ImGui::Button("Undo"))
	//	if (!m_pTerrainObject.empty())
	//	{
	//		m_pGameInstance->Remove_LastGameObject(ENUM_CLASS(LEVEL::EDITOR), L"Layer_Editor_Terrain");
	//		m_pTerrainObject.pop_back();
	//	}
	//if (ImGui::CollapsingHeader("Danger Section"))
	//{
	//	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
	//	if (ImGui::Button("All Reset"))
	//	{
	//		for (size_t i = 0; i < m_pTerrainObject.size(); i++)
	//			m_pGameInstance->Remove_LastGameObject(ENUM_CLASS(LEVEL::EDITOR), L"Layer_Editor_Terrain");
	//		m_pTerrainObject.clear();
	//	}
	//	ImGui::PopStyleColor();
	//}

	// 오브젝트 수정 기능은 나중에 만들어도 됨
	//ImGui::Separator();

	//if (iIndex >= 1)
	//{

	//}


	ImGui::End();
}

void CLevel_Editor::ImGui_ModelDeployer()
{
	// 모델 배치기
	ImGui::Begin("Model Deployer", &isOn_ModelDeployer);

	ImGui::Text("Create Menu");
	ImGui::Separator();

	ImGui::BeginGroup();
	const char* szItems[] = { "Enemy", "Props_Pot", "Props_Fotel", "Props_ServerRack1", "Props_ServerRack2" };
	static int iCurrentItem = 0;
	ImGui::Text("Selected Model");
	ImGui::Combo("##Selected Model", &iCurrentItem, szItems, IM_ARRAYSIZE(szItems));
	ImGui::Separator();

	if (!isOn_DeployMode) {
		if (ImGui::Button("Active Deploy Mode"))
			isOn_DeployMode = true;
	}
	else {
		if (ImGui::Button("Deactive Deploy Mode"))
			isOn_DeployMode = false;
	}


	static _float3 vDebugPos = {}; //

	// 클릭하면 모델 설치
	if (isOn_DeployMode && m_pGameInstance->Get_IsKeyDown(MOUSEKEYSTATE::LB) && m_isPicking)
	{
		switch (iCurrentItem)
		{
		case 0:
		{
			m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::EDITOR), L"Layer_Editor_Object",
				ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Enemy"));
			break;
		}
		case 1:
		{
			m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::EDITOR), L"Layer_Editor_Object",
				ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Props_Pot"));
			break;
		}
		case 2:
		{
			m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::EDITOR), L"Layer_Editor_Object",
				ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Props_Fotel"));
			break;
		}
		case 3:
		{
			m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::EDITOR), L"Layer_Editor_Object",
				ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Props_ServerRack1"));
			break;
		}
		case 4:
		{
			m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::EDITOR), L"Layer_Editor_Object",
				ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Props_ServerRack2"));
			break;
		}
		default:
			break;
		}

		CGameObject* pGameObject = m_pGameInstance->Get_LastGameObject(ENUM_CLASS(LEVEL::EDITOR), L"Layer_Editor_Object");
		if (!m_pTerrainObject.empty())
		{
			CTerrain* pTerrain = dynamic_cast<CTerrain*>(m_pTerrainObject.back());

			_float3 vPos = {};
			pTerrain->isPicked(&vPos);
			CTransform* pObjectTransformCom = dynamic_cast<CTransform*>(pGameObject->Get_Component(L"Com_Transform"));
			pObjectTransformCom->Scale(_float3{ 10, 10, 10 }); // 임시로 크기 키움
			pObjectTransformCom->Set_State(STATE::POSITION, XMLoadFloat3(&vPos));

			vDebugPos = vPos; //
		}
		m_pObject.push_back(pGameObject);


	}

	ImGui::Text("Pos Debug");
	ImGui::DragFloat3("##vPos", reinterpret_cast<float*>(&vDebugPos), 0.01f);

	//ImGui::Separator();
	//if (ImGui::Button("Undo"))
	//	if (!m_pObject.empty())
	//	{
	//		m_pGameInstance->Remove_LastGameObject(ENUM_CLASS(LEVEL::EDITOR), L"Layer_Editor_Object");
	//		m_pObject.pop_back();
	//	}
	//if (ImGui::CollapsingHeader("Danger Section"))
	//{
	//	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
	//	if (ImGui::Button("All Reset"))
	//	{
	//		for (size_t i = 0; i < m_pObject.size(); i++)
	//			m_pGameInstance->Remove_LastGameObject(ENUM_CLASS(LEVEL::EDITOR), L"Layer_Editor_Object");
	//		m_pObject.clear();
	//	}
	//	ImGui::PopStyleColor();
	//}



	ImGui::EndGroup();

	ImGui::End();
}


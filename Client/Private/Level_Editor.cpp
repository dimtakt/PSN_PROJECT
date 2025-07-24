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
	if (!ImGui::GetIO().WantCaptureMouse)	// m_isNotUsingUI 제어
		Check_NotUsingUI();

	if (m_isNotUsingUI && !isOn_DeployMode &&
		m_pGameInstance->Get_IsKeyDown(MOUSEKEYSTATE::LB))
		Check_ObjectPicking();


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

	// 오브젝트를 선택한 상태면 컴포넌트 뷰어 창이 뜨고,
	// 그 하위의 컴포넌트 에디터 창을 띄우는 식으로?

	//if (isObject_Selected)
		ImGui_Inspector();
	//if (isOn_ComViewer_Transform)
	//	ImGui_ComViewer_Transform();


	// 그리기 끝
	m_pImGui_Manager->GUI_Render_End();
}

void CLevel_Editor::ImGui_MenuBar_Render()
{
	//if (ImGui::BeginMenuBar())
	//{
	//}
}

void CLevel_Editor::Check_NotUsingUI()
{
	if (m_pGameInstance->Get_IsKeyDown(MOUSEKEYSTATE::LB))
		m_isNotUsingUI = true;
	else
		m_isNotUsingUI = false;
}

_bool CLevel_Editor::Check_ObjectPicking()
{
	if (m_pObject.empty())
	{
		pSelectedObject = nullptr;
		return false;
	}

	// 커서와 겹친 모든 오브젝트를 확인, 컨테이너에 담음.
	// 단일 오브젝트만 겹쳤다면 즉시 할당 및 리턴
	// 겹친 오브젝트가 없다면 즉시 nullptr, 리턴
	vector<CGameObject*> pPickedObjects = {};

	for (auto object : m_pObject)
		if (object->isPicked())
			pPickedObjects.push_back(object);

	if (pPickedObjects.size() == 1)
	{
		pSelectedObject = pPickedObjects[0];
		return true;
	}
	else if (pPickedObjects.empty())
	{
		pSelectedObject = nullptr;
		return false;
	}

	// 겹친 오브젝트들 중 가장 가까운 오브젝트와
	// 두 번째로 가까운 오브젝트를 찾음
	vector<_float> vecLengthOrigin = {};
	vector<_float> vecLength = {};
	const _float4* vCamPos = m_pGameInstance->Get_CamPosition();

	for (auto object : pPickedObjects)
	{
		_vector vObjPos = dynamic_cast<CTransform*>(object->Get_Component(L"Com_Transform"))->Get_State(STATE::POSITION);
		_vector vCamPosLoad = XMLoadFloat4(vCamPos);

		_float4 vDiff;
		XMStoreFloat4(&vDiff, XMVector3LengthSq(vObjPos - vCamPosLoad));

		vecLengthOrigin.push_back(vDiff.x);
		vecLength.push_back(vDiff.x);
	}

	_int iNearObjIndex = 0, iNextObjIndex = 0;
	
	sort(vecLength.begin(), vecLength.end());
	for (_int i = 0; i < vecLengthOrigin.size(); i++)
	{ 
		if			(vecLengthOrigin[i] == vecLength[0]) iNearObjIndex = i; 
		else if		(vecLengthOrigin[i] == vecLength[1]) iNextObjIndex = i;
	}

	// 가장 가까운 오브젝트를 할당하되,
	// 이미 선택된 오브젝트라면 두 번째로 가까운 오브젝트 할당
	if (pSelectedObject == nullptr ||
		pSelectedObject != pPickedObjects[iNearObjIndex])
		pSelectedObject = pPickedObjects[iNearObjIndex];
	else
		pSelectedObject = pPickedObjects[iNextObjIndex];

	return true;
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
		
		iIndex++; // 이거 안내려서 문제생긴듯
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

#pragma region Model Deploy UI

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

#pragma endregion

#pragma region Model Deploy Logic & Button & Debug

	static _float3 vPickedPos = {}; //
	static _int iObjIndex = 0;

	// 클릭하면 모델 설치
	if (isOn_DeployMode && m_pGameInstance->Get_IsKeyDown(MOUSEKEYSTATE::LB) && m_isNotUsingUI)
	{
		// ksta : 선택한 터레인에 생성되도록 변경? 아니면 터레인 갯수제한을 1로 두거나
		CTerrain* pTerrain = dynamic_cast<CTerrain*>(m_pTerrainObject.back());

		if (pTerrain->isPicked(&vPickedPos)) // 이거 false 뜨면 생성 안되게
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
				CTransform* pObjectTransformCom = dynamic_cast<CTransform*>(pGameObject->Get_Component(L"Com_Transform"));
				pObjectTransformCom->Scale(_float3{ 10, 10, 10 }); // 임시로 크기 키움
				pObjectTransformCom->Set_State(STATE::POSITION, XMVectorSet(vPickedPos.x, vPickedPos.y, vPickedPos.z, 1));
			}
			m_pObject.push_back(pGameObject);

			iObjIndex++; // 이거 안내려서 문제생긴듯
		}

		ImGui::Text("Pos Debug");
		ImGui::DragFloat3("##vPos", reinterpret_cast<float*>(&vPickedPos), 0.01f);

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

	}
	ImGui::EndGroup();

#pragma endregion

	ImGui::End();
}

void CLevel_Editor::ImGui_Inspector()
{
	// 트랜스폼 컴포넌트 존재여부 검사.
	if (pSelectedObject == nullptr)
	{
		isOn_ComViewer_Transform = false;
		isObject_Selected = false;
		return;
	}

	ImGui::Begin("Inspector");

	CTransform* pTransformCom = dynamic_cast<CTransform*>(pSelectedObject->Get_Component(L"Com_Transform"));
	isOn_ComViewer_Transform = (pTransformCom == nullptr)? false : true;
	
#pragma region Inspector : Transform UI

	// 선택한 오브젝트가 Transform 컴포넌트가 있을 때 보여짐.
	if (isOn_ComViewer_Transform)
	{
		static _float3 vSelectedObjPos = {};
		static _float3 vSelectedObjRot = {};	// 보일 각도는 오일러, degree 기준. 내부적으로는 radian 변환 후 쿼터니언 처리.
		static _float3 vSelectedObjSca = {};

		if (ImGui::CollapsingHeader("Transform"))
		{	
			if (ImGui::BeginMenu("Reset Menu"))
			{
				if (ImGui::MenuItem("Reset Position"))	{ vSelectedObjPos = { 0.f, 0.f, 0.f }; }
				if (ImGui::MenuItem("Reset Rotation"))	{ vSelectedObjRot = { 0.f, 0.f, 0.f }; }
				if (ImGui::MenuItem("Reset Scale"))		{ vSelectedObjSca = { 1.f, 1.f, 1.f }; }
				ImGui::Separator();
				if (ImGui::MenuItem("Reset Transform"))	{ vSelectedObjPos = { 0.f, 0.f, 0.f };
														  vSelectedObjRot = { 0.f, 0.f, 0.f };
														  vSelectedObjSca = { 1.f, 1.f, 1.f }; }
				ImGui::EndMenu();
			}
			ImGui::Separator();

			ImGui::PushItemWidth(60);

			// Position Ctrl
			ImGui::Text("Position");

			ImGui::DragFloat("X##pos", &vSelectedObjPos.x, 0.1f);
			ImGui::SameLine();
			ImGui::DragFloat("Y##pos", &vSelectedObjPos.y, 0.1f);
			ImGui::SameLine();
			ImGui::DragFloat("Z##pos", &vSelectedObjPos.z, 0.1f);

			ImGui::Separator();

			// Rotation Ctrl
			ImGui::Text("Position");

			ImGui::DragFloat("X##rot", &vSelectedObjRot.x, 0.1f);
			ImGui::SameLine();
			ImGui::DragFloat("Y##rot", &vSelectedObjRot.y, 0.1f);
			ImGui::SameLine();
			ImGui::DragFloat("Z##rot", &vSelectedObjRot.z, 0.1f);

			ImGui::Separator();

			// Scale Ctrl
			ImGui::Text("Scale");

			ImGui::DragFloat("X##sca", &vSelectedObjSca.x, 0.1f);
			ImGui::SameLine();
			ImGui::DragFloat("Y##sca", &vSelectedObjSca.y, 0.1f);
			ImGui::SameLine();
			ImGui::DragFloat("Z##sca", &vSelectedObjSca.z, 0.1f);

			ImGui::Separator();

			ImGui::PopItemWidth();
		}
	}

#pragma endregion

	ImGui::End();
}


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

#include "CustomObj_NonAnim.h"
#include "CustomObj_Anim.h"


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
	m_pPrevSelectedObject = m_pSelectedObject;

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

	// Selected Objects Render DebugLine
	//if (m_pSelectedObject)
	//{
	//	CModel* pSelectedObjModelCom = dynamic_cast<CModel*>(m_pSelectedObject->Get_Component(L"Com_Model"));
	//	vector<CMesh*> pSelectedObjMeshes = pSelectedObjModelCom->Get_Meshes();
	//
	//	for (size_t i = 0; i < pSelectedObjMeshes.size(); i++)
	//		pSelectedObjMeshes[i]->Render_DebugLine();
	//}

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
		m_pSelectedObject = nullptr;
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
		m_pSelectedObject = pPickedObjects[0];
		return true;
	}
	else if (pPickedObjects.empty())
	{
		m_pSelectedObject = nullptr;
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
	if (m_pSelectedObject == nullptr ||
		m_pSelectedObject != pPickedObjects[iNearObjIndex])
		m_pSelectedObject = pPickedObjects[iNearObjIndex];
	else
		m_pSelectedObject = pPickedObjects[iNextObjIndex];
	
	return true;
}

_bool CLevel_Editor::LoadExternalFile(FILETYPE eFileType, _wstring* strPathOut)
{
	OPENFILENAMEW ofn = {};
	_tchar szFile[260] = { 0 };

	ofn.lStructSize = sizeof(ofn);
	ofn.lpstrFile = szFile;
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter;
	ofn.nFilterIndex = 1;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
	
	switch (eFileType)
	{
	case FILETYPE::FBX:
		ofn.lpstrFilter = L"FBX Files (*.fbx)\0*.fbx\0";
		break;
	case FILETYPE::DATMODEL:
		ofn.lpstrFilter = L"Binary Model Files (*.datmodel)\0*.datmodel\0";
		break;
	case FILETYPE::DATMAP:
		ofn.lpstrFilter = L"Binary Map Files (*.datmap)\0*.datmap\0";
		break;

	case FILETYPE::FILETYPE_END:
	default:
		MessageBoxW(NULL, L"Wrong Type", L"잘못된 파일 형식 로드 시도. \nLevel_Editor::LoadExternalFile()", MB_OK);
		return false;
	}
	

	_bool isLoaded = false;
	isLoaded = GetOpenFileNameW(&ofn);

	if (isLoaded)		*strPathOut = ofn.lpstrFile;
	else				*strPathOut = L"";

	return isLoaded;
}

_bool CLevel_Editor::SaveExternalFile(FILETYPE eFileType, _wstring* strPathOut)
{
	OPENFILENAMEW ofn = {};
	_tchar szFile[260] = { 0 };

	ofn.lStructSize = sizeof(ofn);
	ofn.lpstrFile = szFile;
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter;
	ofn.nFilterIndex = 1;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT;

	switch (eFileType)
	{
	case FILETYPE::FBX:
		ofn.lpstrFilter = L"FBX Files (*.fbx)\0*.fbx\0";
		break;
	case FILETYPE::DATMODEL:
		ofn.lpstrFilter = L"Binary Model Files (*.datmodel)\0*.datmodel\0";
		break;
	case FILETYPE::DATMAP:
		ofn.lpstrFilter = L"Binary Map Files (*.datmap)\0*.datmap\0";
		break;

	case FILETYPE::FILETYPE_END:
	default:
		MessageBoxW(NULL, L"Wrong Type", L"잘못된 파일 형식 저장 시도. Level_Editor::SaveExternalFile()", MB_OK);
		return false;
	}


	_bool isSaved = false;
	isSaved = GetSaveFileNameW(&ofn);

	if (isSaved)		*strPathOut = ofn.lpstrFile;
	else				*strPathOut = L"";

	return isSaved;
}

HRESULT CLevel_Editor::Convert_FBXToBinary(_wstring* strLoadPath, _wstring* strSavePath, MODELTYPE eAnimType)
{
	// 어찌할꼬..
	// 일단 NONANIM만 만들기
	//if (eAnimType == MODELTYPE::ANIM)
	//	return E_FAIL;

	// 문자열 호환안되는거 변환
	_char szLoadPath[256] = { 0 };
	_char szSavePath[256] = { 0 };
	WideCharToMultiByte(CP_ACP, 0, (*strLoadPath).c_str(), -1, szLoadPath, 256, nullptr, nullptr);
	WideCharToMultiByte(CP_ACP, 0, (*strSavePath).c_str(), -1, szSavePath, 256, nullptr, nullptr);

	// 일단 모델 로드해봐
	_matrix		PreTransformMatrix = XMMatrixIdentity();
	PreTransformMatrix = XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EDITOR), TEXT("Prototype_Component_Model_Custom"),
		CModel::Create(m_pDevice, m_pContext, eAnimType, szLoadPath, PreTransformMatrix))))
		return E_FAIL;

	// 바이너리화 후 저장...
	// 실질 구현부는 Model.cpp 에 존재
	CModel* pTargetModel = dynamic_cast<CModel*> (m_pGameInstance->Find_Prototype(ENUM_CLASS(LEVEL::EDITOR), TEXT("Prototype_Component_Model_Custom")));
	pTargetModel->Export_ToBinary(strSavePath);

	// 다 됐으면 제거해
	if (FAILED(m_pGameInstance->Remove_Prototype(ENUM_CLASS(LEVEL::EDITOR), TEXT("Prototype_Component_Model_Custom"))))
		return E_FAIL;

	return S_OK;
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
			ImGui::Separator();
			if (ImGui::BeginMenu("Open File.."))
			{
				if (ImGui::MenuItem("[Binary] Model"))
				{
					wstring strLoadFilePath = L"";
					_bool isLoaded = false;

					isLoaded = LoadExternalFile(FILETYPE::DATMODEL, &strLoadFilePath);

					if (isLoaded)
					{
						// 받아온 경로 문자열을 이용하여 바이너리 모델 로드 진행.
						// 경로를 char로 변환, 확장자 추출, 확장자는 데/  이터 추출 후 가져옴.
						// 파일명을 프로토타입의 suffix 로써 사용하여 프로토타입의 중복을 방지..
						_char		szLoadPath[256] = {};
						WideCharToMultiByte(CP_ACP, 0, (strLoadFilePath).c_str(), -1, szLoadPath, 256, nullptr, nullptr);

						_char		szExt[MAX_PATH] = {};
						_char		szFileName[MAX_PATH] = {};
						_splitpath_s(szLoadPath, nullptr, 0, nullptr, 0, szFileName, MAX_PATH, szExt, MAX_PATH);
						
						_matrix		PreTransformMatrix = XMMatrixIdentity();
						PreTransformMatrix = XMMatrixRotationY(XMConvertToRadians(180.0f));

						_wstring	strModelPrototypeName = L"Prototype_Component_Model_Custom_";
						_wstring	strObjectPrototypeName = L"Prototype_GameObject_Model_Custom_";
						_int		len = MultiByteToWideChar(CP_ACP, 0, szFileName, -1, nullptr, 0);
						_wstring	strFileName(len, 0);
						MultiByteToWideChar(CP_ACP, 0, szFileName, -1, &strFileName[0], len);
						strFileName.pop_back(); // null 문자 제거
						strModelPrototypeName += strFileName;
						strObjectPrototypeName += strFileName;

						// 이후 로드는 CModel에서 진행..
						if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EDITOR), strModelPrototypeName,
							CModel::Create(m_pDevice, m_pContext, MODELTYPE::UNDEFINED, szLoadPath, PreTransformMatrix))))
							MSG_BOX(TEXT("Failed to Add Custom Model Prototype.\nLevel_Editor::ImGui_MainMenu() "));

						CModel* pTargetModel = dynamic_cast<CModel*> (m_pGameInstance->Find_Prototype(ENUM_CLASS(LEVEL::EDITOR), strModelPrototypeName));
						MODELTYPE eTargetModelType = pTargetModel->Get_Modeltype();



						// ksta : 8/4 로드 테스트. 일단 화면에 띄워보기

						// 모델로부터 AnimModel 인지 NonAnimModel 인지 정보를 받아와서
						// 서로 다른 프로토타입 게임오브젝트를 사용하도록.
						// 단, CustomObj 는 프로토타입으로 로드가 되어있어야 할 듯
						// 근데 미리 만들어 둘 수가 없음. 프로토타입이 겹치면 문제생길 것 같은데..
						// ㅇㅇ안됨이거 차라리 그때그때 추가해서 만들어야됨 그래야 프로토타입의 의미가있음

						//m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::EDITOR), L"Layer_Editor_Object",
						//	ENUM_CLASS(LEVEL::EDITOR), strPrototypeName);	// 이거 게임오브젝트가 아님. 컴포넌트임. 이걸 쓰는 오브젝트를 새로 추가하던가 해야 함
						
						// 1. 모델 프로토타입을 만듦			(위에서함)
						// 2. 오브젝트 프로토타입을 만듦		(아래에서 해줘야함)
						// 3. 그걸로 레이어에 넣음

						if		(eTargetModelType == MODELTYPE::NONANIM)
						{
							if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EDITOR), strObjectPrototypeName,
								CCustomObj_NonAnim::Create(m_pDevice, m_pContext))))
								MSG_BOX(TEXT("Failed to Add Custom Object Prototype : Level_Editor::ImGui_MainMenu()"));

							CCustomObj_NonAnim::CUSTOMOBJ_NA_DESC CustomObjDesc = {};
							CustomObjDesc.strModelComPrototypeTag = strModelPrototypeName;
							CustomObjDesc.eGameObjType = GAMEOBJ_TYPE::STATIC_PROPS;

							if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::EDITOR), L"Layer_Editor_Object",
								ENUM_CLASS(LEVEL::EDITOR), strObjectPrototypeName, &CustomObjDesc)))
								MSG_BOX(TEXT("Failed to Add Custom Object To Layer : Level_Editor::ImGui_MainMenu()"));
						}
						else if (eTargetModelType == MODELTYPE::ANIM)
						{
							if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::EDITOR), strObjectPrototypeName,
								CCustomObj_Anim::Create(m_pDevice, m_pContext))))
								MSG_BOX(TEXT("Failed to Add Custom Object Prototype : Level_Editor::ImGui_MainMenu()"));

							CCustomObj_Anim::CUSTOMOBJ_A_DESC CustomObjDesc = {};
							CustomObjDesc.strModelComPrototypeTag = strModelPrototypeName;
							CustomObjDesc.eGameObjType = GAMEOBJ_TYPE::STATIC_PROPS;

							if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::EDITOR), L"Layer_Editor_Object",
								ENUM_CLASS(LEVEL::EDITOR), strObjectPrototypeName, &CustomObjDesc)))
								MSG_BOX(TEXT("Failed to Add Custom Object To Layer : Level_Editor::ImGui_MainMenu()"));

						}
						CGameObject* pGameObject = m_pGameInstance->Get_LastGameObject(ENUM_CLASS(LEVEL::EDITOR), L"Layer_Editor_Object");
						m_pSelectedObject = pGameObject;
						m_pObject.push_back(pGameObject);

						CTransform* pObjectTransformCom = dynamic_cast<CTransform*>(pGameObject->Get_Component(L"Com_Transform"));

						_float3 vecSpawnPosition = {};
						_float3 vecSpawnRotation = {};
						_float3 vecSpawnScale = {10, 10, 10};

						_matrix matXMEditPosition = XMMatrixTranslation(vecSpawnPosition.x, vecSpawnPosition.y, vecSpawnPosition.z);
						_matrix matXMEditRotation = XMMatrixRotationRollPitchYaw(vecSpawnRotation.x, vecSpawnRotation.y, vecSpawnRotation.z);
						_matrix matXMEditScale = XMMatrixScaling(vecSpawnScale.x, vecSpawnScale.y, vecSpawnScale.z);
						_matrix matXMEditResult = matXMEditScale * matXMEditRotation * matXMEditPosition;
						pObjectTransformCom->Set_WorldMatrix(matXMEditResult); // 초기값
					}
				}
				if (ImGui::MenuItem("[Binary] Map"))
				{
					wstring strLoadFilePath = L"";
					_bool isLoaded = false;

					isLoaded = LoadExternalFile(FILETYPE::FBX, &strLoadFilePath);

					if (!isLoaded)
					{
						// 받아온 경로 문자열을 이용하여 바이너리 맵 로드 진행
					}
				}

				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Convert to Binary.."))
			{
				if (ImGui::MenuItem("[Raw] FBX : Non Anim"))
				{
					wstring strLoadFilePath = L"";
					wstring strSaveFilePath = L"";
					_bool isLoaded = false;
					_bool isSaved = false;

					// 단순 경로를 받아오는 창을 띄우는 함수
					isLoaded	= LoadExternalFile(FILETYPE::FBX,		&strLoadFilePath);
					isSaved		= SaveExternalFile(FILETYPE::DATMODEL,	&strSaveFilePath);

					// 경로를 잘 받아왔으면 변환 진행
					if (isLoaded && isSaved)
						Convert_FBXToBinary(&strLoadFilePath, &strSaveFilePath, MODELTYPE::NONANIM);
					
				}
				if (ImGui::MenuItem("[Raw] FBX : Anim"))
				{
					wstring strLoadFilePath = L"";
					wstring strSaveFilePath = L"";
					_bool isLoaded = false;
					_bool isSaved = false;

					// 단순 경로를 받아오는 창을 띄우는 함수
					isLoaded	= LoadExternalFile(FILETYPE::FBX, &strLoadFilePath);
					isSaved		= SaveExternalFile(FILETYPE::DATMODEL, &strSaveFilePath);

					// 경로를 잘 받아왔으면 변환 진행
					if (isLoaded && isSaved)
						Convert_FBXToBinary(&strLoadFilePath, &strSaveFilePath, MODELTYPE::ANIM);

				}
				ImGui::EndMenu();
			}
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
		if (ImGui::Button("Active Instant Deploy Mode"))
			isOn_DeployMode = true;
	}
	else {
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.0f, 0.0f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.9f, 0.0f, 0.0f, 1.0f));
		if (ImGui::Button("Deactive Instant Deploy Mode"))
			isOn_DeployMode = false;
		ImGui::PopStyleColor(3);
	}


	static _int iObjIndex = 0;
	static _float3 vPickedPos = {};

	if (ImGui::CollapsingHeader("Manual Deploy Menu"))
	{
#pragma region Manual Deploy UI

		static _float3 vManualDeployObjPos = {};
		static _float3 vManualDeployObjRot = {};
		static _float3 vManualDeployObjSca = {1.f, 1.f, 1.f};

		if (ImGui::BeginMenu("Reset Menu"))
		{
			if (ImGui::MenuItem("Reset Position"))	{ vManualDeployObjPos = { 0.f, 0.f, 0.f }; }
			if (ImGui::MenuItem("Reset Rotation"))	{ vManualDeployObjRot = { 0.f, 0.f, 0.f }; }
			if (ImGui::MenuItem("Reset Scale"))		{ vManualDeployObjSca = { 1.f, 1.f, 1.f }; }
			ImGui::Separator();
			if (ImGui::MenuItem("Reset Transform"))	{	vManualDeployObjPos = { 0.f, 0.f, 0.f };
														vManualDeployObjRot = { 0.f, 0.f, 0.f };
														vManualDeployObjSca = { 1.f, 1.f, 1.f }; }
			ImGui::EndMenu();
		}


		ImGui::PushItemWidth(60);

		// Position Ctrl
		ImGui::Text("Position");

		ImGui::DragFloat("X##pos", &vManualDeployObjPos.x, 0.1f);
		ImGui::SameLine();
		ImGui::DragFloat("Y##pos", &vManualDeployObjPos.y, 0.1f);
		ImGui::SameLine();
		ImGui::DragFloat("Z##pos", &vManualDeployObjPos.z, 0.1f);

		ImGui::Separator();

		// Rotation Ctrl
		ImGui::Text("Position");

		ImGui::DragFloat("X##rot", &vManualDeployObjRot.x, 0.1f);
		ImGui::SameLine();
		ImGui::DragFloat("Y##rot", &vManualDeployObjRot.y, 0.1f);
		ImGui::SameLine();
		ImGui::DragFloat("Z##rot", &vManualDeployObjRot.z, 0.1f);

		ImGui::Separator();

		// Scale Ctrl
		ImGui::Text("Scale");

		ImGui::DragFloat("X##sca", &vManualDeployObjSca.x, 0.1f);
		ImGui::SameLine();
		ImGui::DragFloat("Y##sca", &vManualDeployObjSca.y, 0.1f);
		ImGui::SameLine();
		ImGui::DragFloat("Z##sca", &vManualDeployObjSca.z, 0.1f);

		ImGui::Separator();

		ImGui::PopItemWidth();

#pragma endregion

#pragma region Manual Deploy Button & Logic

		if (ImGui::Button("Deploy!"))
		{
			// Create Logic..

			// ksta : 선택한 터레인에 생성되도록 변경? 아니면 터레인 갯수제한을 1로 두거나
		

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
			m_pSelectedObject = pGameObject;

			CTransform* pObjectTransformCom = dynamic_cast<CTransform*>(pGameObject->Get_Component(L"Com_Transform"));
				
			_matrix matPosition = XMMatrixTranslationFromVector(XMLoadFloat3(&vManualDeployObjPos));
			_matrix matRotation = XMMatrixRotationRollPitchYaw(TO_RAD(vManualDeployObjRot.x), TO_RAD(vManualDeployObjRot.y), TO_RAD(vManualDeployObjRot.z));
			_matrix matScale	= XMMatrixScalingFromVector(XMLoadFloat3(&vManualDeployObjSca));
				
			_matrix matResult	= matScale * matRotation * matPosition;

			pObjectTransformCom->Set_WorldMatrix(matResult);

			m_pObject.push_back(pGameObject);

			iObjIndex++; // 이거 안내려서 문제생긴듯

		}

#pragma endregion

	}


#pragma endregion

#pragma region Model Deploy Logic & Button & Debug

	// 클릭하면 모델 설치
	if (isOn_DeployMode && m_pGameInstance->Get_IsKeyDown(MOUSEKEYSTATE::LB) && m_isNotUsingUI)
	{
		// ksta : 선택한 터레인에 생성되도록 변경? 아니면 터레인 갯수제한을 1로 두거나
		CTerrain* pTerrain = nullptr;
		if (!m_pTerrainObject.empty())
			pTerrain = dynamic_cast<CTerrain*>(m_pTerrainObject.back());
		
		if (pTerrain != nullptr &&
			pTerrain->isPicked(&vPickedPos)) // 이거 false 뜨면 생성 안되게
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
			m_pSelectedObject = pGameObject;

			CTransform* pObjectTransformCom = dynamic_cast<CTransform*>(pGameObject->Get_Component(L"Com_Transform"));
			pObjectTransformCom->Scale(_float3{ 10, 10, 10 }); // 임시로 크기 키움
			pObjectTransformCom->Set_State(STATE::POSITION, XMVectorSet(vPickedPos.x, vPickedPos.y, vPickedPos.z, 1));

			m_pObject.push_back(pGameObject);

			iObjIndex++; // 이거 안내려서 문제생긴듯
		}
	}




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

#pragma endregion

	ImGui::End();
}

void CLevel_Editor::ImGui_Inspector()
{
	// 선택 오브젝트 존재여부 및
	// 오브젝트가 가진 컴포넌트들의 존재여부 검사.
	if (m_pSelectedObject == nullptr)
	{
		isOn_ComViewer_Transform = false;
		isObject_Selected = false;
		return;
	}

	ImGui::Begin("Inspector");

#pragma region Inspector : Transform UI

	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.0f, 0.0f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.9f, 0.0f, 0.0f, 1.0f));
	if (ImGui::CollapsingHeader("Danger Section##InspectorDangerSection"))
	{
		_bool isRunDeleted = false;

		if (ImGui::CollapsingHeader("Instant Delete##Inst_Del_Warn_Front"))
		{
			if (ImGui::Button("Instant Delete##Inst_Del_Run"))
			{
				m_pGameInstance->Remove_GameObject_FromLayer(ENUM_CLASS(LEVEL::EDITOR), L"Layer_Editor_Object", m_pSelectedObject);

				auto iter = std::find(m_pObject.begin(), m_pObject.end(), m_pSelectedObject);
				if (iter != m_pObject.end())
					m_pObject.erase(iter);

				isRunDeleted = true;
			}
		}
		
		if (ImGui::CollapsingHeader("All Reset##Reset_Warn_Front"))
		{
			if (ImGui::CollapsingHeader("Really?##Reset_Warn_End"))
			{
				if (ImGui::Button("All Reset##Reset_Run"))
				{
					for (size_t i = 0; i < m_pObject.size(); i++)
						m_pGameInstance->Remove_GameObject_FromLayer(ENUM_CLASS(LEVEL::EDITOR), L"Layer_Editor_Object", m_pObject[i]);
					m_pObject.clear();

					for (size_t i = 0; i < m_pTerrainObject.size(); i++)
						m_pGameInstance->Remove_GameObject_FromLayer(ENUM_CLASS(LEVEL::EDITOR), L"Layer_Editor_Terrain", m_pTerrainObject[i]);
					m_pTerrainObject.clear();

					isRunDeleted = true;
				}
			}
		}

		if (isRunDeleted)	// 삭제가 실행됐다면.
		{
			m_pSelectedObject = nullptr;
			ImGui::PopStyleColor(3);
			ImGui::End();
			return;
		}
	}
	ImGui::PopStyleColor(3);

	CTransform* pTransformCom = dynamic_cast<CTransform*>(m_pSelectedObject->Get_Component(L"Com_Transform"));
	isOn_ComViewer_Transform = (pTransformCom == nullptr)? false : true;

	// 선택한 오브젝트가 Transform 컴포넌트가 있을 때 보여짐.
	if (isOn_ComViewer_Transform)
	{
		static _float3 vSelectedObjPos;
		static _float3 vSelectedObjRot;
		static _float3 vSelectedObjSca;
		
		if (m_pPrevSelectedObject != m_pSelectedObject)
			// 선택한 오브젝트의 Transform 정보를 받아옴
		{
			_vector		vXMObjPosition = {}, vXMObjQuaternion = {}, vXMObjScale = {};
			_float3		vStoreObjPosition = {}, vStoreObjRotation = {}, vStoreObjScale = {};
			XMMatrixDecompose(&vXMObjScale, &vXMObjQuaternion, &vXMObjPosition, pTransformCom->Get_WorldMatrix());

			_float4x4	matStoreObjQuaternion = {};	// 쿼터니언
			XMStoreFloat4x4(&matStoreObjQuaternion, QUAT_TO_MAT(vXMObjQuaternion));

			XMStoreFloat3(&vStoreObjPosition, vXMObjPosition);
			vStoreObjRotation = MAT_TO_ROT(matStoreObjQuaternion);
			XMStoreFloat3(&vStoreObjScale, vXMObjScale);

			// 대입하여 보여줌
			vSelectedObjPos = vStoreObjPosition;
			vSelectedObjRot = vStoreObjRotation;	// 보일 각도는 오일러, degree 기준. 내부적으로는 radian 변환 후 쿼터니언 처리.
			vSelectedObjSca = vStoreObjScale;
		}

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
			ImGui::Text("Rotation");

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


		// UI를 통해 수정한 값을 반영해줌
		_matrix matXMEditPosition = XMMatrixTranslationFromVector(XMLoadFloat3(&vSelectedObjPos));
		_matrix matXMEditRotation = XMMatrixRotationRollPitchYaw(TO_RAD(vSelectedObjRot.x), TO_RAD(vSelectedObjRot.y), TO_RAD(vSelectedObjRot.z));
		_matrix matXMEditScale = XMMatrixScalingFromVector(XMLoadFloat3(&vSelectedObjSca));

		_matrix matXMEditResult = matXMEditScale * matXMEditRotation * matXMEditPosition;

		pTransformCom->Set_WorldMatrix(matXMEditResult);




	}

#pragma endregion



	ImGui::End();
}


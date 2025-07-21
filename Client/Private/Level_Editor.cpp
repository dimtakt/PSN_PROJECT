#include "Level_Editor.h"

#include "ImGui_Manager.h"
//#include "ImGuiFileDialog.h"
//#include "ImGuiFileDialogConfig.h"

#include "GameInstance.h"
#include "Layer.h"
#include "GameObject.h"
//#include "Client_Struct.h"
#include "Camera_Free.h"

CLevel_Editor::CLevel_Editor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_Editor::Initialize()
{
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
	if (!ImGui::GetIO().WantCaptureMouse)		//UI창 위에 마우스 올라가 있으면 피킹체크X
		Picking_Check();
}

HRESULT CLevel_Editor::Render()
{
	SetWindowText(g_hWnd, TEXT("에디터 레벨 입니다"));
	//m_pImgui_Manage->Render();
	this->ImGui_Render();

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
	CCamera_Free::CAMERA_FREE_DESC		CameraDesc{};

	CameraDesc.vEye = _float4(0.f, 20.f, -15.f, 1.f);
	CameraDesc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
	CameraDesc.fFovy = XMConvertToRadians(60.0f);
	CameraDesc.fNear = 0.1f;
	CameraDesc.fFar = 500.f;
	CameraDesc.fSpeedPerSec = 10.f;
	CameraDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	CameraDesc.fMouseSensor = .2f;

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::EDITOR), strLayerTag,
		ENUM_CLASS(LEVEL::EDITOR), TEXT("Prototype_GameObject_Camera_Free"), &CameraDesc)))
		return E_FAIL;
	//if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::EDITOR), strLayerTag,
	//	ENUM_CLASS(LEVEL::EDITOR), TEXT("Prototype_GameObject_Camera_Mouse"))))
	//	return E_FAIL;

	return S_OK;
}

void CLevel_Editor::ImGui_Render()
{
	m_pImGui_Manager->GUI_Render_Begin();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	// 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
	if (show_demo_window)
		ImGui::ShowDemoWindow(&show_demo_window);

	// 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
	{
		static float f = 0.0f;
		static int counter = 0;

		ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

		ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
		ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
		ImGui::Checkbox("Another Window", &show_another_window);

		ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
		ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

		if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
			counter++;
		ImGui::SameLine();
		ImGui::Text("counter = %d", counter);

		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
		ImGui::End();
	}

	// 3. Show another simple window.
	if (show_another_window)
	{
		ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
		ImGui::Text("Hello from another window!");
		if (ImGui::Button("Close Me"))
			show_another_window = false;
		ImGui::End();
	}

	m_pImGui_Manager->GUI_Render_End();
}

void CLevel_Editor::ImGui_MenuBar_Render()
{
	if (ImGui::BeginMenuBar())
	{
	}
}

void CLevel_Editor::Picking_Check()
{
	//if (m_pGameInstance->Get_IsKeyDown(DIMOUSE_BUTTON0))
	//	m_bPicking = true;
	//else
	//	m_bPicking = false;
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

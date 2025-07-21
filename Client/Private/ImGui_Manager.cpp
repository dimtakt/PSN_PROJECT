#include "ImGui_Manager.h"

CImGui_Manager::CImGui_Manager()
{
	// Initialize ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
}

CImGui_Manager::~CImGui_Manager()
{
	if (ImGui::GetCurrentContext()) {
		ImGui_ImplDX11_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();
	}
}

void CImGui_Manager::GUI_Render_Begin() // like as Update.
{
	// Start the Dear ImGui frame
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

void CImGui_Manager::GUI_Render()
{
	GUI_Render_Begin();

	ImGui::SetNextWindowSize(ImVec2(400, 300), ImGuiCond_Always);
	// 창 제목, X버튼 상태 받는 bool(nullptr이면 X버튼 없음), ImGuiWindowFlags_를 이용해 옵션 삽입 가능
	ImGui::Begin("Test Window", nullptr, ImGuiWindowFlags_MenuBar);

	// Menu Bar
	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			ImGui::MenuItem("open");
			ImGui::MenuItem("save");
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}

	// Slider
	static float sliderValue = 0.0f;

	ImGui::SliderFloat("Slider", &sliderValue, 0.0f, 1.0f);

	// Button
	if (ImGui::Button("Test Me")) {
		// 버튼이 클릭되었을 때 실행될 코드
		MessageBoxA(nullptr, "Button clicked!", "Info", MB_OK);

	}
	// text
	ImGuiIO& io = ImGui::GetIO();
	ImGui::Text("FPS: %.1f", io.Framerate);
	ImGui::Text("Delta_Time: %.5f", ImGui::GetIO().DeltaTime);
	ImGui::End();

	GUI_Render_End();
}

void CImGui_Manager::GUI_Render_End()
{
	ImGui::EndFrame();
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

HRESULT CImGui_Manager::Initialize(HWND hWnd, ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	if (!ImGui_ImplWin32_Init(hWnd))
	{
		MessageBoxA(nullptr, "ImGui Win32 Init Failed", "Error", MB_OK);
		return E_FAIL;
	}
	if (!ImGui_ImplDX11_Init(pDevice, pContext))
	{
		MessageBoxA(nullptr, "ImGui DX9 Init Failed", "Error", MB_OK);
		return E_FAIL;
	}

	ImGui::StyleColorsDark();
	ImFont* font = ImGui::GetIO().Fonts->AddFontDefault();
	ImGui::GetIO().FontDefault = font;
	return S_OK;
}

CImGui_Manager* CImGui_Manager::Create(HWND hWnd, ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CImGui_Manager* pInstance = new CImGui_Manager();
	if (FAILED(pInstance->Initialize(hWnd, pDevice, pContext)))
	{
		MSG_BOX(TEXT("Failed to Created : CImGui_Manager"));
		return nullptr;
	}
	return pInstance;
}

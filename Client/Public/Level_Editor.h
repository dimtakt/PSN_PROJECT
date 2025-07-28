#pragma once

#include "Level.h"
#include "Client_Defines.h"
#include <array>
//#include "Client_Struct.h"

NS_BEGIN(Engine)
class CGameInstance;
class CGameObject;
class CTexture;
class CTransform;
NS_END

NS_BEGIN(Client)
class CImGui_Manager;

class CLevel_Editor final : public CLevel
{

private:
	explicit CLevel_Editor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_Editor() = default;

public:
	virtual HRESULT Initialize() override;
	virtual void Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;



private:
	HRESULT Ready_Lights();
	HRESULT Ready_ImGui(HWND hWnd, ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	HRESULT Ready_Layer_Camera(const _wstring& strLayerTag);

private:
	CImGui_Manager* m_pImGui_Manager = nullptr;

private:	// Default Window + 변수 컨트롤
	void ImGui_Render();

	void Check_NotUsingUI();		// UI에 커서를 사용중인지를 리턴
	_bool Check_ObjectPicking();	// 컨테이너 내 obj 피킹여부 리턴, 선택중 Obj에 할당

private:	// Custom Window
	void ImGui_MainMenu();			// 최상단 메뉴

	void ImGui_TerrainEditor();
	void ImGui_ModelDeployer();

	void ImGui_Inspector();

private:	// 해당 클래스에서만 사용할 함수
	enum class FILETYPE {
		FBX,			// fbx 원본
		DATMODEL,		// fbx 를 바이너리화 한 data
		DATMAP,			// map data.

		FILETYPE_END
	};
	wstring GetFilePath(FILETYPE eFileType);

private:
	//bool show_demo_window = true;
	//bool show_another_window = false;
	//_float4 clear_color = _float4(0.45f, 0.55f, 0.60f, 1.00f);

	//bool m_bPicking = false;
	
	// Editor WIndows
	_bool isOn_GUITerrainEditor = true;				// 터레인 에디터 창
	_bool isOn_ModelDeployer = false;				// 모델 배치기 창
	_bool isOn_DeployMode = false;					// ㄴ 모델 배치모드 전환


	// ===== Inspector Window
	// Opens when "isObject_Selected" is True,
	// Visibles when "pSelectedObject" has specific component.
	_bool isOn_ComViewer_Transform = false;			// 현재 선택한 오브젝트의 Transform 제어창.





	std::vector<CGameObject*> m_pObject = {};
	std::vector<CGameObject*> m_pTerrainObject = {};




	_bool isObject_Selected = false;		// 선택된 오브젝트가 존재할 때 True.
	CGameObject* pSelectedObject = {};		// 현재 선택된 오브젝트. 임시 포인터 변수
	CGameObject* pPrevSelectedObject = {};	// 이전 선택된 오브젝트. 임시 포인터 변수
	_bool m_isNotUsingUI = false;			// UI창이 사용중이 아닐 때 True.

public:
	static CLevel_Editor* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

NS_END
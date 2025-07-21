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

public:
	HRESULT Ready_ImGui(HWND hWnd, ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	HRESULT Ready_Layer_Camera(const _wstring& strLayerTag);

private:
	CImGui_Manager* m_pImGui_Manager = nullptr;

private:
	void ImGui_Render();
	void ImGui_MenuBar_Render();
	void Picking_Check();

private:
	bool show_demo_window = true;
	bool show_another_window = false;
	_float4 clear_color = _float4(0.45f, 0.55f, 0.60f, 1.00f);

	//bool m_bPicking = false;

public:
	static CLevel_Editor* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

NS_END
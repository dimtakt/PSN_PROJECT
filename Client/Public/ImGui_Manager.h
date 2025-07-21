#pragma once

#include "Client_Defines.h"
#include "Engine_Defines.h"


#ifdef new
#undef new
#endif
#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"
//#include "GeometryHelper.h"
//#include "Camera.h"
//#include "GameObject.h"
//#include "CameraScript.h"
//#include "MeshRenderer.h"
//#include "MeshMaterial.h"
//#include "Model.h"
//#include "ModelRenderer.h"


NS_BEGIN(Client)

class CImgui_Manager
{
private:
	CImgui_Manager();
public:
	~CImgui_Manager();

public:
	void GUI_Render_Begin();
	void GUI_Render();
	void GUI_Render_End();
public:
	HRESULT Initialize(HWND hWnd, ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	static CImgui_Manager* Create(HWND hWnd, ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

private:


};

NS_END
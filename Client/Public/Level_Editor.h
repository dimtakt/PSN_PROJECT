#pragma once

#include "Level.h"
#include "Client_Defines.h"

//#include "Client_Struct.h"

NS_BEGIN(Engine)
class CGameInstance;
class CGameObject;
class CTexture;
class CTransform;
class CCell;
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
	void	ImGui_Render();

	void	Check_NotUsingUI();			// UI에 커서를 사용중인지를 리턴
	_bool	Check_ObjectPicking();		// 컨테이너 내 obj 피킹여부 리턴, 선택중 Obj에 할당

		
private:	// Custom Window
	void	ImGui_MainMenu();			// 최상단 메뉴

	void	ImGui_TerrainEditor();
	void	ImGui_ModelDeployer();
	void	ImGui_NavMeshEditor();

	void	ImGui_Inspector();

	void	ImGui_Descriptions();


private:	// 해당 클래스에서만 사용할 함수
	_bool LoadExternalFile(FILETYPE eFileType, _wstring* strPathOut);
	_bool SaveExternalFile(FILETYPE eFileType, _wstring* strPathOut);
		
	HRESULT Convert_FBXToBinary(_wstring* strLoadPath, _wstring* strSavePath, MODELTYPE eAnimType);
	
	HRESULT Load_BinaryMap(_wstring* strLoadPath);
	HRESULT Save_BinaryMap(_wstring* strSavePath);

	_bool Get_PickingPos(_float3* pOut, _bool isIgnoreAnimMesh = true);			// 피킹 검사
	_bool Get_ObjectPickingPos(_float3* pOut, _bool isIgnoreAnimMesh = true);	// ㄴ오브젝트와 피킹 검사, AnimObject 필터 기능도...
	_bool Get_TerrainPickingPos(_float3* pOut);									// ㄴ터레인과 피킹 검사
	
	void LoadedItemsName();




#pragma region ===== [Overrides Parent Functions..] =====

private:
	virtual HRESULT Add_Prototype_Direct(
		_uint iPrototypeLevelIndex,
		const _wstring& strFileName,
		_matrix* PreTransformMatrix = nullptr,
		LVLCUSTOMOBJ_DESC* pLvlArg = nullptr
	) override
	{
		LVLCUSTOMOBJ_DESC tDesc = {};
		tDesc.strFilePathPrefix				= (pLvlArg != nullptr)? pLvlArg->strFilePathPrefix			: L"../Bin/Resources/_SUPERHOT/_BinaryModels/";
		tDesc.strFileExt					= (pLvlArg != nullptr)? pLvlArg->strFileExt					: L".datmodel";
		tDesc.strModelPrototypePrefix		= (pLvlArg != nullptr)? pLvlArg->strModelPrototypePrefix	: L"Prototype_Component_Model_Custom_";
		tDesc.strObjectPrototypePrefix		= (pLvlArg != nullptr)? pLvlArg->strObjectPrototypePrefix	: L"Prototype_GameObject_Model_Custom_";
		tDesc.strModelCustomPrototypeTag	= (pLvlArg != nullptr)? pLvlArg->strModelCustomPrototypeTag	: L"";
		tDesc.strObjectCustomPrototypeTag	= (pLvlArg != nullptr)? pLvlArg->strObjectCustomPrototypeTag: L"";


		return __super::Add_Prototype_Direct(
			iPrototypeLevelIndex,
			strFileName,
			PreTransformMatrix,
			&tDesc);
	}

	virtual HRESULT Add_GameObject_ToLayer_Direct(
		_uint iLayerLevelIndex,
		const _wstring& strLayerTag,
		_uint iPrototypeLevelIndex,
		const _wstring& strPrototypeTagSuffix,
		void* pObjArg = nullptr,
		LVLCUSTOMOBJ_DESC* pLvlArg = nullptr
	) override
	{
		LVLCUSTOMOBJ_DESC tDesc = {};
		tDesc.strFilePathPrefix				= (pLvlArg != nullptr)? pLvlArg->strFilePathPrefix			: L"../Bin/Resources/_SUPERHOT/_BinaryModels/";
		tDesc.strFileExt					= (pLvlArg != nullptr)? pLvlArg->strFileExt					: L".datmodel";
		tDesc.strModelPrototypePrefix		= (pLvlArg != nullptr)? pLvlArg->strModelPrototypePrefix	: L"Prototype_Component_Model_Custom_";
		tDesc.strObjectPrototypePrefix		= (pLvlArg != nullptr)? pLvlArg->strObjectPrototypePrefix	: L"Prototype_GameObject_Model_Custom_";
		tDesc.strModelCustomPrototypeTag	= (pLvlArg != nullptr)? pLvlArg->strModelCustomPrototypeTag	: L"";
		tDesc.strObjectCustomPrototypeTag	= (pLvlArg != nullptr)? pLvlArg->strObjectCustomPrototypeTag: L"";


		return __super::Add_GameObject_ToLayer_Direct(
			iLayerLevelIndex,
			strLayerTag,
			iPrototypeLevelIndex,
			strPrototypeTagSuffix,
			pObjArg,
			&tDesc);
	}


#pragma endregion

private:
	//bool show_demo_window = true;
	//bool show_another_window = false;
	//_float4 clear_color = _float4(0.45f, 0.55f, 0.60f, 1.00f);

	//bool m_bPicking = false;
	
	// Editor WIndows
	_bool		isOn_GUITerrainEditor	= true;				// 터레인 에디터 창
	_bool		isOn_ModelDeployer		= false;			// 모델 배치기 창
	_bool		isOn_DeployMode			= false;			// ㄴ 모델 배치모드 전환
	_bool		isOn_Descriptions		= false;			// 저장 전 맵 정보 기입창
	_bool		isOn_NavMeshEditor		= false;			// 네비메쉬 편집창
	_bool		isOn_NavEditMode		= false;			// ㄴ 네비메쉬 편집모드 전환



	// ===== Inspector Window
	// Opens when "isObject_Selected" is True,
	// Visibles when "pSelectedObject" has specific component.
	_bool		isOn_ComViewer_Transform = false;			// 현재 선택한 오브젝트의 Transform 제어창.

	
	LEVEL					m_eTargetLevel = {};

	vector<CGameObject*>	m_pObject = {};
	vector<CGameObject*>	m_pTerrainObject = {};

	vector<CCell*>			m_pCells = {};
	vector<CGameObject*>	m_pTempGuideObject = {};		// NavMesh UI 가이드용 임시 오브젝트
															// ㄴ _EditorGuideModels/_EditorGuideSphere.datmodel 사용
	NAVMESH_DESC			m_tNavMeshData = {};
	_float3					m_tCellPoints[3] = {};


	/** 
	*	
	*	typedef struct tagNavigationMeshDesc {
	*	
	*		_uint					iNumTris;
	*		vector<_float3[3]>		vecTris;
	*	
	*	}NAVMESH_DESC;
	* 
	*/
	

	
	

	

	vector<_wstring>		m_vLoadedItems = {};				// 모델 요소 저장. 저장 시 FileName 으로 저장할 것.
	vector<_string>			m_vLoadedItemsConv = {};			// 변환용
	vector<const _char*>	m_vLoadedItemPtrs = {};

	_bool					isObject_Selected = false;			// 선택된 오브젝트가 존재할 때 True.
	CGameObject*			m_pSelectedObject = {};				// 현재 선택된 오브젝트. 임시 포인터 변수
	CGameObject*			m_pPrevSelectedObject = {};			// 이전 선택된 오브젝트. 임시 포인터 변수
	_bool					m_isNotUsingUI = false;				// UI창이 사용중이 아닐 때 True.


	


	MAPDATA_DESC			m_tMapData = {};


public:
	static CLevel_Editor* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

NS_END
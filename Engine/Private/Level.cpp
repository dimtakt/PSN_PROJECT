#include "Level.h"

#include "GameInstance.h"

#include "CustomObj_NonAnim.h"
#include "CustomObj_Anim.h"


CLevel::CLevel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice{ pDevice }
	, m_pContext{ pContext }
	, m_pGameInstance{ CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
	Safe_AddRef(m_pGameInstance);

}

HRESULT CLevel::Initialize()
{
	return S_OK;
}

void CLevel::Update(_float fTimeDelta)
{
}

HRESULT CLevel::Render()
{
	return S_OK;
}

// 게임 실행중에 실시간으로 프로토타입을 추가하기 위함.
// 필요 정보? : 모델 파일의 저장 경로
// 바이너리화(datmodel) 된 파일을, 특정 경로에서만 로드한다고 가정 (../Bin/Resources/_SUPERHOT/_BinaryModels/)
HRESULT CLevel::Add_Prototype_Direct(
	_uint iPrototypeLevelIndex,
	const _wstring& strPrototypeTag,
	CBase* pPrototype,
	const _wstring& strFileName,
	_matrix* PreTransformMatrix
)
{
	// 사전 변환 행렬을 받아왔다면 해당 행렬을, 아니라면 Identity 사용
	_matrix matPreTransformMatrix;
	_matrix matIdentity = XMMatrixIdentity();
	matPreTransformMatrix = (PreTransformMatrix == nullptr) ? matIdentity : *PreTransformMatrix;

	// 파일 경로 (상대경로) 전처리..
	_wstring strFilePathSuffix = strFileName;									// 파일명
	_wstring strFilePathPrefix = L"../Bin/Resources/_SUPERHOT/_BinaryModels/";	// 상대경로
	_wstring strFileExt = L".datmodel";

	_wstring strFilePath = strFilePathPrefix + strFilePathSuffix + strFileExt;
	const _char* szFilePath = WStringToChar(strFilePath);

	_wstring strModelPrototypePrefix = L"Prototype_Component_Model_Custom_";	// 모델 컴포넌트용
	_wstring strObjectPrototypePrefix = L"Prototype_GameObject_Model_Custom_";	// 게임오브젝트용
	_wstring strModelPrototypeTag = strModelPrototypePrefix + strFilePathSuffix;
	_wstring strObjectPrototypeTag = strObjectPrototypePrefix + strFilePathSuffix;


	// ===== !! 모델 프로토타입 생성.. !! =====
	if (FAILED(m_pGameInstance->Add_Prototype(iPrototypeLevelIndex, strModelPrototypeTag,
		CModel::Create(m_pDevice, m_pContext, MODELTYPE::UNDEFINED, szFilePath, matPreTransformMatrix))))
	{
		MSG_BOX(TEXT("Failed to Add Custom Model Prototype.\nLevel::Add_Prototype_Direct() "));
		return E_FAIL;
	}

	// ===== !! 게임오브젝트 프로토타입 생성.. !! =====
	CModel* pTargetModel = dynamic_cast<CModel*>(m_pGameInstance->Find_Prototype(iPrototypeLevelIndex, strModelPrototypeTag));
	MODELTYPE eTargetModelType = pTargetModel->Get_Modeltype();
	if (eTargetModelType == MODELTYPE::NONANIM)
	{
		if (FAILED(m_pGameInstance->Add_Prototype(iPrototypeLevelIndex, strObjectPrototypeTag,
			CCustomObj_NonAnim::Create(m_pDevice, m_pContext))))
			MSG_BOX(TEXT("Failed to Add Custom Object Prototype.\nLevel_Editor::ImGui_MainMenu()"));

		//CCustomObj_NonAnim::CUSTOMOBJ_NA_DESC CustomObjDesc = {};
		//CustomObjDesc.strModelComPrototypeTag = strModelPrototypeTag;
		//CustomObjDesc.eGameObjType = GAMEOBJ_TYPE::STATIC_PROPS;
	}
	else if (eTargetModelType == MODELTYPE::ANIM)
	{
		if (FAILED(m_pGameInstance->Add_Prototype(iPrototypeLevelIndex, strObjectPrototypeTag,
			CCustomObj_Anim::Create(m_pDevice, m_pContext))))
			MSG_BOX(TEXT("Failed to Add Custom Object Prototype.\nLevel_Editor::ImGui_MainMenu()"));

		//CCustomObj_Anim::CUSTOMOBJ_A_DESC CustomObjDesc = {};
		//CustomObjDesc.strModelComPrototypeTag = strModelPrototypeTag;
		//CustomObjDesc.eGameObjType = GAMEOBJ_TYPE::STATIC_PROPS;
	}


	Safe_Delete(szFilePath);
	return S_OK;
}

// 게임 실행중에 실시간으로 게임오브젝트를 추가하기 위함.
HRESULT CLevel::Add_GameObject_ToLayer_Direct(
	_uint iLayerLevelIndex,
	const _wstring& strLayerTag,
	_uint iPrototypeLevelIndex,
	const _wstring& strPrototypeTag,
	void* pArg
)
{



	return S_OK;
}



void CLevel::Free()
{
	__super::Free();

	Safe_Release(m_pGameInstance);
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}

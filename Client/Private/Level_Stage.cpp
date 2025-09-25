#include "Level_Stage.h"

#include "GameInstance.h"
#include "Camera_Free.h"
#include "Terrain.h"

#include "CustomObj.h"

#include "Level_Loading.h"
#include "UI_ScreenText.h"




CLevel_Stage::CLevel_Stage(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel{ pDevice, pContext }
{
}

HRESULT CLevel_Stage::Initialize()
{
	return S_OK;
}

void CLevel_Stage::Update(_float fTimeDelta)
{

}

HRESULT CLevel_Stage::Render()
{
	return S_OK;
}

HRESULT CLevel_Stage::Load_BinaryMap(_wstring* strLoadPath)
{
	_uint iCurLevel = m_pGameInstance->Get_DestLevel();


	// ==============================
	// || 바이너리 파일로부터 로컬 함수로..
	// ==============================

	ifstream ifs(strLoadPath->c_str(), ios::binary);
	if (!ifs.is_open())
	{
		MessageBoxW(nullptr, L"파일 열기 실패", L"LoadMapDataFromFile", MB_OK);
		return E_FAIL;
	}

	ifs.read(reinterpret_cast<char*>(&m_tMapData.iMapLevel), sizeof(m_tMapData.iMapLevel));

	ifs.read(reinterpret_cast<char*>(&m_tMapData.iNumLoadedItems), sizeof(m_tMapData.iNumLoadedItems));
	m_tMapData.vecLoadedItems.clear();
	for (unsigned int i = 0; i < m_tMapData.iNumLoadedItems; ++i)
	{
		unsigned int len = 0;
		ifs.read(reinterpret_cast<char*>(&len), sizeof(len));

		wstring str(len, L'\0');
		ifs.read(reinterpret_cast<char*>(&str[0]), len * sizeof(wchar_t));
		m_tMapData.vecLoadedItems.push_back(str);
	}

	ifs.read(reinterpret_cast<char*>(&m_tMapData.iNumGameObj), sizeof(m_tMapData.iNumGameObj));
	m_tMapData.vecGameObj.clear();
	//m_tMapData.vecGameObj.reserve(m_tMapData.iNumGameObj);

	for (unsigned int i = 0; i < m_tMapData.iNumGameObj; ++i)
	{
		LOADED_OBJ_DESC desc;

		ifs.read(reinterpret_cast<char*>(&desc.matFinalTransform), sizeof(XMFLOAT4X4));

		unsigned int len = 0;
		ifs.read(reinterpret_cast<char*>(&len), sizeof(len));

		wstring str(len, L'\0');
		ifs.read(reinterpret_cast<char*>(&str[0]), len * sizeof(wchar_t));
		desc.strFileName = str;

		ifs.read(reinterpret_cast<char*>(&desc.iObjType), sizeof(_uint));

		m_tMapData.vecGameObj.push_back(desc);
	}

	ifs.read(reinterpret_cast<char*>(&m_tMapData.iNumTerrains), sizeof(m_tMapData.iNumTerrains));
	m_tMapData.vecTerrainTransform.clear();
	for (unsigned int i = 0; i < m_tMapData.iNumTerrains; ++i) // 여기서 루프 돔
	{
		XMFLOAT4X4 mat;
		ifs.read(reinterpret_cast<char*>(&mat), sizeof(XMFLOAT4X4));
		m_tMapData.vecTerrainTransform.push_back(mat);
	}

	ifs.close();



	// ==============================
	// ||  오브젝트 로드.. 
	// ==============================

	m_eTargetLevel = static_cast<LEVEL>(m_tMapData.iMapLevel);

	for (_uint i = 0; i < m_tMapData.iNumLoadedItems; i++)
	{
		// ===== !! 프로토타입 생성.. !! =====

		// 파일 경로 (상대경로)
		_wstring strFilePathSuffix = m_tMapData.vecLoadedItems[i];					// 파일명
		_wstring strFilePathPrefix = L"../Bin/Resources/_SUPERHOT/_BinaryModels/";	// 상대경로
		_wstring strFileExt = L".datmodel";

		// 없을 경우만 생성
		auto iter = std::find(m_vLoadedItems.begin(), m_vLoadedItems.end(), strFilePathSuffix);
		if (iter != m_vLoadedItems.end())
			continue;

		// 좌표계 보정
		_matrix		PreTransformMatrix = XMMatrixIdentity();
		PreTransformMatrix = XMMatrixRotationY(XMConvertToRadians(180.0f));

		// 생성
		if (FAILED(Add_Prototype_Direct(iCurLevel, strFilePathSuffix, &PreTransformMatrix)))
			return E_FAIL;

		// 로컬 변수
		m_vLoadedItems.push_back(m_tMapData.vecLoadedItems[i]);
	}

	for (_uint i = 0; i < m_tMapData.iNumGameObj; i++)
	{
		// ===== !! 게임오브젝트 생성 및 삽입.. !! =====

		// 프로토타입명 (모델 프로토타입은 Desc에 묶어서 인자로, 오브젝트 프로토타입은 인자로) 
		LOADED_OBJ_DESC tDesc = m_tMapData.vecGameObj[i];

		_wstring strFilePathSuffix = tDesc.strFileName;					// 파일명
		_wstring strModelPrototypePrefix = L"Prototype_Component_Model_Custom_";

		CCustomObj::CUSTOMOBJ_DESC CustomObjDesc = {};	// 일단 Description은 이걸로, 어차피 형식은 같음
		CustomObjDesc.iGameObjType = ENUM_CLASS(GAMEOBJ_TYPE::STATIC_PROPS); // ksta : 이것도 나중에 파일 불러올 때에 안에서 정하도록..
		CustomObjDesc.strModelComPrototypeTag = strModelPrototypePrefix + strFilePathSuffix;

		if (FAILED(Add_GameObject_ToLayer_Direct(iCurLevel, L"Layer_Loaded_Object", iCurLevel, strFilePathSuffix, &CustomObjDesc)))
			return E_FAIL;

		// 좌표반영
		CGameObject* pGameObject = m_pGameInstance->Get_LastGameObject(iCurLevel, L"Layer_Loaded_Object");
		pGameObject->Set_FileName(strFilePathSuffix);
		CTransform* pObjectTransformCom = dynamic_cast<CTransform*>(pGameObject->Get_Component(L"Com_Transform"));
		pObjectTransformCom->Set_WorldMatrix(tDesc.matFinalTransform);

		pGameObject->Set_ObjType(m_tMapData.vecGameObj[i].iObjType);

		// 로컬 변수
		m_pObject.push_back(pGameObject);
	}

	for (_uint i = 0; i < m_tMapData.iNumTerrains; i++)
	{
		// ===== !! 터레인 생성 및 삽입.. !! =====
		_float4x4	matTerrainTransform = m_tMapData.vecTerrainTransform[i];

		_wstring strLayerTag = L"Layer_Loaded_Terrain";
		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(iCurLevel, strLayerTag,
			iCurLevel, TEXT("Prototype_GameObject_Terrain"))))
			return E_FAIL;

		CGameObject* pTerrainObject = m_pGameInstance->Get_LastGameObject(iCurLevel, strLayerTag);
		CTransform* pObjectTransformCom = dynamic_cast<CTransform*>(pTerrainObject->Get_Component(L"Com_Transform"));
		pObjectTransformCom->Set_WorldMatrix(m_tMapData.vecTerrainTransform[i]);

		// 로컬 변수
		m_pTerrainObject.push_back(pTerrainObject);

	}

	return S_OK;
}

void CLevel_Stage::Update_CheckEndLevel(_float fTimeDelta, LEVEL eNextLevel)
{
	// 현재 스테이지에 적이 더이상 남아있지 않다면, 레벨의 종료 준비
	_uint iDestLevel = m_pGameInstance->Get_DestLevel();

	CGameObject* pFrontEnemy = m_pGameInstance->Find_GameObject(iDestLevel, L"Layer_Monster");

	// 종료 준비 활성화, 종료 준비 중 돌릴 작업 수행 (UI효과 등)
	if (pFrontEnemy == nullptr)
		m_isEndLevelStandby = true;
	if (m_isEndLevelStandby)
		Update_EndLevelStandby(fTimeDelta);

	if (m_isEndLevel)							// 종료 조건 시
		Change_ToNextLevel(eNextLevel);	// 전환
}

void CLevel_Stage::Update_EndLevelStandby(_float fTimeDelta)
{
	m_fEndLevelDeltaTime += (m_pGameInstance->Get_RawTimeDelta());

	if		(m_fEndLevelDeltaTime > 0.f && (m_iSuperHot == 0))
	{
		m_iSuperHot = 1;
		m_pUI_ScreenText->Show_ScreenText(ENUM_CLASS(SCREENTEXT_INDEX::LVLEND_SUPER));
		m_pGameInstance->PlaySoundFixed(L"super.wav", ENUM_CLASS(SOUND_CHANNEL::SOUND_MAINUI));
	}
	else if (m_fEndLevelDeltaTime > 1.2f && (m_iSuperHot == 1))
	{
		m_iSuperHot = 2;
		m_pUI_ScreenText->Show_ScreenText(ENUM_CLASS(SCREENTEXT_INDEX::LVLEND_HOT));
		m_pGameInstance->PlaySoundFixed(L"hot.wav", ENUM_CLASS(SOUND_CHANNEL::SOUND_MAINUI));
	}
	else if (m_fEndLevelDeltaTime > 2.4f && (m_iSuperHot == 2))
	{
		m_fEndLevelDeltaTime = 0;
		m_iSuperHot = 0;
	}



	// 키 입력 시 레벨 넘김
	if (m_pGameInstance->Get_IsKeyDown(DIK_SPACE) ||
		m_pGameInstance->Get_IsKeyDown(DIK_RETURN))
	{
		m_isEndLevel = true;
	}
}

void CLevel_Stage::Change_ToNextLevel(LEVEL eLevel)
{
	if (FAILED(m_pGameInstance->Open_Level(static_cast<_uint>(LEVEL::LOADING), CLevel_Loading::Create(m_pDevice, m_pContext, eLevel))))
		MSG_BOX(L"[CLevel_Stage::Change_Level] Changing Level Failed.");
}

void CLevel_Stage::Free()
{
	__super::Free();



}

#pragma once

#include "Client_Defines.h"
#include "Level.h"


NS_BEGIN(Engine)

class CGameObject;

NS_END


NS_BEGIN(Client)

class CLevel_Stage abstract : public CLevel
{
protected:
	CLevel_Stage(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_Stage() = default;

public:
	virtual HRESULT Initialize() override;
	virtual void Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

protected:
	virtual HRESULT Load_BinaryMap(_wstring* strLoadPath) override;

#pragma region ===== [Overrides Parent Functions..] =====

protected:
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

protected:
	// 맵 로드시 필요한 정보들..
	MAPDATA_DESC			m_tMapData = {};					// 로드 시 정보를 담을 임시 객체
	LEVEL					m_eTargetLevel = {};				// 현재 로드한 맵의 레벨 정보. 맵에 포함돼있으며, 검증용

	vector<_wstring>		m_vLoadedItems = {};				// 모델 요소 저장. 저장 시 FileName 으로 저장할 것.
																// 중복 로드 방지 검증용

	vector<CGameObject*>	m_pObject = {};						// 로드된 실제 객체 정보가 담김.
	vector<CGameObject*>	m_pTerrainObject = {};				// 로드된 실제 터레인 정보가 담김.

protected:
	// 맵 전환 및 효과에 필요
	void	Update_CheckEndLevel(_float fTimeDelta);
	void	Update_EndLevelStandby(_float fTimeDelta);
	void	Change_ToNextLevel(LEVEL eLevel);

	_float					m_fEndLevelDeltaTime = 0.f;
	_bool					m_isEndLevelStandby = false;
	_bool					m_isEndLevel = false;
	_uint					m_iSuperHot = 0;

	class CUI_ScreenText*	m_pUI_ScreenText = { nullptr };

public:
	virtual void Free() override;
};

NS_END
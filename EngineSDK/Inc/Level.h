#pragma once

#include "Base.h"

/* 클라개발자가 정의한 여러 레벨클래스들의 부모가 되는 클래스 .*/

NS_BEGIN(Engine)

class ENGINE_DLL CLevel abstract : public CBase
{
protected:
	typedef struct tLevelCustomObjDesc
	{
		_wstring strFilePathPrefix = {};		// 파일
		// strFilePathSuffix 는 FileName 으로 받아옴.
		_wstring strFileExt = {};

		_wstring strModelPrototypePrefix = {};
		_wstring strObjectPrototypePrefix = {};

	}LVLCUSTOMOBJ_DESC;

protected:
	CLevel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel() = default;

public:
	virtual HRESULT Initialize();
	virtual void Update(_float fTimeDelta);
	virtual HRESULT Render();

protected:
	ID3D11Device*				m_pDevice = { nullptr };
	ID3D11DeviceContext*		m_pContext = { nullptr };
	class CGameInstance*		m_pGameInstance = { nullptr };


protected:
	virtual HRESULT Add_Prototype_Direct(
		_uint iPrototypeLevelIndex,
		const _wstring& strFileName,
		_matrix* PreTransformMatrix = nullptr,
		LVLCUSTOMOBJ_DESC* pLvlArg = nullptr
	);

	HRESULT Add_Prototype_Direct_Model(
		_uint iPrototypeLevelIndex,
		const _wstring& strFileName,
		_matrix* PreTransformMatrix,
		LVLCUSTOMOBJ_DESC* pLvlArg
	);
	HRESULT Add_Prototype_Direct_GameObject(
		_uint iPrototypeLevelIndex,
		const _wstring& strFileName,
		LVLCUSTOMOBJ_DESC* pLvlArg
	);

	virtual HRESULT Add_GameObject_ToLayer_Direct(
		_uint iLayerLevelIndex,
		const _wstring& strLayerTag,
		_uint iPrototypeLevelIndex,
		const _wstring& strPrototypeTagSuffix,
		void* pObjArg = nullptr,
		LVLCUSTOMOBJ_DESC* pLvlArg = nullptr
	);

public:	
	virtual void Free();
};

NS_END
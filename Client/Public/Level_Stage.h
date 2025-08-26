#pragma once

#include "Client_Defines.h"
#include "Level.h"


NS_BEGIN(Engine)

class CGameObject;


NS_END


NS_BEGIN(Client)

class CLevel_Stage abstract : public CLevel
{
private:
	CLevel_Stage(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_Stage() = default;

public:
	virtual HRESULT Initialize() override;
	virtual void Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

protected:
	virtual HRESULT Load_BinaryMap(_wstring* strLoadPath) override;

protected:
	// 맵 로드시 필요한 정보들..
	MAPDATA_DESC			m_tMapData = {};					// 로드 시 정보를 담을 임시 객체
	LEVEL					m_eTargetLevel = {};				// 현재 로드한 맵의 레벨 정보. 맵에 포함돼있으며, 검증용

	vector<_wstring>		m_vLoadedItems = {};				// 모델 요소 저장. 저장 시 FileName 으로 저장할 것.
																// 중복 로드 방지 검증용

	vector<CGameObject*>	m_pObject = {};						// 로드된 실제 객체 정보가 담김.
	vector<CGameObject*>	m_pTerrainObject = {};				// 로드된 실제 터레인 정보가 담김.

public:
	virtual void Free() override;
};

NS_END
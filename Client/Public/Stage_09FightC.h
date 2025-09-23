#pragma once

#include "Client_Defines.h"
#include "Level_Stage.h"

NS_BEGIN(Client)

class CStage_09FightC final : public CLevel_Stage
{
private:
	CStage_09FightC(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CStage_09FightC() = default;

public:
	virtual HRESULT Initialize() override;
	virtual void Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Ready_Lights();
	HRESULT Ready_Layer_Camera(const _wstring& strLayerTag);
	HRESULT Ready_Layer_BackGround(const _wstring& strLayerTag);
	HRESULT Ready_Layer_UI(const _wstring& strLayerTag);
	HRESULT Ready_Layer_Player(const _wstring& strLayerTag);
	HRESULT Ready_Layer_Monster(const _wstring& strLayerTag);
	HRESULT Ready_Layer_Effect(const _wstring& strLayerTag);

	HRESULT Ready_Pickup_Objects(const _wstring& strLayerTag);

public:
	static CStage_09FightC* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

NS_END
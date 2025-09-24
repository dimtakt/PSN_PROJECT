#pragma once

#include "Client_Defines.h"
#include "Level_Stage.h"

NS_BEGIN(Client)

class CStage_01Kick final : public CLevel_Stage
{
private:
	CStage_01Kick(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CStage_01Kick() = default;

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

private:
	void	Update_TriggerOnce();
	void	Update_Trigger_OnTime(_float fTimeDelta);

private:
	_bool	m_isTriggered			= false;
	_float	m_fTimeEventDeltaTime	= 0.f;

	_bool	m_isUIEventTriggered	= false;
	_float	m_fUIEventDeltaTime		= 0.f;


private:
	//class CUI_ScreenText* m_pUIScreenText = nullptr;
	//class CUI_ScreenFont* m_pUIScreenFont = nullptr;

public:
	static CStage_01Kick* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

NS_END
#pragma once

#include "Client_Defines.h"
#include "Level.h"

NS_BEGIN(Client)

class CLevel_Logo final : public CLevel
{
private:
	enum class LOGO_PATH_MAIN
	{
		PATH_ABSOLUTE,	// 최상위 경로

		PATH_LEVEL,

		PATH_END
	};

	enum class LOGO_INDEX_MAIN
	{
		MAIN_F_LEVELS,

		MAIN_GAMEPLAY,
		MAIN_EDITOR,
		MAIN_QUIT,

		MAIN_END
	};

	enum class LOGO_INDEX_LEVEL
	{
		MAINLVL_FOLDERUP,

		MAINLVL_TEST1,
		MAINLVL_TEST2,
		MAINLVL_TEST3,
		MAINLVL_01KICK,
		MAINLVL_09FIGHTC,
		MAINLVL_10DESPER,

		MAINLVL_END
	};

private:
	CLevel_Logo(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_Logo() = default;

public:
	virtual HRESULT Initialize() override;
	virtual void Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Render_LogoScreen();
	HRESULT Render_Folder_Level();


private:
	HRESULT Ready_Layer_BackGround(const _wstring& strLayerTag);

private:
	_uint	m_iMenuFocusingIndex = {};

	_uint	m_iMenuFocusingPath = {};

public:
	static CLevel_Logo* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

NS_END
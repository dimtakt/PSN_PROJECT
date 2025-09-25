#pragma once

#include "Client_Defines.h"
#include "UIObject.h"

NS_BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
NS_END

NS_BEGIN(Client)

class CUI_ScreenText final : public CUIObject
{
private:
	CUI_ScreenText(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_ScreenText(const CUI_ScreenText& Prototype);
	virtual ~CUI_ScreenText() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();


private:
	void Change_ScreenText(_uint iTexIndex);

private:
	void Update_PopupUI(_float fTimeDelta);
	void Update_FloatUI(_float fTimeDelta);

public:
	// change textsize.. ¿Ã∑±∞≈
	void Change_Size(_float fSize);

	void Show_ScreenText(_uint iTexIndex);
	void Hide_ScreenText() { m_isActive = false; }



private:
	CShader*		m_pShaderCom				= { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom				= { nullptr };

	CTexture*		m_pTextureCom_NoAmmo		= { nullptr };
	CTexture*		m_pTextureCom_TimeMoves		= { nullptr };

	CTexture*		m_pTextureCom_TUTO_LeftClick= { nullptr };

	CTexture*		m_pTextureCom_LVLStart_10_1	= { nullptr };
	CTexture*		m_pTextureCom_LVLStart_10_2	= { nullptr };
	CTexture*		m_pTextureCom_LVLMid_10_1	= { nullptr };
	CTexture*		m_pTextureCom_LVLMid_10_2	= { nullptr };
	CTexture*		m_pTextureCom_LVLMid_10_3	= { nullptr };

	CTexture*		m_pTextureCom_LVLEnd_Super	= { nullptr };
	CTexture*		m_pTextureCom_LVLEnd_Hot	= { nullptr };



private:
	CTexture*		m_pCurTextureCom = { nullptr };

	_float			m_fScaleMultiply = 1.f;
	_bool			m_isActive = false;

	_float			m_fActiveElapsed = 0.f;

private:
	HRESULT Ready_Components();

public:
	static CUI_ScreenText* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END
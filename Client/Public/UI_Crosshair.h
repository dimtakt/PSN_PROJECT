#pragma once

#include "Client_Defines.h"
#include "UIObject.h"

NS_BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
NS_END

NS_BEGIN(Client)

class CUI_Crosshair final : public CUIObject
{
private:
	CUI_Crosshair(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Crosshair(const CUI_Crosshair& Prototype);
	virtual ~CUI_Crosshair() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

public:
	void Change_Crosshair(_uint iTexIndex);
	void Change_RotByCD(_float fCDRatio);
	void Change_EnLarge();

private:

private:
	CShader*		m_pShaderCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

	CTexture*		m_pTextureCom_BasicHand = { nullptr };
	CTexture*		m_pTextureCom_BasicDot = { nullptr };
	CTexture*		m_pTextureCom_BasicPunch = { nullptr };
	CTexture*		m_pTextureCom_Gun = { nullptr };
	CTexture*		m_pTextureCom_Katana = { nullptr };
	CTexture*		m_pTextureCom_HotswitchHover = { nullptr };
	CTexture*		m_pTextureCom_HotswitchIdle = { nullptr };
	CTexture*		m_pTextureCom_HotswitchWait = { nullptr };

private:
	CTexture*		m_pCurTextureCom = { nullptr };	// 조건에 따라 할당해주도록 나중에 수정

	_float			m_fRotDeg = 0.f;
	_float			m_fScaleMultiply = 1.f;
	

private:
	HRESULT Ready_Components();

public:
	static CUI_Crosshair* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END
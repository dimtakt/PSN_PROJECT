#pragma once

#include "Base.h"

NS_BEGIN(Engine)

class ENGINE_DLL CFont final : public CBase
{
private:
    CFont(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual ~CFont() = default;

public:

    HRESULT Initialize_Font(         // ksta : 사용법 찾아보고, Ready 시킬 것.
        const _tchar* pFontPath);
    
    HRESULT Render_Font(
        const _tchar* pText,
        const _float2& vPosition,
        _fvector vColor,
        _float fRotation,
        const _float2& vOrigin,
        _float fScale);

public:
    static CFont* Create(
        ID3D11Device* pDevice,
        ID3D11DeviceContext* pContext,
        const _tchar* pFontPath);

    virtual void Free();

private:
	ID3D11Device*				m_pDevice = { nullptr };
	ID3D11DeviceContext*		m_pContext = { nullptr };

	SpriteFont*					m_pFont = { nullptr };
	SpriteBatch*				m_pBatch = { nullptr };
    _wstring                    m_strFontPath;


};

NS_END

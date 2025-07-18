#pragma once

#include "Base.h"

NS_BEGIN(Engine)

class CFont_Manager final : public CBase
{
private:
    CFont_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual ~CFont_Manager() = default;

public:
    HRESULT Add_Font(
        const _wstring& strFontTag, 
        const _tchar* pFontPath);

    HRESULT Render_Font_Immediately(
        const _wstring& strFontTag,
        const _tchar* pText,
        const _float2& vPosition,
        _fvector vColor,
        _float fRotation,
        const _float2& vOrigin,
        _float fScale);

    HRESULT Render_Font(
        const _wstring& strFontTag,
        const _tchar* pText,
        const _float2& vPosition,
        _fvector vColor,
        _float fRotation,
        const _float2& vOrigin,
        _float fScale);

    HRESULT Render_Begin(const _wstring& strFontTag);
    HRESULT Render_End(const _wstring& strFontTag);

private:
    class CFont* Find_Font(const wstring& strFontTag);
    map<wstring, class CFont*> m_mapFont;

private:
	ID3D11Device*				m_pDevice = { nullptr };
	ID3D11DeviceContext*		m_pContext = { nullptr };

public:
    static CFont_Manager* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual void Free() override;
};

NS_END

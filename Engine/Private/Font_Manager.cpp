#include "Font_Manager.h"
#include "Font.h"

CFont_Manager::CFont_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : m_pDevice{ pDevice }
    , m_pContext{ pContext }
{
    Safe_AddRef(m_pContext);
    Safe_AddRef(m_pDevice);
}

// ==============================

HRESULT CFont_Manager::Add_Font(const _wstring& strFontTag, const _tchar* pFontPath)
{
    if (Find_Font(strFontTag) != nullptr)
        return E_FAIL;

    CFont* pFont = CFont::Create(m_pDevice, m_pContext, pFontPath);
    NULL_CHECK_RETURN(pFont, E_FAIL);

    m_mapFont.emplace(strFontTag, pFont);

    return S_OK;
}

HRESULT CFont_Manager::Render_Font_Immediately(
    const _wstring& strFontTag,
    const _tchar* pText,
    const _float2& vPosition,
    _fvector vColor,
    _float fRotation,
    const _float2& vOrigin,
    _float fScale)
{
    CFont* pFont = Find_Font(strFontTag);
    if (nullptr == pFont)
        return E_FAIL;

    return pFont->Render_Font_Immediately(pText, vPosition, vColor, fRotation, vOrigin, fScale);
}

HRESULT CFont_Manager::Render_Font(
    const _wstring& strFontTag,
    const _tchar* pText,
    const _float2& vPosition,
    _fvector vColor,
    _float fRotation,
    const _float2& vOrigin,
    _float fScale)
{
    CFont* pFont = Find_Font(strFontTag);
    if (nullptr == pFont)
        return E_FAIL;

    return pFont->Render_Font(pText, vPosition, vColor, fRotation, vOrigin, fScale);
}

HRESULT CFont_Manager::Render_Begin(const _wstring& strFontTag)
{
    CFont* pFont = Find_Font(strFontTag);
    if (nullptr == pFont)
        return E_FAIL;

    return pFont->Render_Begin();
}

HRESULT CFont_Manager::Render_End(const _wstring& strFontTag)
{
    CFont* pFont = Find_Font(strFontTag);
    if (nullptr == pFont)
        return E_FAIL;

    return pFont->Render_End();
}

CFont* CFont_Manager::Find_Font(const wstring& strFontTag)
{
    auto iter = m_mapFont.find(strFontTag);
    return (iter != m_mapFont.end()) ? iter->second : nullptr;
}

// ==============================

CFont_Manager* CFont_Manager::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    return new CFont_Manager(pDevice, pContext);
}

void CFont_Manager::Free()
{
    __super::Free();
    
    for (auto& pair : m_mapFont)
        Safe_Release(pair.second);

    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);

    m_mapFont.clear();
}

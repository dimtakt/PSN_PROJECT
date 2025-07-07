#include "Font.h"

USING(Engine)

CFont::CFont(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : m_pDevice{ pDevice }
    , m_pContext{ pContext }
{
    Safe_AddRef(m_pContext);
    Safe_AddRef(m_pDevice);
}

// ==============================

HRESULT CFont::Initialize_Font(
    const _tchar* pFontPath)
{
    // https://github.com/microsoft/DirectXTK/wiki/SpriteFont

    m_pFont = new SpriteFont(m_pDevice, pFontPath);
    m_pBatch = new SpriteBatch(m_pContext);

    return S_OK;
}

HRESULT CFont::Render_Font(
    const _tchar* pText,
    const _float2& vPosition,
    _fvector vColor,
    _float fRotation,
    const _float2& vOrigin,
    _float fScale)
{   
    m_pBatch->Begin();
    m_pFont->DrawString(m_pBatch, pText, vPosition, vColor, fRotation, vOrigin, fScale);
    m_pBatch->End();

    return S_OK;
}

// ==============================

CFont* CFont::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pFontPath)
{
    CFont* pInstance = new CFont(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Font(pFontPath)))
    {
        MSG_BOX(L"Failed to Created : CFont");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CFont::Free()
{
    __super::Free();

    Safe_Delete(m_pFont);
    Safe_Delete(m_pBatch);

    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);
}

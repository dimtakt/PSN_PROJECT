#include "Font.h"
#include "Font_Manager.h"

USING(Engine)

CFont::CFont(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : m_pDevice{ pDevice }
    , m_pContext{ pContext }
{
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
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

HRESULT CFont::Render_Font_Immediately(
    const _tchar* pText,
    const _float2& vPosition,
    _fvector vColor,
    _float fRotation,
    const _float2& vOrigin,
    _float fScale)
{   
    // https://github.com/Microsoft/DirectXTK/wiki/SpriteBatch#state-management
    // Begin, End 사용 시 렌더 상태를 저장/복원하는 과정이 없기에
    // 이를 직접 수정해주어야 함.

    Render_Begin();

    m_pFont->DrawString(m_pBatch, pText, vPosition, vColor, fRotation, vOrigin, fScale);

    Render_End();

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
    m_pFont->DrawString(m_pBatch, pText, vPosition, vColor, fRotation, vOrigin, fScale);

    return S_OK;
}

HRESULT CFont::Render_Begin()
{
    // 현재 렌더상태 저장
    m_pContext->OMGetBlendState(&pOldBlendState, blendFactor, &sampleMask);
    m_pContext->OMGetDepthStencilState(&pOldDepthStencilState, &stencilRef);
    m_pContext->RSGetState(&pOldRasterizerState);

    // Geometry Shader 클리어
    m_pContext->GSSetShader(nullptr, nullptr, 0);

    // Batch Begin
    m_pBatch->Begin();

    return S_OK;
}

HRESULT CFont::Render_End()
{
    // Batch End
    m_pBatch->End();

    // 상태 복원
    m_pContext->OMSetBlendState(pOldBlendState, blendFactor, sampleMask);
    m_pContext->OMSetDepthStencilState(pOldDepthStencilState, stencilRef);
    m_pContext->RSSetState(pOldRasterizerState);

    // Release
    if (pOldBlendState)         pOldBlendState->Release();
    if (pOldDepthStencilState)  pOldDepthStencilState->Release();
    if (pOldRasterizerState)    pOldRasterizerState->Release();

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

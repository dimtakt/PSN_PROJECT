#include "Animation.h"
#include "Channel.h"

CAnimation::CAnimation()
{
    /*   XMMatrixDecompose(스케일, 로테이션, 이동, 행렬);*/
}

CAnimation::CAnimation(const CAnimation& Prototype)
    : m_strAnimName(Prototype.m_strAnimName)
    , m_fDuration(Prototype.m_fDuration)
    , m_fTickPerSecond(Prototype.m_fTickPerSecond)
    , m_fCurrentTrackPosition(Prototype.m_fCurrentTrackPosition)
    , m_iNumChannels(Prototype.m_iNumChannels)
    , m_Channels(Prototype.m_Channels)
    , m_CurrentKeyFrameIndices(Prototype.m_CurrentKeyFrameIndices)
{
    for (auto& pChannel : m_Channels)
        Safe_AddRef(pChannel);
}

HRESULT CAnimation::Initialize(const aiAnimation* pAIAnimation, const vector<class CBone*>& Bones)
{
    // 이름
    const _char* szName = pAIAnimation->mName.data;
    _int len = MultiByteToWideChar(CP_UTF8, 0, szName, -1, nullptr, 0);
    _wstring strName(len, 0);
    MultiByteToWideChar(CP_UTF8, 0, szName, -1, &strName[0], len);
    if (!strName.empty() && strName.back() == L'\0')
        strName.pop_back();

    m_strAnimName = strName;

    m_fDuration = pAIAnimation->mDuration;
    m_fTickPerSecond = pAIAnimation->mTicksPerSecond;

    m_iNumChannels = pAIAnimation->mNumChannels;

    m_CurrentKeyFrameIndices.resize(m_iNumChannels);

    for (size_t i = 0; i < m_iNumChannels; i++)
    {
        CChannel* pChannel = CChannel::Create(pAIAnimation->mChannels[i], Bones);
        if (nullptr == pChannel)
            return E_FAIL;

        m_Channels.push_back(pChannel);
    }

    return S_OK;
}

HRESULT CAnimation::Initialize_Binary(const AIANIM_DESC tAnimDesc, const vector<class CBone*>& Bones)
{
    m_strAnimName = ConvertCharToWString(tAnimDesc.szAnimName.C_Str());

    m_fDuration = tAnimDesc.fDuration;
    m_fTickPerSecond = tAnimDesc.fTicksPerSecond;

    m_iNumChannels = tAnimDesc.iNumChannels;

    m_CurrentKeyFrameIndices.resize(m_iNumChannels);

    for (size_t i = 0; i < m_iNumChannels; i++)
    {
        CChannel* pChannel = CChannel::Create_Binary(tAnimDesc.vecChannels[i], Bones);
        if (nullptr == pChannel)
            return E_FAIL;

        m_Channels.push_back(pChannel);
    }

    return S_OK;
}

void CAnimation::Update_TransformationMatrices(const vector<class CBone*>& Bones, _bool isLoop, _bool* pFinished, _float fTimeDelta, _float fBlendRatio)
{
    m_fCurrentTrackPosition += m_fTickPerSecond * fTimeDelta;
    m_isFinishedLoop = false;

    if (m_fCurrentTrackPosition >= m_fDuration)
    {

        if (false == isLoop)
        {
            *pFinished = true;
            m_fCurrentTrackPosition = m_fDuration;
            return;
        }
        else
        {
            m_isFinishedLoop = true;
            m_fCurrentTrackPosition = 0.f;
        }

    }


    for (_uint i = 0; i < m_iNumChannels; ++i)
    {
        m_Channels[i]->Update_TransformationMatrix(Bones, m_fCurrentTrackPosition, &m_CurrentKeyFrameIndices[i], fBlendRatio);
    }
}

CAnimation* CAnimation::Create(const aiAnimation* pAIAnimation, const vector<class CBone*>& Bones)
{
    CAnimation* pInstance = new CAnimation();

    if (FAILED(pInstance->Initialize(pAIAnimation, Bones)))
    {
        MSG_BOX(TEXT("Failed to Created : CAnimation"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CAnimation* CAnimation::Create_Binary(const AIANIM_DESC tAnimDesc, const vector<class CBone*>& Bones)
{
    CAnimation* pInstance = new CAnimation();

    if (FAILED(pInstance->Initialize_Binary(tAnimDesc, Bones)))
    {
        MSG_BOX(TEXT("Failed to Created : CAnimation with Binary"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CAnimation* CAnimation::Clone()
{
    return new CAnimation(*this);
}

void CAnimation::Free()
{
    __super::Free();

    for (auto& pChannel : m_Channels)
        Safe_Release(pChannel);

    m_Channels.clear();
}

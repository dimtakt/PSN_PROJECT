#include "Channel.h"
#include "Bone.h"

CChannel::CChannel()
{
}

HRESULT CChannel::Initialize(const aiNodeAnim* pAIChannel, const vector<class CBone*>& Bones)
{
    auto	iter = find_if(Bones.begin(), Bones.end(), [&](CBone* pBone)->_bool
        {
            if (true == pBone->Compare_Name(pAIChannel->mNodeName.data))
                return true;

            m_iBoneIndex++;

            return false;
        });

    m_iNumKeyFrames = max(pAIChannel->mNumScalingKeys, pAIChannel->mNumRotationKeys);
    m_iNumKeyFrames = max(m_iNumKeyFrames, pAIChannel->mNumPositionKeys);

    _float3     vScale{};
    _float4     vRotation{};
    _float3     vTranslation{};

    for (size_t i = 0; i < m_iNumKeyFrames; i++)
    {
        KEYFRAME            KeyFrame{};

        if (i < pAIChannel->mNumScalingKeys)
        {
            memcpy(&vScale, &pAIChannel->mScalingKeys[i].mValue, sizeof(_float3));
            KeyFrame.fTrackPosition = pAIChannel->mScalingKeys[i].mTime;
        }

        if (i < pAIChannel->mNumRotationKeys)
        {
            vRotation.x = pAIChannel->mRotationKeys[i].mValue.x;
            vRotation.y = pAIChannel->mRotationKeys[i].mValue.y;
            vRotation.z = pAIChannel->mRotationKeys[i].mValue.z;
            vRotation.w = pAIChannel->mRotationKeys[i].mValue.w;

            KeyFrame.fTrackPosition = pAIChannel->mRotationKeys[i].mTime;
        }

        if (i < pAIChannel->mNumPositionKeys)
        {
            memcpy(&vTranslation, &pAIChannel->mPositionKeys[i].mValue, sizeof(_float3));
            KeyFrame.fTrackPosition = pAIChannel->mPositionKeys[i].mTime;
        }

        KeyFrame.vScale = vScale;
        KeyFrame.vRotation = vRotation;
        KeyFrame.vTranslation = vTranslation;

        m_KeyFrames.push_back(KeyFrame);
    }


    return S_OK;
}

HRESULT CChannel::Initialize_Binary(const AICHANNEL_DESC tChanDesc, const vector<class CBone*>& Bones)
{
    // 원본 initialize 처럼 어딘가에서 본 인덱스 정보를 받아와야 할듯
    // initialize에서 만들어진 것 꺼내와서 저장한 뒤 나중에 불러오면 되는거 아님?
    // 그냥 애님에ㅣㅅ션에서 넣는 넙ㅂ 잇으러ㄱ같은데
    
    m_iBoneIndex = tChanDesc.iBoneIndex;

    m_iNumKeyFrames = tChanDesc.vecKeyFrame.size();
    m_KeyFrames = tChanDesc.vecKeyFrame;

    return S_OK;
}

void CChannel::Update_TransformationMatrix(const vector<class CBone*>& Bones, _float fCurrentTrackPosition, _uint* pCurrentKeyFrameIndex, _float fBlendRatio, _bool isSameAnim, void* pArg)
{
    if (fCurrentTrackPosition == 0.f)
        *pCurrentKeyFrameIndex = 0;

    CHANNEL_UPD_DESC* pDesc = {};
    if (pArg != nullptr)
        pDesc = static_cast<CHANNEL_UPD_DESC*>(pArg);

    /* 선택된 애니메이션이 이용하고 있는 이 뼈(Channel)의 현재 재생된 위치(fCurrrentTrackPosition)에 맞는 상태행렬을 만들어 준다. */
    _vector         vScale, vRotation, vTranslation;

    /* 마지막 키프레임상태. */
    KEYFRAME        LastKeyFrame = m_KeyFrames.back();

    /* Transform 을 반영시킬 본의 정보 */
    CBone* pTargetBone = Bones[m_iBoneIndex];


    if (fCurrentTrackPosition >= LastKeyFrame.fTrackPosition)
    {
        vScale = XMLoadFloat3(&LastKeyFrame.vScale);
        vRotation = XMLoadFloat4(&LastKeyFrame.vRotation);
        vTranslation = XMVectorSetW(XMLoadFloat3(&LastKeyFrame.vTranslation), 1.f);
    }
    /* 양쪽 키프레임사이에서의 중간상태를 보간하여 만든다. */
    else
    {
        while (fCurrentTrackPosition >= m_KeyFrames[*pCurrentKeyFrameIndex + 1].fTrackPosition)
            ++*pCurrentKeyFrameIndex;

        _uint iDestKeyFrameIndex = *pCurrentKeyFrameIndex + 1;

        _vector    vSourScale, vDestScale;
        _vector    vSourRotation, vDestRotation;
        _vector    vSourTranslation, vDestTranslation;

        vSourScale = XMLoadFloat3(&m_KeyFrames[*pCurrentKeyFrameIndex].vScale);
        vSourRotation = XMLoadFloat4(&m_KeyFrames[*pCurrentKeyFrameIndex].vRotation);
        vSourTranslation = XMVectorSetW(XMLoadFloat3(&m_KeyFrames[*pCurrentKeyFrameIndex].vTranslation), 1.f);

        vDestScale = XMLoadFloat3(&m_KeyFrames[iDestKeyFrameIndex].vScale);
        vDestRotation = XMLoadFloat4(&m_KeyFrames[iDestKeyFrameIndex].vRotation);
        vDestTranslation = XMVectorSetW(XMLoadFloat3(&m_KeyFrames[iDestKeyFrameIndex].vTranslation), 1.f);

        _float      fRatio = (fCurrentTrackPosition - m_KeyFrames[*pCurrentKeyFrameIndex].fTrackPosition) / (m_KeyFrames[iDestKeyFrameIndex].fTrackPosition - m_KeyFrames[*pCurrentKeyFrameIndex].fTrackPosition);

        vScale = XMVectorLerp(vSourScale, vDestScale, fRatio);  
        vRotation = XMQuaternionSlerp(vSourRotation, vDestRotation, fRatio);
        vTranslation = XMVectorSetW(XMVectorLerp(vSourTranslation, vDestTranslation, fRatio), 1.f);
    }



    /* 애니메이션이 반영 및 계산된 최종 Transform 정보를, 인자로 받아온 Blend 가중치에 따라 재계산 (애니메이션 전환 시 블렌딩 효과) */
    if (fBlendRatio <= 1.f)
    {
#pragma region 포기한흔적
        //if (!isSameAnim)    // 다른 애니메이션일 경우.
        //{
        //    _float fAnimBlendRatio = fBlendRatio;               // [이쪽이 새로 반영될 Transform] 의 반영 비율
        //    _float fExistBlendRatio = 1.f - fAnimBlendRatio;    // [이쪽이 기존의 뼈대 Transform] 의 반영 비율


        //    // [이전 애니메이션이 적용된] 기존 뼈대의 Translation, Rotation, Scale 정보를 받아와서
        //    // Blend 수치에 맞게 반영하여 lerp 한 뒤 재반영시킴
        //    _vector vExistTranslation = {}, vExistRotation = {}, vExistScale = {};
        //    XMMatrixDecompose(&vExistScale, &vExistRotation, &vExistTranslation, pTargetBone->Get_TransformationMatrix());

        //    vScale = XMVectorLerp(vExistScale, vScale, fAnimBlendRatio);
        //    vRotation = XMQuaternionSlerp(vExistRotation, vRotation, fAnimBlendRatio);
        //    vTranslation = XMVectorSetW(XMVectorLerp(vExistTranslation, vTranslation, fAnimBlendRatio), 1.f);
        //}
        //else                // 같은 애니메이션일 경우
        //{
        //    // 1. 애니메이션 종료되기 [TransitionTime] 전부터 블렌딩 시작 필요
        //    // 2. 블렌딩 시 선형적으로 보간되도록
        //    //
        //    // - 문제)
        //    // 종료로부터 해당 시간 이전 시점의 키프레임을 어떻게 계산하는가
        //    // - 필요한것)
        //    // 종료로부터 해당 시간 이전 시점의 키프레임 에서 "이전 시간". Model 단계서부터 인자로 받아와야 하나?
        //    // 애니메이션의 총 길이. 애니메이션쪽에 [m_fDuration] 으로 있음
        //    // 현재 키프레임이 어디쯤인지 정보.  fCurrentTrackPosition 와 애니메이션의 총 길이를 이용하여 구할 수 있을듯
        //    // 블렌딩 대상의 키프레임이 어디쯤인지 정보. 위와 동일
        //    // - 그래서 그걸 어떻게 구함?)
        //    // 현재 키프레임이 어디쯤인지 정보 : 102번줄과 같이 계산
        //    // 블렌딩 대상의 키프레임 정보 : m_fDuration 에서 102번줄 계산 결과를 빼고, 보정하면 될 듯
        //    // - 이걸로 뭘 하면 됨?)
        //    // [isSameAnim == true] 인 동안, "이전 시간" 시점부터,
        //    // 신규 애니메이션은 0부터, 기존 애니메이션은 "이전 시간"부터
        //    // 시간 경과에 따라 0:1 부터 블렌딩 진행
        //    // 
        //
        //    _float fAnimDuration = pDesc->fAnimDuration;      // tick 단위
        //    _float fTransitionTimeSec = pDesc->fTransitionTime;    // 초 단위
        //    _float fTickPerSecond = pDesc->fTickPerSecond;     // tick/s
        //
        //    // 애니메이션 지속 길이를 m_fDuration - fTranslationTime 로 축소함,
        //    // 애니메이션 루프 중 loop 이후 fTransitionTimeSec 동안만 블렌딩할 것
        //    // isSameAnim 조건이므로, 애니메이션 2회차 반복 시작 시 부터 적용됨
        //    // isSameAnim 이 True 로 바뀌는 타이밍 문제려나.. 1->2 는 안되는데 2->3 부터는 잘됨

        //    _float fTransitionTicks = fTransitionTimeSec * fTickPerSecond;  // 트랜지션이 일어날 Tick 기간

        //    if (fCurrentTrackPosition <= fTransitionTicks)
        //    {
        //        _float fLocalBlendRatio = fCurrentTrackPosition / fTransitionTicks;
        //        if (fLocalBlendRatio > 1.f) fLocalBlendRatio = 1.f;

        //        _vector vExistScale, vExistRot, vExistTrans;
        //        XMMatrixDecompose(&vExistScale, &vExistRot, &vExistTrans, pTargetBone->Get_TransformationMatrix());

        //        vScale = XMVectorLerp(vExistScale, vScale, fLocalBlendRatio);
        //        vRotation = XMQuaternionSlerp(vExistRot, vRotation, fLocalBlendRatio);
        //        vTranslation = XMVectorSetW(XMVectorLerp(vExistTrans, vTranslation, fLocalBlendRatio), 1.f);
        //    }
        //    // 블렌딩 구간이 아니라면 그냥 현재 Transform 그대로 적용
        //}
#pragma endregion
        // 야매로함
        if (pArg != nullptr)
        {
            _float fAnimDuration = pDesc->fAnimDuration;      // tick 단위
            _float fTransitionTimeSec = pDesc->fTransitionTime;    // 초 단위
            _float fTickPerSecond = pDesc->fTickPerSecond;     // tick/s

            // 애니메이션 지속 길이를 m_fDuration - fTranslationTime 로 축소함,
            // 애니메이션 루프 중 loop 이후 fTransitionTimeSec 동안만 블렌딩할 것
            // isSameAnim 조건이므로, 애니메이션 2회차 반복 시작 시 부터 적용됨
            // isSameAnim 이 True 로 바뀌는 타이밍 문제려나.. 1->2 는 안되는데 2->3 부터는 잘됨

            _float fTransitionTicks = fTransitionTimeSec * fTickPerSecond;  // 트랜지션이 일어날 Tick 기간

            if (fCurrentTrackPosition <= fTransitionTicks)
            {
                _float fLocalBlendRatio = fCurrentTrackPosition / fTransitionTicks;
                if (fLocalBlendRatio > 1.f) fLocalBlendRatio = 1.f;

                _vector vExistScale, vExistRot, vExistTrans;
                XMMatrixDecompose(&vExistScale, &vExistRot, &vExistTrans, pTargetBone->Get_TransformationMatrix());

                vScale = XMVectorLerp(vExistScale, vScale, fLocalBlendRatio);
                vRotation = XMQuaternionSlerp(vExistRot, vRotation, fLocalBlendRatio);
                vTranslation = XMVectorSetW(XMVectorLerp(vExistTrans, vTranslation, fLocalBlendRatio), 1.f);
            }
            // 블렌딩 구간이 아니라면 그냥 현재 Transform 그대로 적용
        }
        else
        {
            _float fAnimBlendRatio = fBlendRatio;               // [이쪽이 새로 반영될 Transform] 의 반영 비율
            _float fExistBlendRatio = 1.f - fAnimBlendRatio;    // [이쪽이 기존의 뼈대 Transform] 의 반영 비율


            // [이전 애니메이션이 적용된] 기존 뼈대의 Translation, Rotation, Scale 정보를 받아와서
            // Blend 수치에 맞게 반영하여 lerp 한 뒤 재반영시킴
            _vector vExistTranslation = {}, vExistRotation = {}, vExistScale = {};
            XMMatrixDecompose(&vExistScale, &vExistRotation, &vExistTranslation, pTargetBone->Get_TransformationMatrix());

            vScale = XMVectorLerp(vExistScale, vScale, fAnimBlendRatio);
            vRotation = XMQuaternionSlerp(vExistRotation, vRotation, fAnimBlendRatio);
            vTranslation = XMVectorSetW(XMVectorLerp(vExistTranslation, vTranslation, fAnimBlendRatio), 1.f);
        }


    }
    else
    {
        int i = 1;
    }



    /*_matrix         TransformationMatrix = XMMatrixScaling() * XMMatrixRotationQuaternion() * XMMatrixTranslation();*/
    _matrix         TransformationMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vTranslation);

    pTargetBone->Set_TransformationMatrix(TransformationMatrix);
}

CChannel* CChannel::Create(const aiNodeAnim* pAIChannel, const vector<class CBone*>& Bones)
{
    CChannel* pInstance = new CChannel();

    if (FAILED(pInstance->Initialize(pAIChannel, Bones)))
    {
        MSG_BOX(TEXT("Failed to Created : CChannel"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CChannel* CChannel::Create_Binary(const AICHANNEL_DESC tChanDesc, const vector<class CBone*>& Bones)
{
    CChannel* pInstance = new CChannel();

    if (FAILED(pInstance->Initialize_Binary(tChanDesc, Bones)))
    {
        MSG_BOX(TEXT("Failed to Created : CChannel with Binary"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CChannel::Free()
{
}


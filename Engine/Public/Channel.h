#pragma once

#include "Base.h"

/* 시간에 따른 뼈의 상태행렬들을 보관한다. */

NS_BEGIN(Engine)

class CChannel final : public CBase
{
public:
	typedef struct tChannelUpdateDesc
	{
		_float	fTransitionTime;
		_float	fAnimDuration;
		_float	fTickPerSecond;
	}CHANNEL_UPD_DESC;

private:
	CChannel();
	virtual ~CChannel() = default;

public:
	HRESULT Initialize(const aiNodeAnim* pAIChannel, const vector<class CBone*>& Bones);
	HRESULT Initialize_Binary(const AICHANNEL_DESC tChanDesc, const vector<class CBone*>& Bones);
	void Update_TransformationMatrix(const vector<class CBone*>& Bones, _float fCurrentTrackPosition, _uint* pCurrentKeyFrameIndex, _float fBlendRatio = 1.f, _bool isSameAnim = false, void* pArg = nullptr);

public:
	_uint Get_UsingBoneIndex() { return m_iBoneIndex;  }

private:
	_char							m_szName[MAX_PATH] = { };

	_uint							m_iBoneIndex = {};

	_uint							m_iNumKeyFrames = {};
	vector<KEYFRAME>				m_KeyFrames;

public:
	static CChannel* Create(const aiNodeAnim* pAIChannel, const vector<class CBone*>& Bones);
	static CChannel* Create_Binary(const AICHANNEL_DESC tChanDesc, const vector<class CBone*>& Bones);
	virtual void Free() override;
};

NS_END


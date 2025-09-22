#pragma once
#include "Engine_Defines.h"
#include "Base.h"

NS_BEGIN(Engine)

class CSound_Manager final : public CBase
{
private:
	CSound_Manager();
	~CSound_Manager() = default;

public:
	HRESULT Initialize();

public:
	void PlaySoundW(const TCHAR* pSoundKey, _uint SoundChannel, float fVolume);
	void PlayLoopSound(const TCHAR* pSoundKey, _uint SoundChannel, float fVolume);
	void PlayBGM(const TCHAR* pSoundKey, float fVolume);
	void StopSound(_uint SoundChannel);
	void StopAll();
	void SetChannelVolume(_uint SoundChannel, float fVolume);
	bool IsPlaying(_uint SoundChannel);

private:
	void LoadSoundFile();

public:
	static CSound_Manager* Create();
	virtual void Free() override;

private:

	// 사운드 리소스 정보를 갖는 객체 
	map<TCHAR*, FMOD_SOUND*> m_mapSound;

	// FMOD_CHANNEL : 재생하고 있는 사운드를 관리할 객체 
	FMOD_CHANNEL* m_pChannelArr[MAXCHANNEL];

	// 사운드 ,채널 객체 및 장치를 관리하는 객체 
	FMOD_SYSTEM* m_pSystem;
};

NS_END
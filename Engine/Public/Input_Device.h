#ifndef InputDev_h__
#define InputDev_h__

#include "Base.h"

NS_BEGIN(Engine)

class  CInput_Device : public CBase
{
private:
	CInput_Device(void);
	virtual ~CInput_Device(void) = default;

public:
	_byte	Get_DIKeyState(_ubyte byKeyID)
	{
		return m_byKeyState[byKeyID];
	}

	_byte	Get_DIMouseState(MOUSEKEYSTATE eMouse)
	{
		return m_tCurMouseState.rgbButtons[static_cast<_uint>(eMouse)];
	}

	// 현재 마우스의 특정 축 좌표를 반환
	_long	Get_DIMouseMove(MOUSEMOVESTATE eMouseState)
	{
		/*	switch (eMouseState)
			{
			case X:
				return m_tMouseState.lX;
			case Y:
				return m_tMouseState.lY;
			case WHEEL:
				return m_tMouseState.lZ;
			}*/

		return *((reinterpret_cast<_int*>(&m_tCurMouseState)) + static_cast<_uint>(eMouseState));
	}


	// ksta : 확인 필요
	_bool	Get_IsKeyDown(_ubyte byKeyID)
	{
		auto itPre = m_mPreState.find(byKeyID);
		auto itCur = m_mCurState.find(byKeyID);
		return itCur != m_mCurState.end() && itCur->second && (!itPre->second);
	};
	_bool	Get_IsKeyDown(MOUSEKEYSTATE eMouse)
	{
		auto itPre = m_mPreMouseState.find(ENUM_CLASS(eMouse));
		auto itCur = m_mCurMouseState.find(ENUM_CLASS(eMouse));
		return itCur != m_mCurMouseState.end() && itCur->second && (!itPre->second);
	}
	_bool	Get_IsKeyUp(_ubyte byKeyID)
	{
		auto itPre = m_mPreState.find(byKeyID);
		auto itCur = m_mCurState.find(byKeyID);
		return itCur != m_mCurState.end() && !itCur->second && itPre->second;
	}
	_bool	Get_IsKeyUp(MOUSEKEYSTATE eMouse)
	{
		auto itPre = m_mPreMouseState.find(ENUM_CLASS(eMouse));
		auto itCur = m_mCurMouseState.find(ENUM_CLASS(eMouse));
		return itCur != m_mCurMouseState.end() && !itCur->second && itPre->second;
	}

	_bool	Get_IsKeyPressing(_ubyte byKeyID)
	{
		auto itCur = m_mCurState.find(byKeyID);
		return itCur != m_mCurState.end() && itCur->second;
	}
	_bool	Get_IsKeyPressing(MOUSEKEYSTATE eMouse)
	{
		auto itCur = m_mCurState.find(ENUM_CLASS(eMouse));
		return itCur != m_mCurState.end() && itCur->second;
	}




public:
	HRESULT Initialize(HINSTANCE hInst, HWND hWnd);
	void	Update(void);

private:
	LPDIRECTINPUT8			m_pInputSDK = { nullptr };
	map<_int, _bool>		m_mPreState = {};
	map<_int, _bool>		m_mCurState = {};
	map<_int, _bool>		m_mPreMouseState = {};	// 눌렸는지 등
	map<_int, _bool>		m_mCurMouseState = {};	// 눌렸는지 등

	//unordered_map<_int, _float>	m_mKeyHoldTime = {};

private:
	LPDIRECTINPUTDEVICE8	m_pKeyBoard = { nullptr };
	LPDIRECTINPUTDEVICE8	m_pMouse = { nullptr };
	//LPDIRECTINPUTDEVICE8	m_pJoystick = { nullptr };


private:
	_byte					m_byKeyState[256] = {};
	DIMOUSESTATE			m_tPreMouseState = {};	// 마우스의 위치
	DIMOUSESTATE			m_tCurMouseState = {};	// 마우스의 위치

public:
	static CInput_Device* Create(HINSTANCE hInstance, HWND hWnd);
	virtual void Free(void);

};

NS_END
#endif // InputDev_h__



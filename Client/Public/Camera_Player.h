#pragma once

#include "Client_Defines.h"
//#include "../../Engine/Public/Camera.h"
#include "Camera.h"

NS_BEGIN(Client)

class CCamera_Player final : public CCamera
{
public:
	typedef struct tagCameraFreeDesc : public CCamera::CAMERA_DESC
	{
		_float			fMouseSensor;
	}CAMERA_PLAYER_DESC;

private:
	CCamera_Player(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCamera_Player(const CCamera_Player& Prototype);
	virtual ~CCamera_Player() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	void Cursor_Lock();

private:
	_float			m_fMouseSensor = { };

	_bool			m_isFreeMode = false;
	CTransform*		m_pPlayerTransformCom = { nullptr };

	_float			m_fOriginFovy = {};

private:
	_float m_fPitch = 0.f;   // 상하 회전 값

public:
	static CCamera_Player* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg)override;
	virtual void Free() override;

};

NS_END
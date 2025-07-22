#pragma once

#include "Client_Defines.h"
#include "Camera.h"

NS_BEGIN(Client)

class CCamera_Editor final : public CCamera
{
public:
	typedef struct tagCameraEditorDesc : public CCamera::CAMERA_DESC
	{
		_float			fMouseSensor;
	}CAMERA_EDITOR_DESC;

private:
	CCamera_Editor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCamera_Editor(const CCamera_Editor& Prototype);
	virtual ~CCamera_Editor() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	_float			m_fMouseSensor = { };

public:
	static CCamera_Editor* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg)override;
	virtual void Free() override;

};

NS_END
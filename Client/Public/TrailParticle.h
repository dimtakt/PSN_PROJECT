#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class CShader;
class CTexture;
//class CVIBuffer_Point_Instance;
class CVIBuffer_Rect_Instance;
NS_END

NS_BEGIN(Client)

class CTrailParticle final : public CGameObject
{
public:
	typedef struct tTrailDesc {
		CGameObject* pOwner = nullptr;
	} TRAIL_DESC;

private:
	CTrailParticle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTrailParticle(const CTrailParticle& Prototype);
	virtual ~CTrailParticle() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

private:
	CShader*					m_pShaderCom	= { nullptr };
	CTexture*					m_pTextureCom	= { nullptr };
	//CVIBuffer_Point_Instance*	m_pVIBufferCom	= { nullptr };
	CVIBuffer_Rect_Instance*	m_pVIBufferCom	= { nullptr };

private:
	_float				m_fElapsedTime = {};
	const _float		m_fMaxTime = 1.f;

	CGameObject*		m_pTargetObj = nullptr;

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CTrailParticle* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END
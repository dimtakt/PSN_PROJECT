#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

NS_BEGIN(Engine)

class CShader;
class CModel;

NS_END

NS_BEGIN(Client)

class CProps_ServerRack1 final : public CGameObject
{
private:
	CProps_ServerRack1(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CProps_ServerRack1(const CGameObject& Prototype);
	virtual ~CProps_ServerRack1() = default;

public:
	HRESULT		Initialize_Prototype()				override;
	HRESULT		Initialize(void* pArg)				override;
	void		Priority_Update(_float fTimeDelta)	override;
	void		Update(_float fTimeDelta)			override;
	void		Late_Update(_float fTimeDelta)		override;
	HRESULT		Render()							override;

private:
	// 로컬 함수들 (기능 분리)
	HRESULT		Ready_Components(void* pArg);
	HRESULT		Bind_ShaderResources();
private:
	// 로컬 변수들 (타입, 컴포넌트 등..)
	GAMEOBJ_TYPE	m_eGameObjType = GAMEOBJ_TYPE::STATIC_PROPS;

	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };

public:
	static CProps_ServerRack1* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CProps_ServerRack1* Clone(void* pArg)		override;
	void					Free()					override;


};

NS_END
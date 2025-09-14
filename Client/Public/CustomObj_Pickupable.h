#pragma once

#include "Client_Defines.h"
#include "CustomObj_NonAnim.h"


NS_BEGIN(Engine)

class CShader;
class CModel;

NS_END

NS_BEGIN(Client)

class CCustomObj_Pickupable final : public CCustomObj_NonAnim
{
private:
	CCustomObj_Pickupable(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCustomObj_Pickupable(const CGameObject& Prototype);
	virtual ~CCustomObj_Pickupable() = default;

public:
	HRESULT		Initialize_Prototype()				override;
	HRESULT		Initialize(void* pArg)				override;
	void		Priority_Update(_float fTimeDelta)	override;
	void		Update(_float fTimeDelta)			override;
	void		Late_Update(_float fTimeDelta)		override;
	HRESULT		Render()							override;

	void		OnCollisionRay()					override;

private:
	// 로컬 함수들 (기능 분리)
	HRESULT		Ready_Components(void* pArg);
	HRESULT		Bind_ShaderResources();

private:
	// 로컬 변수들 (타입, 컴포넌트 등..)


public:
	static CCustomObj_Pickupable*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCustomObj_Pickupable*			Clone(void* pArg)		override;
	void							Free()					override;


};

NS_END

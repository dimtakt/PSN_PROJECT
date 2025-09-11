#pragma once

#include "CustomObj.h"

NS_BEGIN(Engine)

class CShader;
class CModel;

NS_END

NS_BEGIN(Engine)

class ENGINE_DLL CCustomObj_NonAnim : public CCustomObj
{
protected:
	CCustomObj_NonAnim(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCustomObj_NonAnim(const CGameObject& Prototype);
	virtual ~CCustomObj_NonAnim() = default;

public:
	HRESULT		Initialize_Prototype()				override;
	HRESULT		Initialize(void* pArg)				override;
	void		Priority_Update(_float fTimeDelta)	override;
	void		Update(_float fTimeDelta)			override;
	void		Late_Update(_float fTimeDelta)		override;
	HRESULT		Render()							override;

protected:
	// 로컬 함수들 (기능 분리)
	HRESULT		Ready_Components(void* pArg);
	HRESULT		Bind_ShaderResources();
protected:
	// 로컬 변수들 (타입, 컴포넌트 등..)
	CShader*		m_pShaderCom	= { nullptr };	
	CModel*			m_pModelCom		= { nullptr };

public:
	static CCustomObj_NonAnim*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCustomObj_NonAnim*			Clone(void* pArg)		override;
	void						Free()					override;


};

NS_END
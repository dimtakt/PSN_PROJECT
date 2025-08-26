#pragma once

#include "GameObject.h"

NS_BEGIN(Engine)

class ENGINE_DLL CCustomObj abstract : public CGameObject
{
public:
	typedef struct customObjectDesc : public GAMEOBJECT_DESC
	{
		_wstring		strModelComPrototypeTag;

	}CUSTOMOBJ_DESC;
protected:
	CCustomObj(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCustomObj(const CGameObject& Prototype);
	virtual ~CCustomObj() = default;

public:
	virtual HRESULT		Initialize_Prototype()				override;
	virtual HRESULT		Initialize(void* pArg)				override;
	virtual void		Priority_Update(_float fTimeDelta)	override;
	virtual void		Update(_float fTimeDelta)			override;
	virtual void		Late_Update(_float fTimeDelta)		override;
	virtual HRESULT		Render()							override;

public:
	virtual CCustomObj* Clone(void* pArg) = 0;
	virtual void		Free()					override;


};

NS_END
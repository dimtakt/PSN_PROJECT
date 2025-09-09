#pragma once

#include "Client_Defines.h"
#include "PartObject.h"

#include "Player.h"

NS_BEGIN(Engine)
class CShader;
class CModel;
NS_END

NS_BEGIN(Client)

class CWeapon abstract : public CPartObject
{
public:
	typedef struct tagWeaponDesc : public CPartObject::PARTOBJECT_DESC
	{
		const _float4x4* pSocketMatrix = { nullptr };
		_uint* pState = { nullptr };

		CGameObject* pParentTarget = { nullptr };
	}WEAPON_DESC;
protected:
	CWeapon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CWeapon(const CWeapon& Prototype);
	virtual ~CWeapon() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

protected:
	_uint				m_iGameObjType = { };

	vector<CCollider*>	m_vecCollidersCom[ENUM_CLASS(COLLIDERTYPE::END)] = { };
	CShader*			m_pShaderCom = { nullptr };
	CModel*				m_pModelCom = { nullptr };

protected:
	const _float4x4*	m_pSocketMatrix = { nullptr };
	_uint*				m_pParentState = { nullptr };
	CGameObject*		m_pParentTarget = { nullptr };

//protected:
//	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CWeapon* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

NS_END
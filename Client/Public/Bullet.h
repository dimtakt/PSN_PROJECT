#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

NS_BEGIN(Engine)

class CCollider;

NS_END


NS_BEGIN(Client)

class CBullet final : public CGameObject
{
public:
	typedef struct tagBulletDesc : public GAMEOBJECT_DESC
	{
		_matrix matSpawnTransform;
		_vector vMoveDir;
	}Bullet_DESC;
private:
	CBullet(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBullet(const CBullet& Prototype);
	virtual ~CBullet() = default;

public:
	HRESULT		Initialize_Prototype()				override;
	HRESULT		Initialize(void* pArg)				override;
	void		Priority_Update(_float fTimeDelta)	override;
	void		Update(_float fTimeDelta)			override;
	void		Late_Update(_float fTimeDelta)		override;
	HRESULT		Render()							override;

public:
	// 퍼블릭 함수들 (Get/Set 등..)
	// ..

private:
	// 로컬 함수들 (기능 분리)
	HRESULT		Ready_Components(void* pArg);
	HRESULT		Bind_ShaderResources();

	_bool		Check_Collision(_uint iTargetType);
	void		Check_Destroy(_float fTimeDelta);

private:
	// 로컬 변수들 (타입, 컴포넌트 등..)
	_uint				m_iGameObjType = { };
	_vector				m_vMoveDir = { };

	vector<CCollider*>	m_vecCollidersCom[ENUM_CLASS(COLLIDERTYPE::END)] = { };

	CShader*			m_pShaderCom = { nullptr };
	CModel*				m_pModelCom	= { nullptr };

	_float				m_fElapsedTime = 0.f;





public:
	static CBullet* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBullet*		Clone(void* pArg)					override;
	void			Free()								override;


};

NS_END
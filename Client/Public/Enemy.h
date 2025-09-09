#pragma once

#include "Client_Defines.h"
#include "ContainerObject.h"


NS_BEGIN(Engine)

//class CShader;
//class CModel;
class CNavigation;
class CCollider;

NS_END

NS_BEGIN(Client)

class CEnemy final : public CContainerObject
{
public:
	typedef struct tagEnemyDesc : public GAMEOBJECT_DESC
	{

	}ENEMY_DESC;
private:
	CEnemy(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEnemy(const CEnemy& Prototype);
	virtual ~CEnemy() = default;

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
	HRESULT		Ready_PartObjects();

	void		Update_Transform(_float fTimeDelta);		// AI 행동에 의한 "Transform" 제어
	void		Update_AnimationState(_float fTimeDelta);	// AI 행동에 의한 "상태" 제어 (이를 기반으로 이벤트 등..)
	void		Update_AnimationIndex(_float fTimeDelta);	// AI 행동에 의한 "애니메이션" 제어

private:
	// 로컬 변수들 (타입, 컴포넌트 등..)

	CShader*			m_pShaderCom		= { nullptr };	
	CModel*				m_pModelCom			= { nullptr };
	CNavigation*		m_pNavigationCom	= { nullptr };

	vector<CCollider*>	m_vecCollidersCom[ENUM_CLASS(COLLIDERTYPE::END)] = { };

	CPartObject*		m_pPart_Weapon		= { nullptr };


private:
	_uint			m_iState			= { };


public:
	static CEnemy*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEnemy*			Clone(void* pArg)					override;
	void			Free()								override;


};

NS_END
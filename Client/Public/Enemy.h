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
		_uint iDefaultWeaponObjType		= ENUM_CLASS(GAMEOBJ_TYPE::END);	// 기본으로 착용중일 오브젝트의 타입을 정의
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
	HRESULT		Render_Shadow()						override;

	_bool		OnCollision(COLLISION_DESC* pColDescFrom, COLLISION_DESC* pColDescTo) override;

public:
	// 퍼블릭 함수들 (Get/Set 등..)
	// ..
	CPartObject* Get_WeaponPart() { return m_pPart_Weapon; };

private:
	// 로컬 함수들 (기능 분리)
	HRESULT		Ready_Components(void* pArg);
	HRESULT		Bind_ShaderResources();
	HRESULT		Ready_PartObject(_uint iObjType, void* pArg = nullptr);

	void		Update_Transform(_float fTimeDelta);		// AI 행동에 의한 "Transform" 제어
	void		Update_AnimationState(_float fTimeDelta);	// AI 행동에 의한 "상태" 제어 (이를 기반으로 이벤트 등..)
	void		Update_AnimationIndex(_float fTimeDelta);	// AI 행동에 의한 "애니메이션" 제어

	void		Update_LogicInterval(_float fTimeDelta);	// AI 행동의 갱신 주기 별도 관리
	void		Update_Interact(_float fTimeDelta);
	
	void		Update_BoneColliders();

private:
	void		Update_BoneCollider(CCollider* pCollider, const _char* szBoneName);
	void		Update_ToggleColliders();
	
	HRESULT		Ready_Colliders(void* pArg);

	void		Update_NearestWeapons();

private:
	// 로컬 변수들 (타입, 컴포넌트 등..)

	CShader*			m_pShaderCom		= { nullptr };	
	CModel*				m_pModelCom			= { nullptr };
	CNavigation*		m_pNavigationCom	= { nullptr };

	CPartObject*		m_pPart_Weapon		= { nullptr };

	ANIMARG_DESC		m_tAnimDesc[PART_END] = {};


	_float				m_fElapsedShot		= 1.f;



	_float				m_fLogic_ElapsedTime = 0.f;
	_float				m_fLogic_ResetIntervalTime = .5f;
	_trigger			m_isLogicTriggered = false;

private:
	_uint				m_iState			= { };

	_vector				m_vLoadShotDir		= {};

	_bool				m_isGroggy			= false;
	_float				m_fGroggy_ElapsedTime = {};
	const _float		m_fGroggyTime		= 0.7f;

	_float				m_fDeadDeltaTime	= 0.f;

private:
	// 근처 무기 탐색용
	CGameObject*		m_pNearestWeapon	= { nullptr };

public:
	static CEnemy*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEnemy*			Clone(void* pArg)					override;
	void			Free()								override;


};

NS_END
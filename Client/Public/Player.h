#pragma once

#include "Client_Defines.h"
#include "ContainerObject.h"

NS_BEGIN(Engine)

class CNavigation;
class CCollider;

NS_END


NS_BEGIN(Client)

class CPlayer final : public CContainerObject
{

public:
	typedef struct tagPlayerDesc : public GAMEOBJECT_DESC
	{

	}PLAYER_DESC;
public:
	typedef struct tAnimArgDesc	// 상하체 애니메이션 별도로 굴리기 위해 편의성으로 만든 구조체.
	{
		_uint iAnimIndex = {};
		_bool isAnimLoop = false;
		_float fTransitionTime = 0.2f;
	}ANIMARG_DESC;
private:
	CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayer(const CPlayer& Prototype);
	virtual ~CPlayer() = default;

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

	void		Update_Transform(_float fTimeDelta);		// 조작 등에 의한 "Transform" 제어
	void		Update_AnimationState(_float fTimeDelta);	// 조작 등에 의한 "상태" 제어 (이를 기반으로 이벤트 등..)
	void		Update_AnimationIndex(_float fTimeDelta);	// 조작 등에 의한 "애니메이션" 제어

private:
	// 로컬 변수들 (타입, 컴포넌트 등..)
	CNavigation*		m_pNavigationCom = { nullptr };

	CShader*			m_pShaderCom = { nullptr };
	CModel*				m_pModelCom = { nullptr };

	vector<CCollider*>	m_vecCollidersCom[ENUM_CLASS(COLLIDERTYPE::END)] = { };


private:
	_uint			m_iGameObjType = { };
	_int			m_iHp = {};

	_uint			m_iState = { };

	_float			m_fMouseSensor = { .1f };



public:
	static CPlayer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject*	Clone(void* pArg)				override;
	void			Free()							override;


};

NS_END
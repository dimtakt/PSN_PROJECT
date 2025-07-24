#pragma once

#include "Transform.h"

/* 게임오브젝트들의 부모가 되는 클래스. */

NS_BEGIN(Engine)

class ENGINE_DLL CGameObject abstract : public CBase
{
public:
	typedef struct tagGameObject : public CTransform::TRANSFORM_DESC
	{

	}GAMEOBJECT_DESC;
protected:
	CGameObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject(const CGameObject& Prototype);
	virtual ~CGameObject() = default;

public:
	class CComponent* Get_Component(const _wstring& strComponentTag);

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

	virtual _bool isPicked(CTransform* pTransform, _float3* pOut);
	virtual _bool isPicked(_float3* pOut);

protected:
	ID3D11Device*				m_pDevice = { nullptr };
	ID3D11DeviceContext*		m_pContext = { nullptr };
	class CGameInstance*		m_pGameInstance = { nullptr };
	class CTransform*			m_pTransformCom = { nullptr };

	map<const _wstring, class CComponent*>		m_Components;

	//CVIBuffer*					m_pVIBufferRef = { nullptr };

	_int			m_iHp			=	{};
	_int			m_iMaxHp		=	{};
	
	
protected:
	/*원형컴포넌트를 찾아서 복제한다. */
	/*map컨테이너에 보관한다.  */
	/*자식의 멤버변수에도 저장한다. */
	HRESULT Add_Component(_uint iPrototypeLevelIndex, const _wstring& strPrototypeTag, 
		const _wstring& strComponentTag, CComponent** ppOut, void* pArg = nullptr);

public:	
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;

};

NS_END
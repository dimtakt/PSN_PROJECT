#pragma once

#include "Transform.h"
#include "VIBuffer.h"
#include "Model.h"
#include "Mesh.h"


/* 게임오브젝트들의 부모가 되는 클래스. */

NS_BEGIN(Engine)

class CMesh;

class ENGINE_DLL CGameObject abstract : public CBase
{
public:
	typedef struct tagGameObject : public CTransform::TRANSFORM_DESC
	{
		_uint			iCustomLoadLevelIndex = UINT_MAX;

		_uint			iGameObjType;
	}GAMEOBJECT_DESC;

public:
	typedef struct tAnimArgDesc	// 상하체 애니메이션 별도로 굴리기 위해 편의성으로 만든 구조체.
	{
		_uint iAnimIndex = {};
		_bool isAnimLoop = false;
		_float fTransitionTime = 0.2f;
	}ANIMARG_DESC;


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
	virtual HRESULT Render_Shadow() { return S_OK; }

public:
	virtual void OnCollision(CGameObject* pCollisionHitBy);
	virtual void OnCollisionRay(CGameObject* pCollisionHitBy) {};

public:
	// 사용 전 반드시 Set_BufferRef 로 버퍼 할당 필요
	_bool isPicked(
		_float3* pOut = nullptr,
		_bool bReturnAll = false,
		std::vector<_float3>* vecOut = nullptr
	);

	_wstring Get_FileName() { return m_strModelName; }
	void Set_FileName(_wstring filename) { m_strModelName = filename; }
	_uint Get_ObjType() { return m_iGameObjType; }
	void Set_ObjType(_uint iObjType) { m_iGameObjType = iObjType; }
	_bool Get_isDead() { return m_isDead; }

	vector<class CCollider*>* Get_Colliders() { return m_vecCollidersCom; };


protected:
	// 이를 부모로 갖는 자식 오브젝트 생성시, 버퍼 정보가 있다면 반드시 할당해야 함.
	// 반드시 "버퍼 컴포넌트 생성 뒤 호출"할 것
	virtual void Set_BufferRef(CVIBuffer* pBuffer) { m_pVIBufferVecRef.push_back(pBuffer); }
	virtual void Set_BufferRef(CModel* pModel) {
		for (auto& mesh : pModel->Get_Meshes()) {
			if (auto pBuffer = dynamic_cast<CVIBuffer*>(mesh))
				m_pVIBufferVecRef.push_back(pBuffer);
		}
	}
	// 이거 이제 다른 게임오브젝트에서도 호출해주기
	
protected:
	ID3D11Device*				m_pDevice = { nullptr };
	ID3D11DeviceContext*		m_pContext = { nullptr };
	class CGameInstance*		m_pGameInstance = { nullptr };

	class CTransform*			m_pTransformCom = { nullptr };
	vector<CCollider*>			m_vecCollidersCom[ENUM_CLASS(COLLIDERTYPE::END)] = { };

	map<const _wstring, class CComponent*>		m_Components;


	// 자식 객체 생성시, 모델의 VIBuffer 주소를 여기에 저장 필요. isPicked 를 위함.
	vector<CVIBuffer*>		m_pVIBufferVecRef = {};

	_int			m_iHp			=	{};
	_int			m_iMaxHp		=	{};

	_wstring		m_strModelName	=	{};

	_uint			m_iGameObjType	= { };

	_bool			m_isDead		= false;
	
	
	
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
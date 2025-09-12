#include "Bullet.h"
#include "GameInstance.h"

#include "ContainerObject.h"
#include "Player.h"
#include "Enemy.h"


CBullet::CBullet(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CBullet::CBullet(const CBullet& Prototype)
	: CGameObject(Prototype)
{
}

HRESULT CBullet::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBullet::Initialize(void* pArg)
{
	Bullet_DESC* pDesc = static_cast<Bullet_DESC*>(pArg);
	m_iGameObjType = pDesc->iGameObjType;	// 컴포넌트 생성에 필요해서 앞으로 옮김

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(this->Ready_Components(pArg)))
		return E_FAIL;

	m_vMoveDir = pDesc->vMoveDir;
	m_pTransformCom->Set_WorldMatrix(pDesc->matSpawnTransform);	// 컴포넌트 생성 이후에 호출돼야 함
	


	_float fScaled = 2.f;
	m_pTransformCom->Set_Scale_Direct(XMVectorSet(fScaled, fScaled, fScaled, 1.f));

	return S_OK;
}

void CBullet::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);


}

void CBullet::Update(_float fTimeDelta)
{
	// 얘는 콜라이더 잘 붙여주고
	// 충돌 확인 진행 잘 되고
	// 앞으로 잘 날아가기만 하면 됨



	// 날아가게끔
	_float fMoveSpeed = 50.f;

	_vector vBulletPos = m_pTransformCom->Get_Position();
	vBulletPos += (fTimeDelta * m_vMoveDir * fMoveSpeed);

	m_pTransformCom->Set_Position_Direct(vBulletPos);



	// 콜라이더 위치 업데이트
	for (auto& vecColliders : m_vecCollidersCom)
		for (auto& collider : vecColliders)
			collider->Update(m_pTransformCom->Get_WorldMatrix());


	// 콜리젼 타겟과 충돌 여부 확인
	_uint iTargetType = UINT_MAX;
	
	if		(m_iGameObjType == ENUM_CLASS(GAMEOBJ_TYPE::PLAYERBULLET))
		iTargetType = ENUM_CLASS(GAMEOBJ_TYPE::ENEMY);
	else if (m_iGameObjType == ENUM_CLASS(GAMEOBJ_TYPE::ENEMYBULLET))
		iTargetType = ENUM_CLASS(GAMEOBJ_TYPE::PLAYER);
	else
		MSG_BOX(L"[CBullet::Update] Type of Bullet is undefined. Collision Checking Failed.");

	//Check_Collision(iTargetType);
	//Check_Destroy(fTimeDelta);
}

void CBullet::Late_Update(_float fTimeDelta)
{
	if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::NONBLEND, this)))
		return;

	__super::Late_Update(fTimeDelta);

#ifdef _DEBUG
	for (auto& vecColliders : m_vecCollidersCom)
		for (auto& collider : vecColliders)
		{
			if (collider != nullptr)
				if (FAILED(m_pGameInstance->Add_DebugComponent(collider)))
					return;
		}
#endif
}

HRESULT CBullet::Render()
{
	// 렌더
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;


	_uint           iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Materials(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE, 0)))
			return E_FAIL;

		m_pShaderCom->Begin(0);

		m_pModelCom->Render(i);
	}

	return S_OK;
}

HRESULT CBullet::Ready_Components(void* pArg)
{
	// 컴포넌트 준비
	_uint iDestLevel = m_pGameInstance->Get_DestLevel();

	if (FAILED(CGameObject::Add_Component(iDestLevel, TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_Weapon_Bullet"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom), nullptr)))
		return E_FAIL;


	CBounding_Sphere::BOUNDING_SPHERE_DESC  SphereDesc{};
	SphereDesc.fRadius = 0.01f;
	SphereDesc.vCenter = _float3(0.f, 0.f, 0.f);
	COLLISION_DESC colDesc = {};

	switch (m_iGameObjType)
	{
	case ENUM_CLASS(GAMEOBJ_TYPE::PLAYERBULLET):
		colDesc = {
			ENUM_CLASS(COLLISION_LAYER::BULLET_ATK),
			ENUM_CLASS(COLLISION_LAYER::ENEMY_HIT),
			true, this
		};
		break;

	case ENUM_CLASS(GAMEOBJ_TYPE::ENEMYBULLET):
		colDesc = {
			ENUM_CLASS(COLLISION_LAYER::BULLET_ATK),
			ENUM_CLASS(COLLISION_LAYER::PLAYER_HIT),
			true, this
		};
		break;

	default:
		std::cout << "[CBullet::Ready_Components] Type of Bullet is undefined. Collision Checking Failed." << std::endl;
		break;
	}
	
	SphereDesc.tColDesc = colDesc;

	CCollider* tmpCol = {};
	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&tmpCol), &SphereDesc)))
		return E_FAIL;
	m_vecCollidersCom[ENUM_CLASS(COLLIDERTYPE::SPHERE)].push_back(tmpCol);

	return S_OK;
}

HRESULT CBullet::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_Shader_Resource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
		return E_FAIL;

	return S_OK;
}

/*
_bool CBullet::Check_Collision(_uint iTargetType)
{
	_uint iDestLevel = m_pGameInstance->Get_DestLevel();
	_bool isIntersect = false;

	vector<CContainerObject*> vecTargets = {};
	_wstring strLayerTag = {};

	switch (iTargetType)		//
	{
	case ENUM_CLASS(GAMEOBJ_TYPE::PLAYER):
		strLayerTag = TEXT("Layer_Player");
		break;
	case ENUM_CLASS(GAMEOBJ_TYPE::ENEMY):
		strLayerTag = TEXT("Layer_Monster");
		break;
	}

	_uint iIndex = 0;
	while (true)
	{
		CContainerObject* pTarget = dynamic_cast<CContainerObject*>(m_pGameInstance->Find_GameObject(iDestLevel, strLayerTag, iIndex++));
		if (pTarget == nullptr) break;
		vecTargets.push_back(pTarget);
	}

	if (vecTargets.empty())
		return false;


	vector<vector<CCollider*>*> pVecColliders = {};
	for (auto& target : vecTargets) //
	{
		if (iTargetType == ENUM_CLASS(GAMEOBJ_TYPE::PLAYER))
		{
			CPlayer* pTarget = dynamic_cast<CPlayer*>(target);
			auto pColliders = pTarget->Get_Colliders(); 
			for (_uint i = 0; i < ENUM_CLASS(COLLIDERTYPE::END); ++i)
				pVecColliders.push_back(&pColliders[i]);
		}
		else if (iTargetType == ENUM_CLASS(GAMEOBJ_TYPE::ENEMY))
		{
			CEnemy* pTarget = dynamic_cast<CEnemy*>(target);
			auto pColliders = pTarget->Get_Colliders();
			for (_uint i = 0; i < ENUM_CLASS(COLLIDERTYPE::END); ++i)
				pVecColliders.push_back(&pColliders[i]);
		}
	}


	for (auto& vecTargetsColliders : pVecColliders)
	{
		for (auto& vecTargetCollider : *vecTargetsColliders)
		{
			CCollider* pTargetCollider = vecTargetCollider;	

			for (auto& vecColliders : m_vecCollidersCom)
				for (auto& collider : vecColliders)
					if (collider->Intersect(pTargetCollider))
						isIntersect = true;
		}
	}
	
	
	
	
	
	//for (auto& target : vecTargets) //
	//{
	//	if (iTargetType == ENUM_CLASS(GAMEOBJ_TYPE::PLAYER))
	//	{
	//		CPlayer* pTarget = dynamic_cast<CPlayer*>(target);
	//		vector<CCollider*>* pColliders = pTarget->Get_Colliders();
	//		for (_uint i = 0; i < ENUM_CLASS(COLLIDERTYPE::END); ++i)
	//		{
	//
	//
	//			for (_uint j = 0; j < pColliders->size(); j++)
	//			{
	//				CCollider* pTargetCollider = pColliders[i][j];
	//
	//				for (auto& vecColliders : m_vecCollidersCom)
	//					for (auto& collider : vecColliders)
	//						if (collider->Intersect(pTargetCollider))
	//							isIntersect = true;
	//			}
	//
	//
	//		}
	//	}
	//	else if (iTargetType == ENUM_CLASS(GAMEOBJ_TYPE::ENEMY))
	//	{
	//		CEnemy* pTarget = dynamic_cast<CEnemy*>(target);
	//		vector<CCollider*>* pColliders = pTarget->Get_Colliders();
	//		for (_uint i = 0; i < ENUM_CLASS(COLLIDERTYPE::END); ++i)
	//		{
	//
	//
	//			for (_uint j = 0; j < pColliders->size(); j++)
	//			{
	//				CCollider* pTargetCollider = pColliders[i][j];
	//
	//				for (auto& vecColliders : m_vecCollidersCom)
	//					for (auto& collider : vecColliders)
	//						if (collider->Intersect(pTargetCollider))
	//							isIntersect = true;
	//			}
	//
	//
	//		}
	//	}
	//}





	if (isIntersect)
		int i = 10;


	return isIntersect;
}
*/

void CBullet::Check_Destroy(_float fTimeDelta)
{
	std::cout << m_fElapsedTime << std::endl;

	m_fElapsedTime += fTimeDelta;

	const _float fDestroyTime = 5.f;

	if (m_fElapsedTime >= fDestroyTime)
	{
		m_fElapsedTime = 0;
		m_isDead = true;
	}
}

CBullet* CBullet::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBullet* pInstance = new CBullet(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Create : CBullet"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CBullet* CBullet::Clone(void* pArg)
{
	CBullet* pInstance = new CBullet(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CBullet"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBullet::Free()
{
	__super::Free();

	for (auto& vecColliders : m_vecCollidersCom)
		for (auto& collider : vecColliders)
			Safe_Release(collider);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
}

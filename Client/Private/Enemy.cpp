#include "Enemy.h"
#include "Gameinstance.h"

#include "Weapon_Karabin.h"
#include "Weapon_Pistol.h"
#include "Weapon_Shotgun.h"

CEnemy::CEnemy(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CContainerObject( pDevice, pContext )
{
}

CEnemy::CEnemy(const CEnemy& Prototype)
	: CContainerObject( Prototype )
{
}

HRESULT CEnemy::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CEnemy::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(this->Ready_Components(pArg)))
		return E_FAIL;

	m_iGameObjType = ENUM_CLASS(GAMEOBJ_TYPE::ENEMY);

	m_pModelCom->Add_Animation();
	m_pModelCom->Set_Animation(MOVE_U_IDLE, PART_UPPER, true);

	m_pModelCom->Add_Animation();
	m_pModelCom->Set_Animation(MOVE_L_IDLE, PART_LOWER, true);

	if (FAILED(Ready_PartObjects()))
		return E_FAIL;


	m_iMaxHp	= 3;
	m_iHp		= 3;	// ksta : 일정 시간 공격받지 않으면 다시 최대 체력으로 회복되어야 함.


	return S_OK;
}

void CEnemy::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);

}

void CEnemy::Update(_float fTimeDelta)
{
	// 행동 패턴 등.. 추후 컴포넌트 등을 이용하여 구현
	// 함수 꼭 분리해서 난잡하지 않게 만들기

	Update_Transform(fTimeDelta);
	Update_AnimationState(fTimeDelta);
	Update_AnimationIndex(fTimeDelta);

	m_pModelCom->Play_Animation_AllLayer(fTimeDelta);


	for (auto& vecColliders : m_vecCollidersCom)
		for (auto& collider : vecColliders)
			collider->Update(m_pTransformCom->Get_WorldMatrix());
}

void CEnemy::Late_Update(_float fTimeDelta)
{
	m_pTransformCom->Set_State(Engine::STATE::POSITION,
		m_pNavigationCom->Compute_OnCell(m_pTransformCom->Get_State(Engine::STATE::POSITION)));

	if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::NONBLEND, this)))
		return;

#ifdef _DEBUG
	for (auto& vecColliders : m_vecCollidersCom)
		for (auto& collider : vecColliders)
		{
			if (FAILED(m_pGameInstance->Add_DebugComponent(collider)))
				return;
		}

	if (FAILED(m_pGameInstance->Add_DebugComponent(m_pNavigationCom)))
		return;
#endif

	__super::Late_Update(fTimeDelta);
}

HRESULT CEnemy::Render()
{
	// 렌더
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;


	_uint           iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Materials(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE, 0)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i)))
			return E_FAIL;

		m_pShaderCom->Begin(0);

		m_pModelCom->Render(i);
	}

	return S_OK;
}

HRESULT CEnemy::Ready_Components(void* pArg)
{
	// 컴포넌트 준비
	_uint iDestLevelIndex = m_pGameInstance->Get_DestLevel();

	m_pTransformCom->Scale(_float3{ 5.f, 5.f, 5.f });

	if (FAILED(CGameObject::Add_Component(iDestLevelIndex, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_Enemy"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom), nullptr)))
		return E_FAIL;
	Set_BufferRef(m_pModelCom);

	CNavigation::NAVIGATION_DESC        NaviDesc{};
	switch (iDestLevelIndex)
	{
	case ENUM_CLASS(LEVEL::TEST_EXTRA1):	NaviDesc.iCurrentCellIndex = 25;	break;

	default:								NaviDesc.iCurrentCellIndex = 0;		break;
	}

	if (FAILED(CGameObject::Add_Component(iDestLevelIndex, TEXT("Prototype_Component_Navigation"),
		TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom), &NaviDesc)))
		return E_FAIL;


	CBounding_OBB::BOUNDING_OBB_DESC  OBBDesc{};
	OBBDesc.vAngles = _float3(0.f, 0.f, 0.f);
	OBBDesc.vExtents = _float3(0.1f, 0.82f, 0.1f);
	OBBDesc.vCenter = _float3(0.f, OBBDesc.vExtents.y, 0.f);


	CCollider* tmpColCom = nullptr;
	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&tmpColCom), &OBBDesc)))
		return E_FAIL;
	m_vecCollidersCom[ENUM_CLASS(COLLIDERTYPE::OBB)].push_back(tmpColCom);

	return S_OK;
}

HRESULT CEnemy::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_Shader_Resource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
		return E_FAIL;

	//const LIGHT_DESC* pLightDesc = m_pGameInstance->Get_LightDesc(0);
	//if (nullptr == pLightDesc)
	//	return E_FAIL;
	//if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightDir", &pLightDesc->vDirection, sizeof(_float4))))
	//	return E_FAIL;
	//if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightDiffuse", &pLightDesc->vDiffuse, sizeof(_float4))))
	//	return E_FAIL;
	//if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightAmbient", &pLightDesc->vAmbient, sizeof(_float4))))
	//	return E_FAIL;
	//if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightSpecular", &pLightDesc->vSpecular, sizeof(_float4))))
	//	return E_FAIL;
	//if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition(), sizeof(_float4))))
	//	return E_FAIL;

	return S_OK;
}

HRESULT CEnemy::Ready_PartObjects()
{
	_uint iDestLevel = m_pGameInstance->Get_DestLevel();

	CWeapon::WEAPON_DESC		WeaponDesc{};
	WeaponDesc.pState = &m_iState;
	WeaponDesc.pSocketMatrix = m_pModelCom->Get_BoneMatrix("root");
	//WeaponDesc.pSocketMatrix = m_pGameInstance->Get_Transform_Float4x4_Inverse(D3DTS::VIEW);
	WeaponDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
	WeaponDesc.pParentTarget = this;

	if (FAILED(__super::Add_PartObject(TEXT("Part_Weapon"), ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Weapon_Karabin"), &WeaponDesc)))
		return E_FAIL;
	
	return S_OK;
}

void CEnemy::Update_Transform(_float fTimeDelta)
{
	_float fTmpSpeed = 1.5f * fTimeDelta;


}

void CEnemy::Update_AnimationState(_float fTimeDelta)
{
	m_pModelCom->Set_Animation(MOVE_U_IDLE, PART_UPPER, true);
	m_pModelCom->Set_Animation(MOVE_L_IDLE, PART_LOWER, true);
}

void CEnemy::Update_AnimationIndex(_float fTimeDelta)
{

}

CEnemy* CEnemy::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CEnemy* pInstance = new CEnemy(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Create : CEnemy"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CEnemy* CEnemy::Clone(void* pArg)
{
	CEnemy* pInstance = new CEnemy(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CEnemy"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEnemy::Free()
{
	__super::Free();

	for (auto& vecColliders : m_vecCollidersCom)
		for (auto& collider : vecColliders)
			Safe_Release(collider);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pNavigationCom);
}

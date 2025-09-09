#include "Bullet.h"
#include "GameInstance.h"

CBullet::CBullet(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CBullet::CBullet(const CGameObject& Prototype)
	: CGameObject(Prototype)
{
}

HRESULT CBullet::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBullet::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(this->Ready_Components(pArg)))
		return E_FAIL;

	Bullet_DESC* pDesc = static_cast<Bullet_DESC*>(pArg);
	pDesc->vMoveDir = m_vMoveDir;
	pDesc->iGameObjType = m_iGameObjType;	// 플레이어 총알인지, 적 총알인지에 따라 판정 다르게?
	m_pTransformCom->Set_WorldMatrix(pDesc->matSpawnTransform);

	return S_OK;
}

void CBullet::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);


}

void CBullet::Update(_float fTimeDelta)
{
	// 얘는 콜라이더 잘 붙여주고
	// 충돌 콜러이더 매니저에서 잘 만들어주고
	// 앞으로 잘 날아가기만 하면 됨


}

void CBullet::Late_Update(_float fTimeDelta)
{
	if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::NONBLEND, this)))
		return;

	__super::Late_Update(fTimeDelta);

#ifdef _DEBUG
	if (m_pColliderCom != nullptr)
		if (FAILED(m_pGameInstance->Add_DebugComponent(m_pColliderCom)))
			return;
#endif
}

HRESULT CBullet::Render()
{
	if (FAILED(__super::Render()))
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


	CBounding_OBB::BOUNDING_OBB_DESC  OBBDesc{};
	OBBDesc.vAngles = _float3(0.f, 0.f, 0.f);
	OBBDesc.vExtents = _float3(0.1f, 0.82f, 0.1f);
	OBBDesc.vCenter = _float3(0.f, OBBDesc.vExtents.y, 0.f);

	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &OBBDesc)))
		return E_FAIL;

	return S_OK;
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

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
}

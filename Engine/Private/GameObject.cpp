#include "GameObject.h"

#include "GameInstance.h"

CGameObject::CGameObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice{ pDevice }
	, m_pContext { pContext }
	, m_pGameInstance { CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

CGameObject::CGameObject(const CGameObject& Prototype)
	: m_pDevice{ Prototype.m_pDevice }
	, m_pContext{ Prototype.m_pContext }
	, m_pGameInstance{ CGameInstance::GetInstance() }
	, m_isDead (false)
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

CComponent* CGameObject::Get_Component(const _wstring& strComponentTag)
{
	auto	iter = m_Components.find(strComponentTag);
	if (iter == m_Components.end())
		return nullptr;

	return iter->second;	
}

HRESULT CGameObject::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CGameObject::Initialize(void* pArg)
{
	m_pTransformCom = CTransform::Create(m_pDevice, m_pContext);
	if (nullptr == m_pTransformCom)
		return E_FAIL;

	if (FAILED(m_pTransformCom->Initialize(pArg)))
		return E_FAIL;	

	m_Components.emplace(TEXT("Com_Transform"), m_pTransformCom);		

	Safe_AddRef(m_pTransformCom);

	return S_OK;
}

void CGameObject::Priority_Update(_float fTimeDelta) 
{
}

void CGameObject::Update(_float fTimeDelta)
{
}

void CGameObject::Late_Update(_float fTimeDelta)
{
}

HRESULT CGameObject::Render()
{
	return S_OK;
}

void CGameObject::OnCollision(CGameObject* pCollisionHitBy)
{
	std::cout << "[CGameObject::OnCollision] Collision Detected!" << std::endl;
}

_bool CGameObject::isPicked(_float3* pOut, _bool bReturnAll, std::vector<_float3>* vecOut)
{
	_bool isPicked = false;
	_float3 vClosest = {};
	_float fClosestDistSq = FLT_MAX;

	if (!m_pVIBufferVecRef.empty()) {
		for (size_t i = 0; i < m_pVIBufferVecRef.size(); i++) {
			if (!bReturnAll) // 단일 점 모드
			{
				_float3 vTempOut = {};
				if (m_pVIBufferVecRef[i]->isPicked(m_pTransformCom, &vTempOut, nullptr))
				{
					const _float4* vCamPos = m_pGameInstance->Get_CamPosition();
					_vector vCam = XMLoadFloat4(vCamPos);
					_vector vHit = XMLoadFloat3(&vTempOut);

					float fDistSq = XMVectorGetX(XMVector3LengthSq(vHit - vCam));

					if (fDistSq < fClosestDistSq)
					{
						fClosestDistSq = fDistSq;
						vClosest = vTempOut;
						isPicked = true;
					}
				}
			}
			else if (vecOut) // 여러 점 모드
			{
				m_pVIBufferVecRef[i]->isPicked(m_pTransformCom, nullptr, vecOut);
				if (!vecOut->empty())
					isPicked = true;
			}
		}
	}

	if (!bReturnAll && pOut != nullptr && isPicked)
		*pOut = vClosest;

	return isPicked;
}

HRESULT CGameObject::Add_Component(_uint iPrototypeLevelIndex, const _wstring& strPrototypeTag, const _wstring& strComponentTag, CComponent** ppOut, void* pArg)
{
	if (nullptr != Get_Component(strComponentTag))
		return E_FAIL;

	CComponent*		pComponent = dynamic_cast<CComponent*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::COMPONENT, iPrototypeLevelIndex, strPrototypeTag, pArg));
	if (nullptr == pComponent)
		return E_FAIL;

	m_Components.emplace(strComponentTag, pComponent);

	*ppOut = pComponent;

	Safe_AddRef(pComponent);

	return S_OK;
}

void CGameObject::Free()
{
	__super::Free();

	for (auto& vecColliders : m_vecCollidersCom)
		for (auto& collider : vecColliders)
		{
			m_pGameInstance->Remove_Collider(collider);
			Safe_Release(collider);
		}

	for (auto& Pair : m_Components)
		Safe_Release(Pair.second);
	m_Components.clear();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pGameInstance);
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}

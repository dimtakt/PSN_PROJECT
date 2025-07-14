#include "Enemy.h"

CEnemy::CEnemy(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject( pDevice, pContext )
{
}

CEnemy::CEnemy(const CGameObject& Prototype)
	: CGameObject( Prototype )
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


}

void CEnemy::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
}

HRESULT CEnemy::Render()
{
	// 렌더

	return S_OK;
}

HRESULT CEnemy::Ready_Components(void* pArg)
{
	// 컴포넌트 준비

	return S_OK;
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
}

#include "Bullet.h"

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
	__super::Late_Update(fTimeDelta);
}

HRESULT CBullet::Render()
{
	// 렌더

	return S_OK;
}

HRESULT CBullet::Ready_Components(void* pArg)
{
	// 컴포넌트 준비

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
}

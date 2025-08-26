#include "CustomObj.h"

CCustomObj::CCustomObj(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CCustomObj::CCustomObj(const CGameObject& Prototype)
	: CGameObject(Prototype)
{
}

HRESULT CCustomObj::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCustomObj::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CCustomObj::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CCustomObj::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
}

void CCustomObj::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
}

HRESULT CCustomObj::Render()
{
	return S_OK;
}

void CCustomObj::Free()
{
	__super::Free();
}

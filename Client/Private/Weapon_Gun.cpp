#include "Weapon_Gun.h"
#include "GameInstance.h"

CWeapon_Gun::CWeapon_Gun(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CWeapon{ pDevice, pContext }
{

}

CWeapon_Gun::CWeapon_Gun(const CWeapon_Gun& Prototype)
    : CWeapon{ Prototype }
{

}

HRESULT CWeapon_Gun::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CWeapon_Gun::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    return S_OK;
}

void CWeapon_Gun::Priority_Update(_float fTimeDelta)
{
    __super::Priority_Update(fTimeDelta);
}

void CWeapon_Gun::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);
}

void CWeapon_Gun::Late_Update(_float fTimeDelta)
{
    __super::Late_Update(fTimeDelta);
}

HRESULT CWeapon_Gun::Render()
{
    if (FAILED(__super::Render()))
        return E_FAIL;

    return S_OK;
}

void CWeapon_Gun::Free()
{
    __super::Free();
}

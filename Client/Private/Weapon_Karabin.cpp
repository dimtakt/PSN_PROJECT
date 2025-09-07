#include "Weapon_Karabin.h"
#include "GameInstance.h"

CWeapon_Karabin::CWeapon_Karabin(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CWeapon_Gun{ pDevice, pContext }
{

}

CWeapon_Karabin::CWeapon_Karabin(const CWeapon_Karabin& Prototype)
    : CWeapon_Gun{ Prototype }
{

}

HRESULT CWeapon_Karabin::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CWeapon_Karabin::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    return S_OK;
}

void CWeapon_Karabin::Priority_Update(_float fTimeDelta)
{
    __super::Priority_Update(fTimeDelta);
}

void CWeapon_Karabin::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);
}

void CWeapon_Karabin::Late_Update(_float fTimeDelta)
{

}

HRESULT CWeapon_Karabin::Render()
{
    if (FAILED(__super::Render()))
        return E_FAIL;

    return S_OK;
}

CWeapon_Karabin* CWeapon_Karabin::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CWeapon_Karabin* pInstance = new CWeapon_Karabin(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CWeapon_Karabin"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CWeapon_Karabin::Clone(void* pArg)
{
    CWeapon_Karabin* pInstance = new CWeapon_Karabin(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CWeapon_Karabin"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CWeapon_Karabin::Free()
{
    __super::Free();
}

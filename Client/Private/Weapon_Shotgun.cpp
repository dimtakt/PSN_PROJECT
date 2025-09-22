#include "Weapon_Shotgun.h"
#include "GameInstance.h"
#include "UI_ScreenText.h"

CWeapon_Shotgun::CWeapon_Shotgun(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CWeapon_Gun{ pDevice, pContext }
{

}

CWeapon_Shotgun::CWeapon_Shotgun(const CWeapon_Shotgun& Prototype)
    : CWeapon_Gun{ Prototype }
{

}

HRESULT CWeapon_Shotgun::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CWeapon_Shotgun::Initialize(void* pArg)
{
    WEAPON_DESC* pDesc = static_cast<WEAPON_DESC*>(pArg);
    m_pParentState = pDesc->pState;
    m_pSocketMatrix = pDesc->pSocketMatrix;
    m_pParentTarget = pDesc->pParentTarget;

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    // 초기 트랜스폼 설정
    _matrix matId = XMMatrixIdentity();
    _matrix matScale = matId, matRot1 = matId, matRot2 = matId, matPos = matId;


    if (m_pParentTarget->Get_ObjType() == ENUM_CLASS(GAMEOBJ_TYPE::PLAYER))
    {
        matPos = XMMatrixTranslation(0.2f, -0.05f, 0.5f); // 순서대로 오른쪽, 위쪽, 앞쪽
    }
    else if (m_pParentTarget->Get_ObjType() == ENUM_CLASS(GAMEOBJ_TYPE::ENEMY))
    {
        matScale = XMMatrixScaling(1.f, 1.f, 1.f);
        matRot1 = XMMatrixRotationX(TO_RAD(100));
        matRot2 = XMMatrixRotationZ(TO_RAD(215));
        matPos = XMMatrixTranslation(-0.15f, +0.15f, +0.1f);  // 오-뒤 - 왼-앞 - 위
    }


    _matrix matTransform = matScale * matRot1 * matRot2 * matPos;
    m_pTransformCom->Set_WorldMatrix(matTransform);


    // 총, 탄 관련 초기 설정
    //m_iMaxBullets = (m_pParentTarget->Get_ObjType() == ENUM_CLASS(GAMEOBJ_TYPE::PLAYER))? 20 : 100;
    m_iMaxBullets = 200;
    m_iCurBullets = (pDesc->iCurLeftBullets == UINT_MAX) ?
        m_iMaxBullets : pDesc->iCurLeftBullets;
    m_fShotRandRange = 15.f;
    m_fZeroDst = 50.f;

    m_iGameObjType = ENUM_CLASS(GAMEOBJ_TYPE::WEAPON_RANGED_SHOTGUN);

    return S_OK;
}

void CWeapon_Shotgun::Priority_Update(_float fTimeDelta)
{
    // 뭐 안하는듯?
    __super::Priority_Update(fTimeDelta);
}

void CWeapon_Shotgun::Update(_float fTimeDelta)
{
    // weapon.cpp 에서 기본 업데이트 도는 중
    __super::Update(fTimeDelta);


    if (m_pParentTarget->Get_ObjType() == ENUM_CLASS(GAMEOBJ_TYPE::PLAYER))
    {
        const _float4x4* matCam = m_pGameInstance->Get_Transform_Float4x4_Inverse(D3DTS::VIEW);
        XMStoreFloat4x4(&m_CombinedWorldMatrix, m_pTransformCom->Get_WorldMatrix() * XMLoadFloat4x4(matCam));
    }
    else if (m_pParentTarget->Get_ObjType() == ENUM_CLASS(GAMEOBJ_TYPE::ENEMY))
    {
        _matrix     BoneMatrix = XMLoadFloat4x4(m_pSocketMatrix);

        for (size_t i = 0; i < 3; i++)
            BoneMatrix.r[i] = XMVector3Normalize(BoneMatrix.r[i]);

        XMStoreFloat4x4(&m_CombinedWorldMatrix,
            m_pTransformCom->Get_WorldMatrix() * BoneMatrix * XMLoadFloat4x4(m_pParentMatrix));
    }


    for (auto& vecColliders : m_vecCollidersCom)
        for (auto& collider : vecColliders)
            collider->Update(XMLoadFloat4x4(&m_CombinedWorldMatrix));


    //_float3 pos;
    //XMStoreFloat3(&pos, m_pTransformCom->Get_State(STATE::POSITION));
    //
    //_float3 rot;
    //rot = m_pTransformCom->Get_RotationEuler_Store();

    // combonedWorldMatrix 가 최종 반영된 Transform임에 유의

    m_CombinedWorldMatrix;
    m_CombinedWorldMatrix;
    m_pTransformCom;

    return;


    // 여기에서 업데이트 동작 진행.. (총알 발사 등)

}

void CWeapon_Shotgun::Late_Update(_float fTimeDelta)
{
    // weapon.cpp 에서 렌더그룹 및 디버그용 렌더 준비중 
    __super::Late_Update(fTimeDelta);

}

HRESULT CWeapon_Shotgun::Render()
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

void CWeapon_Shotgun::Shot(_vector* pDir, _uint iObjTypeIndex)
{
    if (m_iCurBullets == 0)
    {
        if (m_pParentTarget->Get_ObjType() == ENUM_CLASS(GAMEOBJ_TYPE::PLAYER))
        {
            m_pGameInstance->PlaySoundFixed(L"wpn_pistol_noammo.wav", ENUM_CLASS(SOUNDCH::SOUND_PLAYERWEAPON_EFF));
            // UI 출력 이벤트
            CUI_ScreenText* pUI_ScreenText = dynamic_cast<CUI_ScreenText*>(m_pGameInstance->Find_GameObject(m_pGameInstance->Get_DestLevel(), L"Layer_UI_ScreenText"));
            pUI_ScreenText->Show_ScreenText(ENUM_CLASS(SCREENTEXT_INDEX::NOAMMO));
        }
        return;
    }


    if (m_isOnCD)
        return;

    _uint iShotAmount = 10;
    for (_uint i = 0; i < iShotAmount; i++)
    {
        __super::Shot(pDir, iObjTypeIndex);
    }

    if (m_pParentTarget->Get_ObjType() == ENUM_CLASS(GAMEOBJ_TYPE::PLAYER))
        m_pGameInstance->PlaySoundFixed(L"shotgun_shot.wav", ENUM_CLASS(SOUNDCH::SOUND_PLAYERWEAPON_SHOT));
    else if (m_pParentTarget->Get_ObjType() == ENUM_CLASS(GAMEOBJ_TYPE::ENEMY))
        m_pGameInstance->PlaySoundFixed(L"shotgun_shot.wav", ENUM_CLASS(SOUNDCH::SOUND_ENEMYWEAPON_SHOT));

    m_isOnCD = true;

}

void CWeapon_Shotgun::Throw(_vector* pDir, _float fThrowPower, _vector vRot, _uint iObjTypeIndex)
{
    __super::Throw(pDir, fThrowPower, vRot, iObjTypeIndex);
}

HRESULT CWeapon_Shotgun::Ready_Components()
{
    _uint iDestLevel = m_pGameInstance->Get_DestLevel();

    if (FAILED(CGameObject::Add_Component(iDestLevel, TEXT("Prototype_Component_Shader_VtxMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_Weapon_Shotgun"),
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom), nullptr)))
        return E_FAIL;


    CBounding_OBB::BOUNDING_OBB_DESC  OBBDesc{};
    OBBDesc.vAngles = _float3(0.f, 0.f, 0.f);
    OBBDesc.vExtents = _float3(.05f, .15f, .7f);
    OBBDesc.vCenter = _float3(0.f, -(OBBDesc.vExtents.y * 0.2f), +(OBBDesc.vExtents.z * 0.4f));

    CCollider* tmpColCom = nullptr;
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Collider_OBB"),
        TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&tmpColCom), &OBBDesc)))
        return E_FAIL;
    m_vecCollidersCom[ENUM_CLASS(COLLIDERTYPE::OBB)].push_back(tmpColCom);

    return S_OK;

}


//HRESULT CWeapon_Shotgun::Bind_ShaderResources()
//{
//    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMatrix)))
//        return E_FAIL;
//
//    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
//        return E_FAIL;
//
//    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
//        return E_FAIL;
//
//    return S_OK;
//}

CWeapon_Shotgun* CWeapon_Shotgun::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CWeapon_Shotgun* pInstance = new CWeapon_Shotgun(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CWeapon_Shotgun"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CWeapon_Shotgun::Clone(void* pArg)
{
    CWeapon_Shotgun* pInstance = new CWeapon_Shotgun(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CWeapon_Shotgun"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CWeapon_Shotgun::Free()
{
    __super::Free();
}

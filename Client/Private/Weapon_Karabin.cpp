#include "Weapon_Karabin.h"
#include "GameInstance.h"
#include "Bullet.h"

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
    //m_iMaxBullets = (m_pParentTarget->Get_ObjType() == ENUM_CLASS(GAMEOBJ_TYPE::PLAYER)) ? 12 : 60;
    m_iMaxBullets = 200;
    m_iCurBullets = (pDesc->iCurLeftBullets == UINT_MAX) ? 
        m_iMaxBullets : pDesc->iCurLeftBullets;
    	
    m_fShotRandRange = 8.f;
    m_fZeroDst = 100.f;



    m_iGameObjType = ENUM_CLASS(GAMEOBJ_TYPE::WEAPON_RANGED_KARABIN);

    return S_OK;
}

void CWeapon_Karabin::Priority_Update(_float fTimeDelta)
{
    // 뭐 안하는듯?
    __super::Priority_Update(fTimeDelta);
}

void CWeapon_Karabin::Update(_float fTimeDelta)
{
    // weapon.cpp 에서 기본 업데이트 도는 중
    //__super::Update(fTimeDelta);


    // 수동 Update. m_pTransform 은 로컬 트랜스폼이 되어야 하는데..

    if      (m_pParentTarget->Get_ObjType() == ENUM_CLASS(GAMEOBJ_TYPE::PLAYER))
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


    // 여기에서 업데이트 동작 진행.. (총알 발사 등)
    if (m_isDoingShot)
        Shot_Continuously(m_pShotDir, m_iShotObjTypeIndex, fTimeDelta);
    else
    {
        m_pShotDir = nullptr;
        m_iShotObjTypeIndex = UINT_MAX;
    }

}

void CWeapon_Karabin::Late_Update(_float fTimeDelta)
{
    // weapon.cpp 에서 렌더그룹 및 디버그용 렌더 준비중 
    __super::Late_Update(fTimeDelta);   

}

HRESULT CWeapon_Karabin::Render()
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

void CWeapon_Karabin::Shot(_vector* pDir, _uint iObjTypeIndex)
{
    if (m_iCurBullets == 0)
    {
        // UI 출력 이벤트

        return;
    }


    if (m_isDoingShot == false)
    {
        m_isDoingShot = true;
        m_pShotDir = pDir;
        m_iShotObjTypeIndex = iObjTypeIndex;
        m_iShotIndex = 0;
        m_fShotElapsedTime = 0;
        // shot 시도 시 isDoingShot 로컬변수가 true 가 되며,
        // true 인 동안에는 정해진 발 수 만큼 공격 명령
        // 종료 후 false 전환. true 인 동안에는 새로 shot 시도 불가.
    }

    std::cout << "[CWeapon_Karabin::Shot] Current Left Bullets : " << m_iCurBullets << std::endl;
}

void CWeapon_Karabin::Shot_Continuously(_vector* pDir, _uint iObjTypeIndex, _float fTimeDelta)
{
    // Shot을 한번만 호출할거면,
    // 현재 날아갈 좌표 (vDIr) 를 어떻게 실시간으로 가져올거임?
    // 로컬에 포인터로 받아와서 저장해두고 그걸 사용하기?

    m_fShotElapsedTime += fTimeDelta;
    
    _float fShotInterval = 0.25f;

    const _uint iShotAmount = 4;          // Karabin 의 경우엔 소총 쏘듯이, 일정 간격으로 4발 나가도록 해야 함
 
    
    if ((m_iShotIndex + 1) * (fShotInterval) <= m_fShotElapsedTime)
    {
        if (iShotAmount == m_iShotIndex)
        {
            m_isDoingShot = false;
            return;
        }
        __super::Shot(pDir, iObjTypeIndex);
        m_iShotIndex++;
        m_iCurBullets--;

        if (m_pParentTarget->Get_ObjType() == ENUM_CLASS(GAMEOBJ_TYPE::PLAYER))
            m_pGameInstance->Req_EditTimeSpeed(1.f, true);
    }
}

void CWeapon_Karabin::Throw(_vector* pDir, _vector vRot, _uint iObjTypeIndex)
{
    __super::Throw(pDir, vRot, iObjTypeIndex);
}

HRESULT CWeapon_Karabin::Ready_Components()
{
    _uint iDestLevel = m_pGameInstance->Get_DestLevel();

    if (FAILED(CGameObject::Add_Component(iDestLevel, TEXT("Prototype_Component_Shader_VtxMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_Weapon_Karabin"),
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom), nullptr)))
        return E_FAIL;


    CBounding_OBB::BOUNDING_OBB_DESC  OBBDesc{};
    OBBDesc.vAngles = _float3(0.f, 0.f, 0.f);
    OBBDesc.vExtents = _float3(.05f, .15f, .5f);
    OBBDesc.vCenter = _float3(0.f, -(OBBDesc.vExtents.y * 0.65f), 0.f);

    CCollider* tmpColCom = nullptr;
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Collider_OBB"),
        TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&tmpColCom), &OBBDesc)))
        return E_FAIL;
    m_vecCollidersCom[ENUM_CLASS(COLLIDERTYPE::OBB)].push_back(tmpColCom);

    return S_OK;

}


//HRESULT CWeapon_Karabin::Bind_ShaderResources()
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
        MSG_BOX(TEXT("Failed to Cloned : CWeapon_Karabin"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CWeapon_Karabin::Free()
{
    __super::Free();
}

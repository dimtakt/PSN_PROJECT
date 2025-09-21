#include "CustomObj_Pickupable.h"
#include "Gameinstance.h"

#include "Player.h"
#include "Enemy.h"
#include "PartObject.h"
#include "Weapon.h"

CCustomObj_Pickupable::CCustomObj_Pickupable(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CCustomObj_NonAnim(pDevice, pContext)
{
}

CCustomObj_Pickupable::CCustomObj_Pickupable(const CGameObject& Prototype)
    : CCustomObj_NonAnim(Prototype)
{
}

HRESULT CCustomObj_Pickupable::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CCustomObj_Pickupable::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(this->Ready_Components(pArg)))
        return E_FAIL;

    if (pArg != nullptr)
    {
        THROWN_PICKUPOBJ_DESC* pDesc = static_cast<THROWN_PICKUPOBJ_DESC*>(pArg);
        m_pThrowDir = pDesc->vThrowDir;
        m_pThrowRot = pDesc->vThrowRot;

        m_tGunInfoDesc = pDesc->tGunInfoDesc;
    }

    return S_OK;
}

void CCustomObj_Pickupable::Priority_Update(_float fTimeDelta)
{
    __super::Priority_Update(fTimeDelta);
}

void CCustomObj_Pickupable::Update(_float fTimeDelta)
{
    if (m_isPickingUp)
        Update_PickingUp(fTimeDelta);




    // 벡터분리 해서 현재 transform에 변화주도록

    m_pTransformCom->Set_Position_Direct(m_pTransformCom->Get_Position() + m_pThrowDir * fTimeDelta);
    

    _vector vEulerDelta = m_pThrowRot * fTimeDelta;  // 이번 프레임에 회전할 양 (degree)
    _vector qDelta = ROT_TO_QUAT(   TO_RAD(XMVectorGetX(vEulerDelta)),
                                    TO_RAD(XMVectorGetY(vEulerDelta)),
                                    TO_RAD(XMVectorGetZ(vEulerDelta)));

    _vector qCur = m_pTransformCom->Get_RotationQuat();
    _vector qCalced = XMQuaternionMultiply(qCur, qDelta);
    qCalced = XMQuaternionNormalize(qCalced);

    m_pTransformCom->Set_Rotation_DirectQuat(qCalced);

    m_vecCollidersCom[ENUM_CLASS(COLLIDERTYPE::OBB)][0]->Update(m_pTransformCom->Get_WorldMatrix());	// Direct Update
}

void CCustomObj_Pickupable::Late_Update(_float fTimeDelta)
{
    if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::NONBLEND, this)))
        return;

#ifdef _DEBUG
    for (auto& vecColliders : m_vecCollidersCom)
        for (auto& collider : vecColliders)
        {
            if (FAILED(m_pGameInstance->Add_DebugComponent(collider)))
                return;
        }
#endif




    if (m_isDeadStandby)
    {
        for (auto& vecColliders : m_vecCollidersCom)
            for (auto& collider : vecColliders)
                collider->Set_isActive(false);
    }
}

HRESULT CCustomObj_Pickupable::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    _uint           iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (_uint i = 0; i < iNumMeshes; i++)
    {
        if (FAILED(m_pModelCom->Bind_Materials(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE, 0)))
            return E_FAIL;

        m_pShaderCom->Begin(0);

        m_pModelCom->Render(i);


    }

    return S_OK;
}

_bool CCustomObj_Pickupable::OnCollision(COLLISION_DESC* pColDescFrom, COLLISION_DESC* pColDescTo)
{
    // 1. 이펙트 추가
    // 2. 사망 타이머 작동, 작동 중 사라짐
    // 3. 사망 타이머 채워질 시 사망 및 삭제
    Add_HitEffect();
    m_isDeadStandby = true;
    
    m_isDead = true; // 임시

    return true;
}

void CCustomObj_Pickupable::OnCollisionRay(CGameObject* pCollisionHitBy)
{
    // 플레이어가 클릭 시 플레이어쪽으로 다가오며 사라지도록 할 것
    // 이게 무슨 오브젝트인지의 정보를 플레이어 단계에서 알아야 함

    m_pCollByTarget = pCollisionHitBy;
    _uint iCollisionObjType = m_pCollByTarget->Get_ObjType();

    if (iCollisionObjType == ENUM_CLASS(GAMEOBJ_TYPE::PLAYER))
    {
        //CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pCollByTarget);
        //CPartObject* pWeaponObj = pPlayer->Get_WeaponPart();

        //_float4x4 matWeapon = pWeaponObj->Get_CombinedMatrix();

        m_isPickingUp = true;
        m_pCollByTarget = pCollisionHitBy;

        //std::cout << "[CustomObj_Pickupable::OnCollisionRay] CollisionRay Event Called. Hit by Player!" << std::endl;
    }
    else if (iCollisionObjType == ENUM_CLASS(GAMEOBJ_TYPE::ENEMY))
    {
        m_isPickingUp = true;
        m_pCollByTarget = pCollisionHitBy;

        std::cout << "[CustomObj_Pickupable::OnCollisionRay] CollisionRay Event Called. by Enemy!" << std::endl;
    }
    //else if (iCollisionObjType == ENUM_CLASS(GAMEOBJ_TYPE::ENEMY))
    //{
    //    // ... 
    //}
}

HRESULT CCustomObj_Pickupable::Ready_Components(void* pArg)
{
#pragma region old

    /*
   
    // 부모 오브젝트에서 이미 이루어짐

    //CUSTOMOBJ_DESC* pDesc = reinterpret_cast<CUSTOMOBJ_DESC*>(pArg);
    //
    //_wstring strPrototypeName = pDesc->strModelComPrototypeTag;
    //m_iGameObjType = pDesc->iGameObjType;
    //
    //_uint iDestLevelIndex = m_pGameInstance->Get_DestLevel();
    //
    //if (FAILED(CGameObject::Add_Component(iDestLevelIndex, TEXT("Prototype_Component_Shader_VtxMesh"),
    //    TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
    //    return E_FAIL;
    //
    //
    //if (FAILED(CGameObject::Add_Component(iDestLevelIndex, strPrototypeName,
    //    TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom), nullptr)))
    //    return E_FAIL;
    //Set_BufferRef(m_pModelCom);

    */

#pragma endregion

    // 콜라이더 추가..
    // 이게 일반 NonAnim과 Pickupable의 다른점 중 하나. 이를 통해 ray 인식이 가능하도록 함

    CBounding_OBB::BOUNDING_OBB_DESC  OBBDesc{};
    COLLISION_DESC colDesc = {};

    switch (m_iGameObjType)
    {
    case ENUM_CLASS(GAMEOBJ_TYPE::WEAPON_RANGED_KARABIN):
        OBBDesc.vAngles = _float3(0.f, 0.f, 0.f);
        OBBDesc.vExtents = _float3(.05f, .15f, .5f);
        OBBDesc.vCenter = _float3(0.f, -(OBBDesc.vExtents.y * 0.65f), 0.f);
        break;
    case ENUM_CLASS(GAMEOBJ_TYPE::WEAPON_RANGED_PISTOL):
        OBBDesc.vAngles = _float3(0.f, 0.f, 0.f);
        OBBDesc.vExtents = _float3(.03f, .10f, .15f);
        OBBDesc.vCenter = _float3(0.f, 0.f, +(OBBDesc.vExtents.z * 0.4f));
        break;
    case ENUM_CLASS(GAMEOBJ_TYPE::WEAPON_RANGED_SHOTGUN):
        OBBDesc.vAngles = _float3(0.f, 0.f, 0.f);
        OBBDesc.vExtents = _float3(.05f, .15f, .7f);
        OBBDesc.vCenter = _float3(0.f, -(OBBDesc.vExtents.y * 0.2f), +(OBBDesc.vExtents.z * 0.4f));
        break;
    default:
        break;
    }

    colDesc = {
        ENUM_CLASS(COLLISION_LAYER::PICKUPABLE),
        0,
        true, this
    };
    OBBDesc.tColDesc = colDesc;



    CCollider* tmpColCom = nullptr;
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Collider_OBB"),
        TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&tmpColCom), &OBBDesc)))
        return E_FAIL;
    m_vecCollidersCom[ENUM_CLASS(COLLIDERTYPE::OBB)].push_back(tmpColCom);

    return S_OK;
}

HRESULT CCustomObj_Pickupable::Bind_ShaderResources()
{
    if (FAILED(m_pTransformCom->Bind_Shader_Resource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
        return E_FAIL;



    return S_OK;
}

void CCustomObj_Pickupable::Update_PickingUp(_float fTimeDelta)
{
    _float fRawTimeDelta = fTimeDelta / m_pGameInstance->Get_TimeSpeed();
    _uint iDestLevel = m_pGameInstance->Get_DestLevel();

    switch (m_pCollByTarget->Get_ObjType())
    {
    case ENUM_CLASS(GAMEOBJ_TYPE::PLAYER):
    {
        // 플레이어 시선점으로, 정해진 시간만큼, 시간 경과에 따라,
        // 현재 이동까지 남은 거리와 그 다음에 이동할 거리를 계산하여 트랜스폼 매트릭스에 반영
        
        // RawTimeDelta 기반, 이동 상태 반영
        m_fPickingElapsedTime += fRawTimeDelta;
        _float fLeftTime = max(0.0f, m_fPickingMaxTime - m_fPickingElapsedTime);
        _float fRatio = min(1.0f, fRawTimeDelta / fLeftTime);
        //std::cout << "[CustomObj_Pickupable::Update_PickingUp] fRatio : " << fRatio << std::endl;

        _float3 vPos = {}, vRot = {}, vSca = {};
        _float3 vDestPos = {}, vDestRot = {}, vDestSca = {};
        _float3 vCalcPos = {}, vCalcRot = {}, vCalcSca = {};

        CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pCollByTarget);
        if (nullptr == pPlayer->Get_WeaponPart())
            return;


        //CTransform* pTargetTransform = dynamic_cast<CTransform*>(pPlayer->Get_WeaponPart()->Get_Component(L"Com_Transform"));
        _float4x4 matWeaponMatrix = pPlayer->Get_WeaponPart()->Get_CombinedMatrix();

        _vector vLoadDestPos = {}, vLoadDestRot = {}, vLoadDestSca = {};
        XMMatrixDecompose(&vLoadDestSca, &vLoadDestRot, &vLoadDestPos, XMLoadFloat4x4(&matWeaponMatrix));

        XMStoreFloat3(&vDestPos, vLoadDestPos);
        _float4x4 matTmpRot; XMStoreFloat4x4(&matTmpRot, QUAT_TO_MAT(vLoadDestRot));
        vDestRot = MAT_TO_ROT(matTmpRot);
        XMStoreFloat3(&vDestSca, vLoadDestSca);

        vPos = m_pTransformCom->Get_Position_Store();
        vRot = m_pTransformCom->Get_RotationEuler_Store();
        vSca = m_pTransformCom->Get_Scale_Store();

        XMStoreFloat3(&vCalcPos, XMLoadFloat3(&vPos) * (1 - fRatio) + XMLoadFloat3(&vDestPos) * (fRatio));
        XMStoreFloat3(&vCalcRot, XMLoadFloat3(&vRot) * (1 - fRatio) + XMLoadFloat3(&vDestRot) * (fRatio));
        XMStoreFloat3(&vCalcSca, XMLoadFloat3(&vSca) * (1 - fRatio) + XMLoadFloat3(&vDestSca) * (fRatio));

        m_pTransformCom->Set_Scale_Direct(vCalcSca);
        m_pTransformCom->Set_Rotation_DirectEuler(vCalcRot);
        m_pTransformCom->Set_Position_Direct(vCalcPos);

        // 파괴
        if (m_fPickingElapsedTime > m_fPickingMaxTime)  // 일정 시간이 다 지났으면
        {
            dynamic_cast<CWeapon*>(pPlayer->Get_WeaponPart())->Set_toAttached(true);
            m_isDead = true;
            //m_pGameInstance->Remove_GameObject_FromLayer(iDestLevel, L"Layer_Loaded_Object_Pickupable", this);
        }
    } break;
    case ENUM_CLASS(GAMEOBJ_TYPE::ENEMY):
    {
        // Enemy는 TimeDelta 기반, 이동 상태 반영
        m_fPickingElapsedTime += fTimeDelta;
        _float fLeftTime = max(0.0f, m_fPickingMaxTime - m_fPickingElapsedTime);
        _float fRatio = min(1.0f, fTimeDelta / fLeftTime);
        //std::cout << "[CustomObj_Pickupable::Update_PickingUp] fRatio : " << fRatio << std::endl;

        _float3 vPos = {}, vRot = {}, vSca = {};
        _float3 vDestPos = {}, vDestRot = {}, vDestSca = {};
        _float3 vCalcPos = {}, vCalcRot = {}, vCalcSca = {};

        CEnemy* pEnemy = dynamic_cast<CEnemy*>(m_pCollByTarget);
        if (nullptr == pEnemy->Get_WeaponPart())
            return;


        //CTransform* pTargetTransform = dynamic_cast<CTransform*>(pPlayer->Get_WeaponPart()->Get_Component(L"Com_Transform"));
        _float4x4 matWeaponMatrix = pEnemy->Get_WeaponPart()->Get_CombinedMatrix();

        _vector vLoadDestPos = {}, vLoadDestRot = {}, vLoadDestSca = {};
        XMMatrixDecompose(&vLoadDestSca, &vLoadDestRot, &vLoadDestPos, XMLoadFloat4x4(&matWeaponMatrix));

        XMStoreFloat3(&vDestPos, vLoadDestPos);
        _float4x4 matTmpRot; XMStoreFloat4x4(&matTmpRot, QUAT_TO_MAT(vLoadDestRot));
        vDestRot = MAT_TO_ROT(matTmpRot);
        XMStoreFloat3(&vDestSca, vLoadDestSca);

        vPos = m_pTransformCom->Get_Position_Store();
        vRot = m_pTransformCom->Get_RotationEuler_Store();
        vSca = m_pTransformCom->Get_Scale_Store();

        XMStoreFloat3(&vCalcPos, XMLoadFloat3(&vPos) * (1 - fRatio) + XMLoadFloat3(&vDestPos) * (fRatio));
        XMStoreFloat3(&vCalcRot, XMLoadFloat3(&vRot) * (1 - fRatio) + XMLoadFloat3(&vDestRot) * (fRatio));
        XMStoreFloat3(&vCalcSca, XMLoadFloat3(&vSca) * (1 - fRatio) + XMLoadFloat3(&vDestSca) * (fRatio));

        m_pTransformCom->Set_Scale_Direct(vCalcSca);
        m_pTransformCom->Set_Rotation_DirectEuler(vCalcRot);
        m_pTransformCom->Set_Position_Direct(vCalcPos);

        // 파괴
        if (m_fPickingElapsedTime > m_fPickingMaxTime)  // 일정 시간이 다 지났으면
        {
            dynamic_cast<CWeapon*>(pEnemy->Get_WeaponPart())->Set_toAttached(true);
            m_isDead = true;
            //m_pGameInstance->Remove_GameObject_FromLayer(iDestLevel, L"Layer_Loaded_Object_Pickupable", this);
        }
    } break;
    default:
    {

    } break;
        
    }
}

void CCustomObj_Pickupable::Add_HitEffect()
{
    _matrix matBulletTransform = m_pTransformCom->Get_WorldMatrix();
    _uint iDestLevel = m_pGameInstance->Get_DestLevel();
    const _wstring strEffectTag = L"Layer_Particle_HitEffect";

    if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(iDestLevel, strEffectTag,
        iDestLevel, TEXT("Prototype_GameObject_Particle_HitEffect"))))
        MSG_BOX(L"이펙트 생성 실패");

    CGameObject* pEffectObj = m_pGameInstance->Get_LastGameObject(iDestLevel, strEffectTag);
    CTransform* pEffectTransform = dynamic_cast<CTransform*>(pEffectObj->Get_Component(L"Com_Transform"));
    pEffectTransform->Set_WorldMatrix(m_pTransformCom->Get_WorldMatrix());
}

CCustomObj_Pickupable* CCustomObj_Pickupable::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CCustomObj_Pickupable* pInstance = new CCustomObj_Pickupable(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Create : CCustomObj_Pickupable"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CCustomObj_Pickupable* CCustomObj_Pickupable::Clone(void* pArg)
{
    CCustomObj_Pickupable* pInstance = new CCustomObj_Pickupable(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Clone : CCustomObj_Pickupable"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CCustomObj_Pickupable::Free()
{

    __super::Free();
}

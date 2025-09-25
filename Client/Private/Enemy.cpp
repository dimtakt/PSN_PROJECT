#include "Enemy.h"
#include "Gameinstance.h"

#include "Weapon_Karabin.h"
#include "Weapon_Pistol.h"
#include "Weapon_Shotgun.h"
#include "CustomObj_Pickupable.h"

#include "Camera_Player.h"



CEnemy::CEnemy(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CContainerObject( pDevice, pContext )
{
}

CEnemy::CEnemy(const CEnemy& Prototype)
	: CContainerObject( Prototype )
{
}

HRESULT CEnemy::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CEnemy::Initialize(void* pArg)
{
	ENEMY_DESC* pDesc = static_cast<ENEMY_DESC*>(pArg);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(this->Ready_Components(pArg)))
		return E_FAIL;

	m_iGameObjType = ENUM_CLASS(GAMEOBJ_TYPE::ENEMY);

	m_pModelCom->Add_Animation();
	m_pModelCom->Set_Animation(MOVE_U_IDLE, PART_UPPER, true);

	m_pModelCom->Add_Animation();
	m_pModelCom->Set_Animation(MOVE_L_IDLE, PART_LOWER, true);


	m_iMaxHp	= 3;
	m_iHp		= 3;	// ksta : 일정 시간 공격받지 않으면 다시 최대 체력으로 회복되어야 함.
	


	switch (pDesc->iDefaultWeaponObjType)
	{
	case ENUM_CLASS(GAMEOBJ_TYPE::WEAPON_RANGED_KARABIN):
	case ENUM_CLASS(GAMEOBJ_TYPE::WEAPON_RANGED_PISTOL):
	case ENUM_CLASS(GAMEOBJ_TYPE::WEAPON_RANGED_SHOTGUN):
	{
		Ready_PartObject(pDesc->iDefaultWeaponObjType);
		static_cast<CWeapon*>(m_pPart_Weapon)->Set_toAttached(true);
	} break;
	default:			break;	// 무기 기본값이 없거나 잘못된 값이 들어간 경우
	}

	for (auto& pos : pDesc->vecPremovePoses)
		m_listPreMovePoses.push_back(pos);


	m_fLogic_ElapsedTime = m_pGameInstance->Rand(0.f, m_fLogic_ResetIntervalTime);

	return S_OK;
}

void CEnemy::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);

	Update_HitCD(fTimeDelta);
}

void CEnemy::Update(_float fTimeDelta)
{
	// 행동 패턴 등.. 추후 컴포넌트 등을 이용하여 구현
	// 함수 꼭 분리해서 난잡하지 않게 만들기

	Update_NearestWeapons();
	Update_Transform(fTimeDelta);

	Update_AnimationState(fTimeDelta);
	Update_AnimationIndex(fTimeDelta);
	Update_Interact(fTimeDelta);

	m_pModelCom->Play_Animation_AllLayer(fTimeDelta);

	Update_BoneColliders();
	Update_LogicInterval(fTimeDelta);


	//for (auto& vecColliders : m_vecCollidersCom)
	//	for (auto& collider : vecColliders)
	//	{
	//		collider->Update(m_pTransformCom->Get_WorldMatrix());
	//	}

	// 테스트용
	//static _float fMoveTimeDelta = 0.f;
	//static _bool isGoStraight = true;

	//fMoveTimeDelta += fTimeDelta;
	//if (fMoveTimeDelta >= 3.f)
	//{
	//	fMoveTimeDelta = 0;
	//	isGoStraight = !isGoStraight;
	//}

	//if (isGoStraight)
	//	m_pTransformCom->Go_Straight(fTimeDelta, m_pNavigationCom);
	//else	
	//	m_pTransformCom->Go_Backward(fTimeDelta, m_pNavigationCom);

	// 사망 처리
	const _float fDeadTime = 2.f;
	_float fRawTimeDelta = fTimeDelta / m_pGameInstance->Get_TimeSpeed();

	if (m_isDeadStandby)
	{
		m_fDeadDeltaTime += fTimeDelta;
		

		if (m_fDeadDeltaTime >= fDeadTime)
		{
			m_isDead = true;
		}
	}

	__super::Update(fTimeDelta);
}

void CEnemy::Late_Update(_float fTimeDelta)
{

	m_pTransformCom->Set_State(Engine::STATE::POSITION,
		m_pNavigationCom->Compute_OnCell(m_pTransformCom->Get_State(Engine::STATE::POSITION)));

	//RENDERGROUP tNormalGroup = (m_isDeadStandby) ? RENDERGROUP::BLEND : RENDERGROUP::NONBLEND;
	//if (FAILED(m_pGameInstance->Add_RenderGroup(tNormalGroup, this)))
	//	return;
	if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::NONBLEND, this)))
		return;
	if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::SHADOW, this)))
		return;


	if (m_isDeadStandby)
		return;

#ifdef _DEBUG
	for (auto& vecColliders : m_vecCollidersCom)
		for (auto& collider : vecColliders)
		{
			if (FAILED(m_pGameInstance->Add_DebugComponent(collider)))
				return;
		}

	if (FAILED(m_pGameInstance->Add_DebugComponent(m_pNavigationCom)))
		return;
#endif

	__super::Late_Update(fTimeDelta);
}

HRESULT CEnemy::Render()
{
	// 렌더
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;


	_uint           iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Materials(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE, 0)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i)))
			return E_FAIL;

		_uint iShaderIndex = (m_isDeadStandby)? 3 : 0;

		m_pShaderCom->Begin(3);

		m_pModelCom->Render(i);;
	}

	return S_OK;
}


HRESULT CEnemy::Render_Shadow()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom->Get_WorldMatrixPtr())))
		return E_FAIL;

	/* 그림자를 표현하고하는 특수한 광원을 정의하고 그 광원이 바라본 장면응로서 플레이어를 그려준다. */
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_ShadowLight_Transform_Float4x4(D3DTS::VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_ShadowLight_Transform_Float4x4(D3DTS::PROJ))))
		return E_FAIL;

	_uint           iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i)))
			return E_FAIL;

		m_pShaderCom->Begin(2);

		m_pModelCom->Render(i);
	}


	return S_OK;
}

_bool CEnemy::OnCollision(COLLISION_DESC* pColDescFrom, COLLISION_DESC* pColDescTo)
{
	if (__super::OnCollision(pColDescFrom, pColDescTo) == false)
		return false;

	// 콜리젼을 받는 경우를 정의..
	// 이는 피격 당 1회 (정확히는 최소 0.2초 간격) 로만 발동됨.

	// 맞은 desc와 공격한 desc 정보 둘 다를 받아와야 할 듯


	// 총에는 한 방에 죽음
	if (pColDescFrom->pOwner->Get_ObjType() == ENUM_CLASS(GAMEOBJ_TYPE::PLAYERBULLET))
	{
		m_iHp = 0;
		m_isDeadStandby = true;
	}



	CCollider* pAtkCom = pColDescFrom->pColCom;
	CGameObject* pAtkObj = pColDescFrom->pOwner;
	// 플레이어 손 콜라이더면 플레이어에게 카메라 흔들림 처리
	if (pAtkCom == pAtkObj->Get_Component(L"Com_Collider_LeftHand") ||
		pAtkCom == pAtkObj->Get_Component(L"Com_Collider_RightHand"))
	{
		dynamic_cast<CCamera_Player*>(m_pGameInstance->Find_GameObject(m_pGameInstance->Get_DestLevel(), L"Layer_Camera"))->Camera_Shake(10.f);
	}



	CCollider* pHitCom = pColDescTo->pColCom;	// 공격받은 콜라이더

	if (pHitCom == CGameObject::Get_Component(L"Com_Collider_BodyAll") ||
		pHitCom == CGameObject::Get_Component(L"Com_Collider_LeftHand") || 
		pHitCom == CGameObject::Get_Component(L"Com_Collider_RightHand"))
	{
		// 몸통에 맞음
#ifdef _DEBUG
		std::cout << "[CEnemy::OnCollision] Body Collision Detected." << std::endl;
#endif
		if (m_iState & ENUM_CLASS(ENEMY_STATE::DMGD_L) ||
			m_iState & ENUM_CLASS(ENEMY_STATE::DMGD_U))		// 이미 데미지 받은 상태라면..
		{
			m_iState = ENUM_CLASS(ENEMY_STATE::DMGD_L);
			m_fGroggy_ElapsedTime = 0.f;
		}
		else
		{
			m_iState = ENUM_CLASS(ENEMY_STATE::DMGD_L);
			m_isGroggy = true;
		}
	}
	else if (pHitCom == CGameObject::Get_Component(L"Com_Collider_Head"))
	{
		// 머리에 맞음
#ifdef _DEBUG
		std::cout << "[CEnemy::OnCollision] Head Collision Detected." << std::endl;
#endif
		if (m_iState & ENUM_CLASS(ENEMY_STATE::DMGD_L) ||
			m_iState & ENUM_CLASS(ENEMY_STATE::DMGD_U))		// 이미 데미지 받은 상태라면..
		{
			m_iState = ENUM_CLASS(ENEMY_STATE::DMGD_U);
			m_fGroggy_ElapsedTime = 0.f;
		}
		else
		{
			m_iState = ENUM_CLASS(ENEMY_STATE::DMGD_U);
			m_isGroggy = true;
		}
	}

	CWeapon_Gun* pWeaponGun = dynamic_cast<CWeapon_Gun*>(m_pPart_Weapon);
	if (pWeaponGun)
	{
		_float fThrowPower = 2.5f;
		pWeaponGun->Drop(&m_vLoadShotDir, fThrowPower, XMVectorSet(0.f, 0.f, 0.f, 0.f), pWeaponGun->Get_ObjType());

		// 현재 사용중인 무기 삭제
		Remove_PartObject(L"Part_Weapon_Enemy");
		m_pPart_Weapon = nullptr;
	}

	Update_NearestWeapons();

	_uint iSoundRand = static_cast<_uint>(m_pGameInstance->Rand(0.f, 2.99f));
	switch (iSoundRand)
	{
	case 0: m_pGameInstance->PlaySoundFixed(L"crystal_crush.ogg", ENUM_CLASS(SOUNDCH::SOUND_ENEMYWEAPON_ETC)); break;
	case 1: m_pGameInstance->PlaySoundFixed(L"crystal_crush2.ogg", ENUM_CLASS(SOUNDCH::SOUND_ENEMYWEAPON_ETC)); break;
	case 2: m_pGameInstance->PlaySoundFixed(L"crystal_crush3.ogg", ENUM_CLASS(SOUNDCH::SOUND_ENEMYWEAPON_ETC)); break;
	default:
		break;
	}
	

	return true;
}


HRESULT CEnemy::Ready_Components(void* pArg)
{
	// 컴포넌트 준비
	_uint iDestLevelIndex = m_pGameInstance->Get_DestLevel();
	ENEMY_DESC* pDesc = static_cast<ENEMY_DESC*>(pArg);


	m_pTransformCom->Scale(_float3{ 5.f, 5.f, 5.f });

	if (FAILED(CGameObject::Add_Component(iDestLevelIndex, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_Enemy"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom), nullptr)))
		return E_FAIL;
	Set_BufferRef(m_pModelCom);

	CNavigation::NAVIGATION_DESC        NaviDesc{};
	switch (iDestLevelIndex)
	{
	case ENUM_CLASS(LEVEL::TEST_EXTRA1):	NaviDesc.iCurrentCellIndex = 25;	break;

	default:								NaviDesc.iCurrentCellIndex = 0;		break;
	}

	NaviDesc.iCurrentCellIndex = pDesc->iFirstCellIndex;

	if (FAILED(CGameObject::Add_Component(iDestLevelIndex, TEXT("Prototype_Component_Navigation"),
		TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom), &NaviDesc)))
		return E_FAIL;

	if (FAILED(Ready_Colliders(pArg)))
		return E_FAIL;


	return S_OK;
}

HRESULT CEnemy::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_Shader_Resource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
		return E_FAIL;
	


	if (FAILED(m_pShaderCom->Bind_RawValue("g_fFadeDeltaRatio", &m_fDeadDeltaTime, sizeof(_float))))
		return E_FAIL;


	_float4x4 matWorldInv = {};
	XMStoreFloat4x4(&matWorldInv, m_pTransformCom->Get_WorldMatrix_Inverse());
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrixInv", &matWorldInv)))
		return E_FAIL;


	return S_OK;
}

HRESULT CEnemy::Ready_PartObject(_uint iObjType, void* pArg)
{
	CWeapon::WEAPON_DESC		WeaponDesc{};
	WeaponDesc.pState = &m_iState;
	WeaponDesc.pSocketMatrix = m_pModelCom->Get_BoneMatrix("RightHand");
	//WeaponDesc.pSocketMatrix = m_pGameInstance->Get_Transform_Float4x4_Inverse(D3DTS::VIEW);
	WeaponDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
	WeaponDesc.pParentTarget = this;

	//if (FAILED(__super::Add_PartObject(TEXT("Part_Weapon_Enemy"), ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Weapon_Pistol"), &WeaponDesc)))
	//	return E_FAIL;
	
	if (pArg)
		WeaponDesc.iCurLeftBullets = static_cast<CWeapon_Gun::GUNINFO_DESC*>(pArg)->iCurLeftBullets;

	_wstring strObjPrototypeTag = {};
	switch (iObjType)
	{
	case ENUM_CLASS(GAMEOBJ_TYPE::WEAPON_RANGED_KARABIN):	strObjPrototypeTag = L"Prototype_GameObject_Weapon_Karabin";	break;
	case ENUM_CLASS(GAMEOBJ_TYPE::WEAPON_RANGED_PISTOL):	strObjPrototypeTag = L"Prototype_GameObject_Weapon_Pistol";		break;
	case ENUM_CLASS(GAMEOBJ_TYPE::WEAPON_RANGED_SHOTGUN):	strObjPrototypeTag = L"Prototype_GameObject_Weapon_Shotgun";	break;
	default:
		break;
	}

	if (FAILED(__super::Add_PartObject(TEXT("Part_Weapon_Enemy"), ENUM_CLASS(LEVEL::STATIC), strObjPrototypeTag, &WeaponDesc)))
		return E_FAIL;

	m_pPart_Weapon = Find_PartObject(L"Part_Weapon_Enemy");

	return S_OK;
}

void CEnemy::Update_Transform(_float fTimeDelta)
{
	if (!m_listPreMovePoses.empty())
	{
		Update_Transform_PreMove(fTimeDelta);
		return;
	}

	_uint iDestLevel = m_pGameInstance->Get_DestLevel();
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Find_GameObject(iDestLevel, L"Layer_Player"));
	CTransform* pPlayerTransformCom = dynamic_cast<CTransform*>(m_pGameInstance->Find_Component(iDestLevel, L"Layer_Player", L"Com_Transform"));
	CTransform* pNearestWeaponTransformCom = (m_pNearestWeapon)? dynamic_cast<CTransform*>(m_pNearestWeapon->Get_Component(L"Com_Transform")) : nullptr;
	
	_float fDist = XMVectorGetX(XMVector3Length(m_pTransformCom->Get_Position() - pPlayerTransformCom->Get_Position()));

	CWeapon_Gun* pWeaponGun = dynamic_cast<CWeapon_Gun*>(m_pPart_Weapon);

	_bool isNearExistWeapon = false;

	if (!(m_iState & ENUM_CLASS(ENEMY_STATE::IDLE) &&
		(m_iState & ENUM_CLASS(ENEMY_STATE::MOVE))))
	{
		// 타겟을 바라보고 다가오도록
		_vector vCurrentLook = m_pTransformCom->Get_State(STATE::LOOK); // 현재 바라보는 방향
		_vector vTargetDir = {};

		if (m_iState & ENUM_CLASS(ENEMY_STATE::TRACK_PLAYER))
			vTargetDir = XMVector3Normalize(pPlayerTransformCom->Get_Position() - m_pTransformCom->Get_Position()); // 목표 위치 방향
		else if (m_iState & ENUM_CLASS(ENEMY_STATE::TRACK_WEAPON))
			if (m_pNearestWeapon)
				vTargetDir = XMVector3Normalize(pNearestWeaponTransformCom->Get_Position() - m_pTransformCom->Get_Position());

		_float vDeg = TO_DEG(acosf(XMVectorGetX(XMVector3Dot(vCurrentLook, vTargetDir))));	// 바라보는 방향과 목표물의 각도 차이

		_vector vCross = XMVector3Cross(vCurrentLook, vTargetDir);
		_float fDir = (XMVectorGetY(vCross) >= 0.f) ? +1.f : -1.f;

		if (vDeg > 20)
			m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fDir * fTimeDelta);





		if (m_iState & ENUM_CLASS(ENEMY_STATE::TRACK_PLAYER))
			m_pTransformCom->Chase(pPlayerTransformCom->Get_Position(), fTimeDelta, 0.5f, m_pNavigationCom);
		else if (m_iState & ENUM_CLASS(ENEMY_STATE::TRACK_WEAPON))
			if (m_pNearestWeapon)
				m_pTransformCom->Chase(pNearestWeaponTransformCom->Get_Position(), fTimeDelta, 0.5f, m_pNavigationCom);



	}

	if (m_iState & ENUM_CLASS(ENEMY_STATE::TRACK_WEAPON))
	{
		if (m_pNearestWeapon)
		{
			_vector vWeaponPos = pNearestWeaponTransformCom->Get_Position();
			m_pTransformCom->LookAt(XMVectorSetY(vWeaponPos, m_pTransformCom->Get_Position_Store().y));
		}
	}
	else 
		m_pTransformCom->LookAt(pPlayerTransformCom->Get_Position());

}

void CEnemy::Update_Transform_PreMove(_float fTimeDelta)
{
	if (m_listPreMovePoses.empty())
		return;

	const _float fTargetDist = 1.5f;		// 목표까지 해당 거리 이하가 되면 도달한 것으로 간주
	_vector vTargetPos = XMVectorSetW(XMLoadFloat3(&m_listPreMovePoses.front()), 1.f);

	// PreMove - 해당 위치로 이동함
	m_pTransformCom->Chase(vTargetPos, fTimeDelta, fTargetDist / 2.f, m_pNavigationCom);
	_float fDist = XMVectorGetX(XMVector3Length(m_pTransformCom->Get_Position() - vTargetPos));

	// PreMove - 해당 위치를 바라봄
	m_pTransformCom->LookAt_Smooth(XMVectorSetY(vTargetPos, m_pTransformCom->Get_Position_Store().y), fTimeDelta);

	// Premove - 목표 도달 시 맨 앞 원소 제거하여 다음 목표로 전환.
	if (fDist < fTargetDist)
	{
		m_listPreMovePoses.pop_front();
	}
}

void CEnemy::Update_AnimationState(_float fTimeDelta)
{
	// 상태 추가 (켜기) → |=
	// 상태 제거 (끄기) → &= ~
	// 
	// 상태 토글 (반전) → ^=
	// 상태 확인 (켜져 있는지 검사) → &

	if (!m_listPreMovePoses.empty())
	{
		m_iState = ENUM_CLASS(ENEMY_STATE::MOVE);
		return;
	}



	_float fShotInterval = 3.5f;

	_uint iDestLevel = m_pGameInstance->Get_DestLevel();
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Find_GameObject(iDestLevel, L"Layer_Player"));
	CTransform* pPlayerTransformCom = dynamic_cast<CTransform*>(m_pGameInstance->Find_Component(iDestLevel, L"Layer_Player", L"Com_Transform"));

	_float fDist = XMVectorGetX(XMVector3Length(m_pTransformCom->Get_Position() - pPlayerTransformCom->Get_Position()));
	CWeapon_Gun* pWeaponGun = dynamic_cast<CWeapon_Gun*>(m_pPart_Weapon);


	// 근접 공격을 받는 경우도..

	if (/*m_isLogicTriggered &&*/ 
		!(m_iState & ENUM_CLASS(ENEMY_STATE::DMGD_L) ||
		m_iState & ENUM_CLASS(ENEMY_STATE::DMGD_U)))
	{


		if (pWeaponGun)		// [총]		무기 들고 있음
		{
			if (IS_BETWEEN(fDist, 60.f, 80.f))				// [Track]	적당히 가까이 있음
			{
				m_iState = ENUM_CLASS(ENEMY_STATE::TRACK_PLAYER);
				m_iState |= ENUM_CLASS(ENEMY_STATE::MOVE);
			}
			else if (IS_BETWEEN(fDist, 0.f, 60.f))			// [Aiming]	가까이 있음
			{
				m_iState = ENUM_CLASS(ENEMY_STATE::ATK_WEAPON_GUN);
				m_iState &= ~ENUM_CLASS(ENEMY_STATE::MOVE);
			}
			else if (fDist >= 70.f)							// [Idle]	멀리 있음 
			{
				m_iState = ENUM_CLASS(ENEMY_STATE::IDLE);
				m_iState &= ~ENUM_CLASS(ENEMY_STATE::MOVE);
			}
		}
		else if (false)		// [근접]	무기 들고 있음
		{

		}
		else				// [ X ]	든 무기 없음
		{
			if (m_pNearestWeapon)	//	근처에 무기 감지
			{
				m_iState = ENUM_CLASS(ENEMY_STATE::TRACK_WEAPON);
				m_iState |= ENUM_CLASS(ENEMY_STATE::MOVE);
			}
			else					//	근처에 무기가 없다면
			{
				if (fDist >= 20.f)
				{
					m_iState = ENUM_CLASS(ENEMY_STATE::IDLE);
					m_iState &= ~ENUM_CLASS(ENEMY_STATE::MOVE);
				}
				else if (IS_BETWEEN(fDist, 6.f, 20.f))
				{
					m_iState = ENUM_CLASS(ENEMY_STATE::TRACK_PLAYER);
					m_iState |= ENUM_CLASS(ENEMY_STATE::MOVE);
				}
				else if (IS_BETWEEN(fDist, 0.0f, 6.f))
				{
					m_iState = ENUM_CLASS(ENEMY_STATE::ATK_MELEE);
					m_iState &= ~ENUM_CLASS(ENEMY_STATE::MOVE);
				}
			}
		}


		if (m_iState & ENUM_CLASS(ENEMY_STATE::ATK_WEAPON_GUN))
		{
			if (pWeaponGun != nullptr && m_fElapsedShot > fShotInterval)
			{
				// 날아갈 방향 계산
				_vector vGunPos = XMVectorSet(pWeaponGun->Get_CombinedMatrix()._41, pWeaponGun->Get_CombinedMatrix()._42, pWeaponGun->Get_CombinedMatrix()._43, 1.f);

				_vector vDir = XMVector3Normalize(XMLoadFloat4(m_pGameInstance->Get_CamPosition()) - vGunPos);	// 방향은, 목적지(에이밍중인 방향) - 출발지(플레이어 카메라 위치) 의 정규화 값.
	
				m_vLoadShotDir = vDir;

				pWeaponGun->Shot(&m_vLoadShotDir, ENUM_CLASS(GAMEOBJ_TYPE::ENEMYBULLET));
				m_fElapsedShot = 0.f;
			}
			else
				m_fElapsedShot += fTimeDelta;
		}
	}
	

	// 공격받은 상태라면
	if (m_iState & ENUM_CLASS(ENEMY_STATE::DMGD_L) ||
		m_iState & ENUM_CLASS(ENEMY_STATE::DMGD_U))
	{
		// ksta : 피격 애니메이션 종료 시 공격받음 상태 제거..?
		//CModel::MODEL_ANIM_DESC tUpperDesc = m_pModelCom->Get_PlayingAnimDesc(PART_UPPER);
		//CModel::MODEL_ANIM_DESC tLowerDesc = m_pModelCom->Get_PlayingAnimDesc(PART_LOWER);

		// 그냥 자체 그로기 쿨타임 적용..
		if (!m_isGroggy)
		{
			m_iState &= ~ENUM_CLASS(ENEMY_STATE::DMGD_L);
			m_iState &= ~ENUM_CLASS(ENEMY_STATE::DMGD_U);
		}
	}


}

void CEnemy::Update_AnimationIndex(_float fTimeDelta)
{
	if (m_isLogicTriggered)
	{

		if  (m_iState & ENUM_CLASS(ENEMY_STATE::IDLE))
		{
			m_tAnimDesc[PART_UPPER] = { MOVE_U_IDLE, true, 0.4f };
			m_tAnimDesc[PART_LOWER] = { MOVE_L_IDLE, true, 0.4f };
		
			//m_pModelCom->Set_Animation(GUN_U_RIFLE_AIM_IDLE, PART_UPPER, true);
			//m_pModelCom->Set_Animation(GUN_L_RIFLE_AIM_IDLE, PART_LOWER, true);
		}
		else if (m_iState & ENUM_CLASS(ENEMY_STATE::MOVE))
		{
			m_tAnimDesc[PART_UPPER] = { MOVE_U_RUNNING, true };
			m_tAnimDesc[PART_LOWER] = { MOVE_L_RUNNING, true };
		}



		if (m_iState & ENUM_CLASS(ENEMY_STATE::ATK_MELEE))
		{
			m_tAnimDesc[PART_UPPER] = { MELEE_U_FIST_02, true };
			m_tAnimDesc[PART_LOWER] = { MOVE_L_IDLE, true };
			//m_tAnimDesc[PART_UPPER] = { MELEE_U_FIST_03, true };
		}
		else if (m_iState & ENUM_CLASS(ENEMY_STATE::ATK_WEAPON_GUN))
		{
			//switch (m_pPart_Weapon->Get_ObjType())
			//{
			//case ENUM_CLASS(GAMEOBJ_TYPE::WEAPON_RANGED_PISTOL):
			//	m_tAnimDesc[PART_UPPER] = { GUN_U_DISARMED, true };				break;
			//case ENUM_CLASS(GAMEOBJ_TYPE::WEAPON_RANGED_KARABIN):
			//case ENUM_CLASS(GAMEOBJ_TYPE::WEAPON_RANGED_SHOTGUN):
				m_tAnimDesc[PART_UPPER] = { GUN_U_RIFLE_AIM_IDLE, true };		//break;
			//}

			if (!(m_iState & ENUM_CLASS(ENEMY_STATE::MOVE)))
			{
				//switch (m_pPart_Weapon->Get_ObjType())
				//{
				//case ENUM_CLASS(GAMEOBJ_TYPE::WEAPON_RANGED_PISTOL):
				//	m_tAnimDesc[PART_LOWER] = { GUN_L_DISARMED, true };				break;
				//case ENUM_CLASS(GAMEOBJ_TYPE::WEAPON_RANGED_KARABIN):
				//case ENUM_CLASS(GAMEOBJ_TYPE::WEAPON_RANGED_SHOTGUN):
					m_tAnimDesc[PART_LOWER] = { GUN_L_RIFLE_AIM_IDLE, true };		//break;
				//}
			}
		}
	}

	if (m_iState & ENUM_CLASS(ENEMY_STATE::DMGD_L) ||
		m_iState & ENUM_CLASS(ENEMY_STATE::DMGD_U))
	{
		if		(m_iState & ENUM_CLASS(ENEMY_STATE::DMGD_L))
		{
			m_tAnimDesc[PART_UPPER] = { DMGD_U_BIG_STOMACH_HIT, true };
			m_tAnimDesc[PART_LOWER] = { DMGD_L_BIG_STOMACH_HIT, true };
		}
		else if (m_iState & ENUM_CLASS(ENEMY_STATE::DMGD_U))
		{
			m_tAnimDesc[PART_UPPER] = { DMGD_U_HEAD_HIT, true};
			m_tAnimDesc[PART_LOWER] = { DMGD_L_HEAD_HIT, true};
		}
	}

	m_pModelCom->Set_Animation(m_tAnimDesc[PART_UPPER].iAnimIndex, PART_UPPER, m_tAnimDesc[PART_UPPER].isAnimLoop, m_tAnimDesc[PART_UPPER].fTransitionTime);
	m_pModelCom->Set_Animation(m_tAnimDesc[PART_LOWER].iAnimIndex, PART_LOWER, m_tAnimDesc[PART_LOWER].isAnimLoop, m_tAnimDesc[PART_LOWER].fTransitionTime);
}

void CEnemy::Update_LogicInterval(_float fTimeDelta)
{
	m_fLogic_ElapsedTime += fTimeDelta;
	m_isLogicTriggered = false;

	if (m_fLogic_ElapsedTime >= m_fLogic_ResetIntervalTime)
	{
		m_fLogic_ElapsedTime = 0.f;
		m_isLogicTriggered = true;

		_float fIntervalRange[2] = { 0.4f, 0.6f };
		m_fLogic_ResetIntervalTime = m_pGameInstance->Rand(fIntervalRange[0], fIntervalRange[1]);

		//std::cout << "[Enemy::Update_LogicInterval] Logic Triggered!" << std::endl;
	}

	if (m_isGroggy == true)
	{
		m_fGroggy_ElapsedTime += fTimeDelta;

		if (m_fGroggy_ElapsedTime >= m_fGroggyTime)
		{
			m_isGroggy = false;
			m_fGroggy_ElapsedTime = 0;
		}
	}
}

void CEnemy::Update_Interact(_float fTimeDelta)
{
	// 무기 가까이에 있으면 줍도록

	if (!(
		m_pPart_Weapon == nullptr &&
		(m_iState & ENUM_CLASS(ENEMY_STATE::TRACK_WEAPON)) &&
		m_pNearestWeapon != nullptr &&
		!m_isDeadStandby
		))
		return;

	_float fPickupableDist = 10.f;

	CTransform* pNearestWeaponTransform = dynamic_cast<CTransform*>(m_pNearestWeapon->Get_Component(L"Com_Transform"));
	_float fDist = XMVectorGetX(XMVector3Length(pNearestWeaponTransform->Get_Position() - m_pTransformCom->Get_Position()));


	if (fDist /* 적과 무기 간의 거리 계산 후 조건 삽입 */< fPickupableDist)
	{
		_uint iRayObjType = m_pNearestWeapon->Get_ObjType();		// 이거 가져와서 레이 대상에 따라 바뀌도록
		CWeapon_Gun::GUNINFO_DESC tGunDesc = {};

		_bool isGun = false;
		if (iRayObjType == ENUM_CLASS(GAMEOBJ_TYPE::WEAPON_RANGED_KARABIN) ||
			iRayObjType == ENUM_CLASS(GAMEOBJ_TYPE::WEAPON_RANGED_PISTOL) ||
			iRayObjType == ENUM_CLASS(GAMEOBJ_TYPE::WEAPON_RANGED_SHOTGUN))
		{
			isGun = true;
			tGunDesc = dynamic_cast<CCustomObj_Pickupable*>(m_pNearestWeapon)->Get_GunInfoDesc();
		}

		if (isGun)	Ready_PartObject(iRayObjType, &tGunDesc);
		else		Ready_PartObject(iRayObjType);

		m_pNearestWeapon->OnCollisionRay(this);
	}
}

void CEnemy::Update_BoneColliders()
{
	Update_BoneCollider(m_vecCollidersCom[ENUM_CLASS(COLLIDERTYPE::SPHERE)][0], "Head");
	Update_BoneCollider(m_vecCollidersCom[ENUM_CLASS(COLLIDERTYPE::SPHERE)][1], "LeftHand");
	Update_BoneCollider(m_vecCollidersCom[ENUM_CLASS(COLLIDERTYPE::SPHERE)][2], "RightHand");

	m_vecCollidersCom[ENUM_CLASS(COLLIDERTYPE::OBB)][0]->Update(m_pTransformCom->Get_WorldMatrix());	// Direct Update

	Update_ToggleColliders();
}

void CEnemy::Update_BoneCollider(CCollider* pCollider, const _char* szBoneName)
{
	// 객체 자체의 월드 행렬과, 본 자체의 로컬 행렬을 가져옴.
	_matrix matWorld = m_pTransformCom->Get_WorldMatrix();
	_matrix matTargetBone = XMLoadFloat4x4(m_pModelCom->Get_BoneMatrix(szBoneName));

	// 본 행렬 분리
	_vector vPos, vQuat, vSca;
	XMMatrixDecompose(&vSca, &vQuat, &vPos, matTargetBone);

	// 본 분리한걸 다시 각각의 요소별 행렬화
	_matrix matWorldPos = XMMatrixTranslationFromVector(vPos);
	_matrix matWorldRot = XMMatrixRotationQuaternion(vQuat);

	// 재조립 ( 크기는 1로 할 것이라 제외 - "자" - "이" - "공"..)
	_matrix matWorld_Calced = matWorldRot * matWorldPos * matWorld;

	pCollider->Update(matWorld_Calced);
}

HRESULT CEnemy::Ready_Colliders(void* pArg)
{	// ===== Colliders =====

	CCollider* tmpColCom = nullptr;

	CBounding_Sphere::BOUNDING_SPHERE_DESC  SphereDesc{};
	SphereDesc.vCenter = _float3(0.f, 0.f, 0.f);
	COLLISION_DESC colDesc = {};

	// s1. For Head / Zero / 0.08f
	// s2. For LeftHand / Zero / 0.08f
	// s3. For RightHand / Zero / 0.08f
	_wstring strNameTag[3] = { L"Head", L"LeftHand", L"RightHand" };
	_float	fRad[3] = { 0.12f, 0.09f, 0.09f };
	for (_uint i = 0; i < 3; i++)
	{
		if (strNameTag[i] == L"Head")
		{
			colDesc = {					// 콜라이더 충돌 레이어 및 대상 정의
				ENUM_CLASS(COLLISION_LAYER::ENEMY_HIT),
				ENUM_CLASS(COLLISION_LAYER::PLAYER_ATK) | ENUM_CLASS(COLLISION_LAYER::PLAYER_BULLET_ATK) | ENUM_CLASS(COLLISION_LAYER::THROWN),
				true, this
			};
			SphereDesc.tColDesc = colDesc;
		}
		else if (strNameTag[i] == L"LeftHand" ||
			strNameTag[i] == L"RightHand")
		{
			colDesc = {					// 콜라이더 충돌 레이어 및 대상 정의
				/*ENUM_CLASS(COLLISION_LAYER::ENEMY_ATK) | */ENUM_CLASS(COLLISION_LAYER::ENEMY_HIT),
				ENUM_CLASS(COLLISION_LAYER::PLAYER_HIT),
				false, this
			};
			SphereDesc.tColDesc = colDesc;
		}

		SphereDesc.fRadius = fRad[i];
		if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Collider_Sphere"),
			TEXT("Com_Collider_") + strNameTag[i], reinterpret_cast<CComponent**>(&tmpColCom), &SphereDesc)))
			return E_FAIL;
		m_vecCollidersCom[ENUM_CLASS(COLLIDERTYPE::SPHERE)].push_back(tmpColCom);
	}

	CBounding_OBB::BOUNDING_OBB_DESC  OBBDesc{};
	OBBDesc.vAngles = _float3(0.f, 0.f, 0.f);
	OBBDesc.vExtents = _float3(0.15f, 0.75f, 0.10f);
	OBBDesc.vCenter = _float3(0.f, OBBDesc.vExtents.y, 0.f);
	colDesc = {
		ENUM_CLASS(COLLISION_LAYER::ENEMY_HIT),
		ENUM_CLASS(COLLISION_LAYER::PLAYER_ATK) | ENUM_CLASS(COLLISION_LAYER::PLAYER_BULLET_ATK) | ENUM_CLASS(COLLISION_LAYER::THROWN),
		true, this
	};
	OBBDesc.tColDesc = colDesc;

	// o1. Fol BodyAll (Not Specific Bone) / Zero / .1 .82 .1 / 0 .82 0 
	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider_BodyAll"), reinterpret_cast<CComponent**>(&tmpColCom), &OBBDesc)))
		return E_FAIL;
	m_vecCollidersCom[ENUM_CLASS(COLLIDERTYPE::OBB)].push_back(tmpColCom);

	return S_OK;
}


void CEnemy::Update_ToggleColliders()
{
	_bool isFistPlaying = false;

	CModel::MODEL_ANIM_DESC tCurAnimDesc = m_pModelCom->Get_PlayingAnimDesc(PART_UPPER);
	if (tCurAnimDesc.iCurAnimIndex == MELEE_U_FIST_01 ||
		tCurAnimDesc.iCurAnimIndex == MELEE_U_FIST_02 ||
		tCurAnimDesc.iCurAnimIndex == MELEE_U_FIST_03 ||
		tCurAnimDesc.iCurAnimIndex == MELEE_U_FIST_04)

		isFistPlaying = true;

	// 주먹이 나가는 중이라면
	if (isFistPlaying && (m_iState & ENUM_CLASS(ENEMY_STATE::ATK_MELEE)))
	{
		// isActive 끄고, 콜라이더 내에서 isActive off 시 충돌 처리 안하도록 로직 제작
		m_vecCollidersCom[ENUM_CLASS(COLLIDERTYPE::SPHERE)][1]->Set_isActive(true);			// LeftHand
		m_vecCollidersCom[ENUM_CLASS(COLLIDERTYPE::SPHERE)][2]->Set_isActive(true);			// RightHand
	}
	else
	{
		m_vecCollidersCom[ENUM_CLASS(COLLIDERTYPE::SPHERE)][1]->Set_isActive(false);		// LeftHand
		m_vecCollidersCom[ENUM_CLASS(COLLIDERTYPE::SPHERE)][2]->Set_isActive(false);		// RightHand
	}
}

void CEnemy::Update_NearestWeapons()
{
	// 가진 무기가 없을 때만 탐색
	CWeapon_Gun* pWeaponGun = dynamic_cast<CWeapon_Gun*>(m_pPart_Weapon);
	m_pNearestWeapon = nullptr;
	if (pWeaponGun) return;


	// 픽업 레이어에서 순회 돌아서 가장 가까운 무기 탐색
	_uint iDestLevel = m_pGameInstance->Get_DestLevel();
	_wstring strWeaponLayerTag = L"Layer_Loaded_Object_Pickupable";

	vector<CGameObject*> vecDroppedWeapons = {};

	_uint iIndex = 0;
	while (true)
	{
		CGameObject* pWeapon = m_pGameInstance->Find_GameObject(iDestLevel, strWeaponLayerTag, iIndex++);
		if (pWeapon == nullptr) break;


		// 콜라이더 있는지 검사. 없으면 throw 된 것으로 간주하고 포함 X
		// !! 이렇게 하면 안되고, 콜라이더의 인덱스 타입이 THROW인지로 확인해야 할 것 같음
		// !! 또한 pickingup중인 것도 제외해야 함
		_bool isNotPickupable = false;

		vector<CCollider*>* vecColliders = pWeapon->Get_Colliders();
		for (_uint i = 0; i < ENUM_CLASS(COLLIDERTYPE::END); i++)
			for (auto& collider : vecColliders[i])
				if ((collider->Get_ColDesc().iLayerIndex & ENUM_CLASS(COLLISION_LAYER::THROWN)) &&
					isNotPickupable == false)
				{
					isNotPickupable = true;
					break;
				}

		if (isNotPickupable == false)
			isNotPickupable = static_cast<CCustomObj_Pickupable*>(pWeapon)->Get_IsPickingUp();

		if (isNotPickupable)
			continue;

		vecDroppedWeapons.push_back(pWeapon);
	}


	// 가까운 무기가 없으면 초기화 후 종료
	if (vecDroppedWeapons.size() == 0)
	{
		m_pNearestWeapon = nullptr;
		return;
	}
		

	CGameObject* pNearestWeapon = nullptr;
	_float fNearestDistSq = FLT_MAX;

	for (auto& weapon : vecDroppedWeapons)
	{
		CTransform* pWeaponTransform = dynamic_cast<CTransform*>(weapon->Get_Component(L"Com_Transform"));
		_vector vWeaponPos = pWeaponTransform->Get_Position();
		_vector vPos = m_pTransformCom->Get_Position();

		_float fDistSq = XMVectorGetX(XMVector3LengthSq(vWeaponPos - vPos));

		if (fNearestDistSq > fDistSq)
		{
			pNearestWeapon = weapon;
			fNearestDistSq = fDistSq;
		}
	}
	
	m_pNearestWeapon = pNearestWeapon;
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

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pNavigationCom);
}

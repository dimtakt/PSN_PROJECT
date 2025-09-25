#include "Player.h"
#include "GameInstance.h"

#include "Body_Player.h"
#include "CustomObj_Pickupable.h"

//#include "Weapon.h"
#include "Weapon_Karabin.h"
#include "Weapon_Pistol.h"
#include "Weapon_Shotgun.h"

#include "UI_Crosshair.h"
#include "UI_ScreenText.h"

#include "Camera_Player.h"

CPlayer::CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CContainerObject (pDevice, pContext)
{
}

CPlayer::CPlayer(const CPlayer& Prototype)
	: CContainerObject( Prototype )
{
}

HRESULT CPlayer::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPlayer::Initialize(void* pArg)
{
	GAMEOBJECT_DESC         Desc{};
	Desc.fSpeedPerSec = 15.f;
	Desc.fRotationPerSec = XMConvertToRadians(180.0f);

	if (FAILED(__super::Initialize(&Desc)))
		return E_FAIL;

	if (FAILED(this->Ready_Components(pArg)))
		return E_FAIL;

	m_iGameObjType = ENUM_CLASS(GAMEOBJ_TYPE::PLAYER);

	m_pModelCom->Add_Animation();
	m_pModelCom->Set_Animation(MOVE_U_IDLE, PART_UPPER, true);

	m_pModelCom->Add_Animation();
	m_pModelCom->Set_Animation(MOVE_L_IDLE, PART_LOWER, true);


	_uint iDestLevel = m_pGameInstance->Get_DestLevel();
	m_pUI_Crosshair = dynamic_cast<CUI_Crosshair*>(m_pGameInstance->Find_GameObject(iDestLevel, L"Layer_UI_Crosshair"));
	if (m_pUI_Crosshair == nullptr)		return E_FAIL;
	m_pUI_ScreenText = dynamic_cast<CUI_ScreenText*>(m_pGameInstance->Find_GameObject(iDestLevel, L"Layer_UI_ScreenText"));
	if (m_pUI_ScreenText == nullptr)	return E_FAIL;
	m_pCameraPlayer = dynamic_cast<CCamera_Player*>(m_pGameInstance->Find_GameObject(iDestLevel, L"Layer_Camera"));
	if (m_pCameraPlayer == nullptr)	return E_FAIL;

	switch (iDestLevel)
	{
	case ENUM_CLASS(LEVEL::CH01_KICK):
		Ready_PartObject(ENUM_CLASS(GAMEOBJ_TYPE::WEAPON_RANGED_PISTOL));	static_cast<CWeapon_Gun*>(m_pPart_Weapon)->Set_toAttached(true);	break;
	default:																		break;
	}


	m_pGameInstance->Req_EditTimeSpeed(0.01f, true);

	//if (FAILED(Ready_PartObject()))
	//	return E_FAIL;


	m_iMaxHp	= 1;
	m_iHp		= 1;

	return S_OK;
}

void CPlayer::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);		// 본인이 아닌, PartObject 들의 Priority_Update 를 순회함

	Update_HitCD(fTimeDelta);
}

void CPlayer::Update(_float fTimeDelta)
{
#ifdef _DEBUG	// ksta : 테스트 후 삭제

	CWeapon_Gun* pWeaponGun = dynamic_cast<CWeapon_Gun*>(m_pPart_Weapon);

	if (pWeaponGun == nullptr)
		if (m_pGameInstance->Get_IsKeyDown(DIK_O))
		{
			if (FAILED(Ready_PartObject(ENUM_CLASS(GAMEOBJ_TYPE::WEAPON_RANGED_KARABIN))))
				return;
			if (m_pPart_Weapon)
				static_cast<CWeapon*>(m_pPart_Weapon)->Set_toAttached(true);
		}

	if (m_pGameInstance->Get_IsKeyDown(DIK_L))
	{
		_float3 vPlayerPosDebug = m_pTransformCom->Get_Position_Store();
		std::cout << "[CPlayer::Update] Player Current Pos : { " << vPlayerPosDebug.x << "f, " << vPlayerPosDebug.y << "f, " << vPlayerPosDebug.z << "f }" << std::endl;

		std::cout << "[CPlayer::Update] Player Current NavIndex : " << m_pNavigationCom->Get_CurrentCellIndex() << std::endl;


		std::cout << "[CPlayer::Update] Player Current ZRot(Deg) : " << m_pTransformCom->Get_RotationEuler_Store().y << std::endl;
	}

#endif // _DEBUG



	// 행동 패턴 등.. 추후 컴포넌트 등을 이용하여 구현
	// 함수 꼭 분리해서 난잡하지 않게 만들기
	_float fRawTimeDelta = fTimeDelta / (m_pGameInstance->Get_TimeSpeed());

	Update_TimeControl(fTimeDelta);

	Update_Transform(fTimeDelta);
	Update_AnimationState(fTimeDelta);
	Update_AnimationIndex(fTimeDelta);

	Update_Interact(fTimeDelta);
	Update_UI(fTimeDelta);

#ifdef _DEBUG
	//m_pGameInstance->Req_EditTimeSpeed(1.0f, true);
#endif // _DEBUG

	m_pModelCom->Play_Animation_AllLayer(fRawTimeDelta);
	//m_pModelCom->Play_Animation(fTimeDelta, PART_LOWER);
	Update_BoneColliders();

	




	__super::Update(fTimeDelta);
}

void CPlayer::Late_Update(_float fTimeDelta)
{
	m_pTransformCom->Set_State(Engine::STATE::POSITION,
		m_pNavigationCom->Compute_OnCell(m_pTransformCom->Get_State(Engine::STATE::POSITION)));


	if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::NONBLEND, this)))
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

HRESULT CPlayer::Render()
{
	// 렌더
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

#ifdef _DEBUG

	_wstring strFontTag = L"Font_DOS";
	_uint iRenderStartX = 100.f;
	_uint iRenderStartY = 25.f;
	_uint iRenderSpaceY = 25.f;
	_vector vLoadColor = XMVectorSet(0.f, 0.f, 0.f, 1.f);
	_vector vBackColor = XMVectorSet(1.f, 1.f, 1.f, 1.f);

	m_pGameInstance->Render_Font_Begin(strFontTag);
	for (_uint i = 0; i < m_pModelCom->Get_NumPlayingAnims(); i++)
	{
		m_pGameInstance->Render_Font(strFontTag, L"██████████████████████████████████████████", _float2(iRenderStartX, iRenderStartY += iRenderSpaceY), vBackColor);
		m_pGameInstance->Render_Font(strFontTag, m_pModelCom->Get_CurAnimNames()[i].c_str(), _float2(iRenderStartX, iRenderStartY), vLoadColor);
	}
	m_pGameInstance->Render_Font_End(strFontTag);


#endif // _DEBUG

	// 신체는 주먹 공격 중에만 렌더할 것임.
	if (!(m_iState & ENUM_CLASS(PLAYER_STATE::ATK)) ||
		(m_pPart_Weapon != nullptr)) // 공격 상태가 아니거나, 무기가 있으면 return
		return S_OK;



	_uint           iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Materials(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE, 0)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i)))
			return E_FAIL;

		m_pShaderCom->Begin(0);

		m_pModelCom->Render(i);
	}


	//CPartObject* pBody = Find_PartObject(TEXT("Part_Body"));
	//if (nullptr == pBody)
	//	return E_FAIL;

	



	return S_OK;
}

HRESULT CPlayer::Render_Shadow()
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


HRESULT CPlayer::Ready_Components(void* pArg)
{
	// 컴포넌트 준비
	_uint iDestLevelIndex = m_pGameInstance->Get_DestLevel();

	m_pTransformCom->Scale(_float3{5.f, 5.f, 5.f});

	if (FAILED(CGameObject::Add_Component(iDestLevelIndex, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
		return E_FAIL;

	//if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_Enemy"),
	//	TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom), nullptr)))
	//	return E_FAIL;
	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_Player"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom), nullptr)))
		return E_FAIL;
	Set_BufferRef(m_pModelCom);

	CNavigation::NAVIGATION_DESC        NaviDesc{};

	// 레벨이 추가될 때 마다 추가.. 추후 이걸 define쪽에 옮기는 것도 고려..
	switch (iDestLevelIndex)
	{
	case ENUM_CLASS(LEVEL::TEST_EXTRA1):	NaviDesc.iCurrentCellIndex = 25;	break;
	case ENUM_CLASS(LEVEL::CH01_KICK):		NaviDesc.iCurrentCellIndex = 0;		break;
	case ENUM_CLASS(LEVEL::CH09_FIGHTC):	NaviDesc.iCurrentCellIndex = 3;		break;
	case ENUM_CLASS(LEVEL::CH10_DESPER):	NaviDesc.iCurrentCellIndex = 1;		break;

	default:								NaviDesc.iCurrentCellIndex = 0;		break;
	}
	

	if (FAILED(CGameObject::Add_Component(iDestLevelIndex, TEXT("Prototype_Component_Navigation"),
		TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom), &NaviDesc)))
		return E_FAIL;

	CBounding_OBB::BOUNDING_OBB_DESC  OBBDesc{};
	OBBDesc.vAngles = _float3(0.f, 0.f, 0.f);
	OBBDesc.vExtents = _float3(0.1f, 0.82f, 0.1f);
	OBBDesc.vCenter = _float3(0.f, OBBDesc.vExtents.y, 0.f);


	if (FAILED(Ready_Colliders(pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_Shader_Resource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
		return E_FAIL;



	//if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_ShadowLight_Transform_Float4x4(D3DTS::VIEW))))
	//	return E_FAIL;
	//if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_ShadowLight_Transform_Float4x4(D3DTS::PROJ))))
	//	return E_FAIL;

	return S_OK;
}

HRESULT CPlayer::Ready_PartObject(_uint iObjType, void* pArg)
{
	_uint iDestLevel = m_pGameInstance->Get_DestLevel();

#pragma region Legacy : CBody_Player

	//CBody_Player::BODY_DESC     BodyDesc{};
	//BodyDesc.pState = &m_iState;
	//BodyDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
	//
	//if (FAILED(__super::Add_PartObject(TEXT("Part_Body"), iDestLevel, TEXT("Prototype_GameObject_Body_Player"), &BodyDesc)))
	//	return E_FAIL;
	//
	//CPartObject* pBody = Find_PartObject(TEXT("Part_Body"));
	//if (nullptr == pBody)
	//	return E_FAIL;


#pragma endregion

	CWeapon::WEAPON_DESC		WeaponDesc{};
	WeaponDesc.pState = &m_iState;
	WeaponDesc.pSocketMatrix = m_pModelCom->Get_BoneMatrix("root");
	//WeaponDesc.pSocketMatrix = m_pGameInstance->Get_Transform_Float4x4_Inverse(D3DTS::VIEW);
	WeaponDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
	WeaponDesc.pParentTarget = this;

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

	if (FAILED(__super::Add_PartObject(TEXT("Part_Weapon_Player"), ENUM_CLASS(LEVEL::STATIC), strObjPrototypeTag, &WeaponDesc)))
		return E_FAIL;

	m_pPart_Weapon = Find_PartObject(L"Part_Weapon_Player");
	
	return S_OK;
}

void CPlayer::Update_Transform(_float fTimeDelta)
{
	_float fRawTimeDelta = fTimeDelta / (m_pGameInstance->Get_TimeSpeed());

	
	if (m_pGameInstance->Get_IsKeyPressing(DIK_S))
	{
		m_pTransformCom->Go_Backward(fRawTimeDelta, m_pNavigationCom);
	}
	if (m_pGameInstance->Get_IsKeyPressing(DIK_A))
	{
		m_pTransformCom->Go_Left(fRawTimeDelta, m_pNavigationCom);
	}
	if (m_pGameInstance->Get_IsKeyPressing(DIK_D))
	{
		m_pTransformCom->Go_Right(fRawTimeDelta, m_pNavigationCom);
	}
	if (m_pGameInstance->Get_IsKeyPressing(DIK_W))
	{
		m_pTransformCom->Go_Straight(fRawTimeDelta, m_pNavigationCom);
	}

	_int iMouseMove;
	if (iMouseMove = m_pGameInstance->Get_DIMouseMove(MOUSEMOVESTATE::X))
		m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fRawTimeDelta * iMouseMove * m_fMouseSensor);
}

void CPlayer::Update_AnimationState(_float fTimeDelta)
{

	// 상태 추가 (켜기) → |=
	// 상태 제거 (끄기) → &= ~
	// 
	// 상태 토글 (반전) → ^=
	// 상태 확인 (켜져 있는지 검사) → &
	
	_float fRawTimeDelta = fTimeDelta / (m_pGameInstance->Get_TimeSpeed());

	// 공격 상태 제어

	static _float fFistProgressTime = 0.f;
	_float fFistPlayTime = 1.0f;

	if (m_iState & ENUM_CLASS(PLAYER_STATE::ATK))
		fFistProgressTime += fRawTimeDelta;

	if (m_pGameInstance->Get_IsKeyDown(MOUSEKEYSTATE::LB) &&
		!(m_iState & ENUM_CLASS(PLAYER_STATE::ATK)) &&
		m_pPart_Weapon == nullptr)
	{
		m_iState |= ENUM_CLASS(PLAYER_STATE::ATK);
	}

	if (m_iState & ENUM_CLASS(PLAYER_STATE::ATK) &&
		//fFistProgressTime >= fFistPlayTime &&	// 현재 애니메이션이 종료 될 시 state 회수
		m_pModelCom->Get_PlayingAnimDesc(PART_UPPER).isFinished)	// 현재 애니메이션이 종료 될 시 state 회수
	{
		m_iState &= ~ENUM_CLASS(PLAYER_STATE::ATK);
		fFistProgressTime = 0.f;
	}


	// 이동 상태 제어
	if (m_pGameInstance->Get_IsKeyPressing(DIK_S) ||
		m_pGameInstance->Get_IsKeyPressing(DIK_A) ||
		m_pGameInstance->Get_IsKeyPressing(DIK_D) ||
		m_pGameInstance->Get_IsKeyPressing(DIK_W))
	{
		m_iState |= ENUM_CLASS(PLAYER_STATE::MOVE);
	}
	else
	{
		m_iState &= ~ENUM_CLASS(PLAYER_STATE::MOVE);
	}






	if (m_iState == 0)
		m_iState = ENUM_CLASS(PLAYER_STATE::IDLE);
	else if (m_iState & ~ENUM_CLASS(PLAYER_STATE::IDLE))
		m_iState &= ~ENUM_CLASS(PLAYER_STATE::IDLE);




}

void CPlayer::Update_AnimationIndex(_float fTimeDelta)
{
	// 변수화를 하여 중간에 Set_Animation 중복 호출을 방지
	// (중복 호출 시 내부적으로 Prev Animation 이 바뀌어 문제 발생
	
	_float fRawTimeDelta = fTimeDelta / (m_pGameInstance->Get_TimeSpeed());


	static ANIMARG_DESC tAnimDesc[PART_END] = {};

	// ==============================

	// 가만히 있는 상태	
	if (m_iState & ENUM_CLASS(PLAYER_STATE::IDLE))
	{
		tAnimDesc[PART_UPPER] = { MOVE_U_IDLE , true };
		tAnimDesc[PART_LOWER] = { MOVE_L_IDLE , true };
	}


	// 이동 중에만 동작
	if (m_iState & ENUM_CLASS(PLAYER_STATE::MOVE))
	{
		if (m_pGameInstance->Get_IsKeyPressing(DIK_S))
		{	
			tAnimDesc[PART_UPPER] = { MOVE_U_WALKING_BACK , true };
			tAnimDesc[PART_LOWER] = { MOVE_L_WALKING_BACK , true };
		}
		if (m_pGameInstance->Get_IsKeyPressing(DIK_A))
		{	
			tAnimDesc[PART_UPPER] = { MOVE_U_LEFT_STRAFE_WALK , true };
			tAnimDesc[PART_LOWER] = { MOVE_L_LEFT_STRAFE_WALK , true };
		}
		if (m_pGameInstance->Get_IsKeyPressing(DIK_D))
		{	
			tAnimDesc[PART_UPPER] = { MOVE_U_RIGHT_STRAFE_WALK , true };
			tAnimDesc[PART_LOWER] = { MOVE_L_RIGHT_STRAFE_WALK , true };
		}
		if (m_pGameInstance->Get_IsKeyPressing(DIK_W))
		{
			tAnimDesc[PART_UPPER] = { MOVE_U_WALKING , true };
			tAnimDesc[PART_LOWER] = { MOVE_L_WALKING , true };
		}
	}
	

	// 공격 중에만 동작. 누른 후 일정 시간동안만 유지 (state에서 관리)
	static _bool isFistPlaying = false;			// 현재 fist 중인디
	static _float fFistTimeDelta = {};			// 마지막으로 fist 한 지 경과시간
	static _uint iRandFistIndex = {};			// 애니메이션 랜덤 적용
	static _uint iPastFistIndex = UINT_MAX;		// 마지막으로 재생된 fist 애니메이션 인덱스

	//_float fFistComboExistTime = 0.2f;

	if (m_pPart_Weapon == nullptr) // 총을 들고 있지 않다면
	{
		if ((m_iState & ENUM_CLASS(PLAYER_STATE::ATK)) && !isFistPlaying)		// 시작
		{
			isFistPlaying = true;

			iRandFistIndex = static_cast<_uint>(m_pGameInstance->Rand(1, 3));
			iPastFistIndex = iRandFistIndex;

			fFistTimeDelta = 0.f;

			switch (iRandFistIndex)
			{
			default:
				//case 0:		tAnimDesc[PART_UPPER] = { MELEE_U_FIST_01 , false };  break;
			case 1:		tAnimDesc[PART_UPPER] = { MELEE_U_FIST_02 , false };  break;
			case 2:		tAnimDesc[PART_UPPER] = { MELEE_U_FIST_03 , false };  break;
				//case 3:		tAnimDesc[PART_UPPER] = { MELEE_U_FIST_04 , false };  break;
			}

			isFistPlaying = true;

		}
		else if ((m_iState & ENUM_CLASS(PLAYER_STATE::ATK)) && isFistPlaying)		// 진행중
		{
			fFistTimeDelta += fRawTimeDelta;

			switch (iRandFistIndex)
			{
			default:
				//case 0:		tAnimDesc[PART_UPPER] = { MELEE_U_FIST_01 , false };  break;
			case 1:		tAnimDesc[PART_UPPER] = { MELEE_U_FIST_02 , false };  break;
			case 2:		tAnimDesc[PART_UPPER] = { MELEE_U_FIST_03 , false };  break;
				//case 3:		tAnimDesc[PART_UPPER] = { MELEE_U_FIST_04 , false };  break;
			}

		}
		else if (!(m_iState & ENUM_CLASS(PLAYER_STATE::ATK)) && isFistPlaying)	// 종료
		{
			fFistTimeDelta += fRawTimeDelta;
			isFistPlaying = false;
			iRandFistIndex = {};
		}
	}
	else	// 총을 들고 있다면
	{
		fFistTimeDelta += fRawTimeDelta;
		isFistPlaying = false;
		iRandFistIndex = {};
	}

	m_pModelCom->Set_Animation(tAnimDesc[PART_UPPER].iAnimIndex, PART_UPPER, tAnimDesc[PART_UPPER].isAnimLoop, tAnimDesc[PART_UPPER].fTransitionTime);
	m_pModelCom->Set_Animation(tAnimDesc[PART_LOWER].iAnimIndex, PART_LOWER, tAnimDesc[PART_LOWER].isAnimLoop, tAnimDesc[PART_LOWER].fTransitionTime);
}

void CPlayer::Update_TimeControl(_float fTimeDelta)
{
	// 특정 키를 누르는 동안에는 시간 속도를 고정,
	// 그 외의 경우 원래 속도(느린)대로 정상화

	_float fRawTimeDelta = fTimeDelta / (m_pGameInstance->Get_TimeSpeed());
	CWeapon_Gun* pWeaponGun = dynamic_cast<CWeapon_Gun*>(m_pPart_Weapon);

	static _float fElapsedTime = 0.f;
	static _bool isPressed = false;
	static _float fDuration = 0.05f;						// 잠시동안 속도가 유지될 시간

	if (
		m_pGameInstance->Get_IsKeyPressing(DIK_W) ||
		m_pGameInstance->Get_IsKeyPressing(DIK_A) ||
		m_pGameInstance->Get_IsKeyPressing(DIK_S) ||
		m_pGameInstance->Get_IsKeyPressing(DIK_D) ||
		false)										// 조작을 하는 동안 시간 강제로 빠르게
	{
		m_pGameInstance->Req_EditTimeSpeed(1.f, true);
		fElapsedTime = 0;
		isPressed = true;
		fDuration = 0.015f;
	}
	else if (
		m_pGameInstance->Get_IsKeyDown(MOUSEKEYSTATE::LB)
		)
	{
		if ((pWeaponGun && pWeaponGun->Get_isOnCD()))			// 총이 있다면 쿨이 아니라서 격발될때
		{
			m_pGameInstance->Req_EditTimeSpeed(1.f, true);
			fElapsedTime = 0;
			isPressed = true;
			fDuration = 0.015f;
		}
		else if (!pWeaponGun && !(m_iState & ENUM_CLASS(PLAYER_STATE::ATK)))		// 총이 없다면 공격중이 아닐 때
		{
			m_pGameInstance->Req_EditTimeSpeed(1.f, true);
			fElapsedTime = 0;
			isPressed = true;
			fDuration = 0.015f;
		}
	}
	else if (
		m_pGameInstance->Get_IsKeyDown(MOUSEKEYSTATE::RB) &&
		pWeaponGun
		)
	{
		m_pGameInstance->Req_EditTimeSpeed(1.f, true);
		fElapsedTime = 0;
		isPressed = true;
		fDuration = 0.015f;
	}
	else if ((fElapsedTime < fDuration) && isPressed)		// 조작을 하지 않는 동안 시간 복원까지 유예 타이머 진행
	{
		m_pGameInstance->Req_EditTimeSpeed(1.f, true);
		fElapsedTime += fRawTimeDelta;
	}




	else if (fElapsedTime >= fDuration)	// 조작하지 않은 지 일정 시간 지나면 시간 정상화 요청
	{
		m_pGameInstance->Req_EditTimeSpeed(0.01f);
		fElapsedTime = 0;
		isPressed = false;
	}

	//std::cout << "[CPlayer::Update_TimeControl] Current Time Multiplier : " << m_pGameInstance->Get_TimeSpeed() << std::endl;
}

void CPlayer::Update_Interact(_float fTimeDelta)
{
	// 방향은, 목적지(에이밍중인 방향) - 출발지(플레이어 카메라 위치) 의 정규화 값.

	_matrix matCameraview = m_pGameInstance->Get_Transform_Matrix_Inverse(D3DTS::VIEW);
	_vector vCameraLook = matCameraview.r[2];
	m_vLoadShotDir = vCameraLook;

	if (m_pGameInstance->Get_IsKeyDown(MOUSEKEYSTATE::LB))
	{
		CWeapon_Gun* pWeaponGun = dynamic_cast<CWeapon_Gun*>(m_pPart_Weapon);

		if (pWeaponGun)
		{
			// 날아갈 방향 계산
			pWeaponGun->Shot(&m_vLoadShotDir, ENUM_CLASS(GAMEOBJ_TYPE::PLAYERBULLET));
		}
		else
		{
			// 무기 줍기
			const _float fPickupableDist = m_fPickupableDist;

			RAYCOLLISION_DESC tRayDesc = {};
			tRayDesc.vRayPos = XMLoadFloat4(m_pGameInstance->Get_CamPosition());

			tRayDesc.vRayDir = XMVectorSetW(XMVector3Normalize(m_vLoadShotDir), 1.f);

			tRayDesc.iLayerIndex = ENUM_CLASS(COLLISION_LAYER::NONE);
			tRayDesc.iMask = ENUM_CLASS(COLLISION_LAYER::PICKUPABLE);
			tRayDesc.isActive = m_pPart_Weapon == nullptr;
			tRayDesc.pOwner = this;

			CGameObject* pRayObj = nullptr;
			_float fRayDist = FLT_MAX;
			if (m_pGameInstance->Check_RayCollisions(&tRayDesc, pRayObj, fRayDist))
			{
				if (pRayObj && fRayDist < fPickupableDist)
				{
					_uint iRayObjType = pRayObj->Get_ObjType();		// 이거 가져와서 레이 대상에 따라 바뀌도록
					CWeapon_Gun::GUNINFO_DESC tGunDesc = {};

					_bool isGun = false;
					if (iRayObjType == ENUM_CLASS(GAMEOBJ_TYPE::WEAPON_RANGED_KARABIN) ||
						iRayObjType == ENUM_CLASS(GAMEOBJ_TYPE::WEAPON_RANGED_PISTOL) ||
						iRayObjType == ENUM_CLASS(GAMEOBJ_TYPE::WEAPON_RANGED_SHOTGUN))
					{
						isGun = true;
						tGunDesc = dynamic_cast<CCustomObj_Pickupable*>(pRayObj)->Get_GunInfoDesc();
					}

					if (isGun && !dynamic_cast<CCustomObj_Pickupable*>(pRayObj)->Get_IsPickingUp())
					{
						if (isGun)	Ready_PartObject(iRayObjType, &tGunDesc);
						else		Ready_PartObject(iRayObjType);

						pRayObj->OnCollisionRay(this);
					}
				}
			}

		}
	}

	else if (m_pGameInstance->Get_IsKeyDown(MOUSEKEYSTATE::RB))
	{
		CWeapon_Gun* pWeaponGun = dynamic_cast<CWeapon_Gun*>(m_pPart_Weapon);

		if (pWeaponGun)
		{
			// 1. 현재 무기의 Transform 정보를 저장
			// 2. 날아갈 Pickupable 오브젝트 생성, 여기엔 잔탄 정보 저장 (이는 충돌 시 파괴될 것임)
			// 3. 현재 무기 삭제
			// 4. 플레이어 애니메이션 중 투척에 가까운 것으로 재생
			 

			// 바라보는 방향 및 일정 회전값을 주어 날아가도록 함.

			const _float fThrowPower = 10.f;
			const _float fThrowRotMultiply = 3.f;



			_float fRotX = m_pGameInstance->Rand(-180.f * fThrowRotMultiply, 180.f * fThrowRotMultiply);
			_float fRotY = m_pGameInstance->Rand(-180.f * fThrowRotMultiply, 180.f * fThrowRotMultiply);
			_float fRotZ = m_pGameInstance->Rand(-180.f * fThrowRotMultiply, 180.f * fThrowRotMultiply);

			_vector vRot = ROT_TO_QUAT(TO_RAD(fRotX), TO_RAD(fRotY), TO_RAD(fRotZ));
			pWeaponGun->Throw(&m_vLoadShotDir, fThrowPower, vRot, pWeaponGun->Get_ObjType());

			// 현재 사용중인 무기 삭제
			Remove_PartObject(L"Part_Weapon_Player");
			m_pPart_Weapon = nullptr;
		}

	}
}

void CPlayer::Update_UI(_float fTimeDelta)
{
	// 픽업 가능 최소거리
	const _float fPickupableDist = m_fPickupableDist;

	_uint iTextureIndex = UINT_MAX;


	if (m_pPart_Weapon)	// 총이 아닌 무기까지 추가한다면 세분화 필요
	{
		iTextureIndex = ENUM_CLASS(CROSSHAIR_INDEX::GUN);

		if (m_pPart_Weapon->Get_ObjType() == ENUM_CLASS(GAMEOBJ_TYPE::WEAPON_RANGED_PISTOL) ||
			m_pPart_Weapon->Get_ObjType() == ENUM_CLASS(GAMEOBJ_TYPE::WEAPON_RANGED_SHOTGUN))
			m_pUI_Crosshair->Change_RotByCD(dynamic_cast<CWeapon_Gun*>(m_pPart_Weapon)->Get_CDRatio());
	}
	else
	{
		RAYCOLLISION_DESC tRayDesc = {};
		tRayDesc.vRayPos = XMLoadFloat4(m_pGameInstance->Get_CamPosition());

		tRayDesc.vRayDir = m_vLoadShotDir;

		tRayDesc.iLayerIndex = ENUM_CLASS(COLLISION_LAYER::NONE);
		tRayDesc.iMask = ENUM_CLASS(COLLISION_LAYER::ENEMY_HIT) | ENUM_CLASS(COLLISION_LAYER::PICKUPABLE);
		tRayDesc.isActive = true;
		tRayDesc.pOwner = this;



		_float fRayDist = FLT_MAX;
		if (m_pGameInstance->Check_RayCollisions(&tRayDesc, m_pRayObj, m_fRayDist))
		{
			if (m_pRayObj && (m_fRayDist < fPickupableDist))
			{
				_uint iRayObjType = m_pRayObj->Get_ObjType();		// 이거 가져와서 레이 대상에 따라 바뀌도록

				switch (iRayObjType)
				{
				case ENUM_CLASS(GAMEOBJ_TYPE::WEAPON_RANGED_KARABIN):
				case ENUM_CLASS(GAMEOBJ_TYPE::WEAPON_RANGED_PISTOL):
				case ENUM_CLASS(GAMEOBJ_TYPE::WEAPON_RANGED_SHOTGUN):
					if (!dynamic_cast<CCustomObj_Pickupable*>(m_pRayObj)->Get_IsPickingUp())
						iTextureIndex = ENUM_CLASS(CROSSHAIR_INDEX::BASICHAND);		break;
				case ENUM_CLASS(GAMEOBJ_TYPE::ENEMY):
					iTextureIndex = ENUM_CLASS(CROSSHAIR_INDEX::BASICPUNCH);	break;
				default:
					iTextureIndex = ENUM_CLASS(CROSSHAIR_INDEX::BASICDOT);		break;
				}
			}
		}
		else
			iTextureIndex = ENUM_CLASS(CROSSHAIR_INDEX::BASICDOT);

		m_pUI_Crosshair->Change_RotByCD(0.f);
	}

	m_pUI_Crosshair->Change_Crosshair(iTextureIndex);


	//if ((m_pRayObj != nullptr) &&
	//	(m_pRayObj->Get_ObjType() == ENUM_CLASS(GAMEOBJ_TYPE::ENEMY)) &&
	//	(m_fRayDist < m_fPickupableDist) &&
	//	(m_pPart_Weapon == nullptr) &&
	//	(m_pGameInstance->Get_IsKeyDown(MOUSEKEYSTATE::LB)))
	//	m_pCameraPlayer->Camera_Shake(5.f);

}

void CPlayer::Update_BoneColliders()
{
	Update_BoneCollider(m_vecCollidersCom[ENUM_CLASS(COLLIDERTYPE::SPHERE)][0], "Head");
	Update_BoneCollider(m_vecCollidersCom[ENUM_CLASS(COLLIDERTYPE::SPHERE)][1], "LeftHand");
	Update_BoneCollider(m_vecCollidersCom[ENUM_CLASS(COLLIDERTYPE::SPHERE)][2], "RightHand");

	m_vecCollidersCom[ENUM_CLASS(COLLIDERTYPE::OBB)][0]->Update(m_pTransformCom->Get_WorldMatrix());	// Direct Update

	Update_ToggleColliders();
}

void CPlayer::Update_BoneCollider(CCollider* pCollider, const _char* szBoneName)
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

HRESULT CPlayer::Ready_Colliders(void* pArg)
{
	// ===== Colliders =====

	CCollider* tmpColCom = nullptr;

	CBounding_Sphere::BOUNDING_SPHERE_DESC  SphereDesc{};
	SphereDesc.vCenter = _float3(0.f, 0.f, 0.f);
	COLLISION_DESC colDesc = {};
	
	// s1. For Head / Zero / 0.08f
	// s2. For LeftHand / Zero / 0.08f
	// s3. For RightHand / Zero / 0.08f
	_wstring strNameTag[3] = { L"Head", L"LeftHand", L"RightHand" };
	_float	fRad[3] = { 0.10f, 0.08f, 0.08f };
	for (_uint i = 0; i < 3; i++)
	{
		if		(	strNameTag[i] == L"Head"		)
		{
			colDesc = {					// 콜라이더 충돌 레이어 및 대상 정의
				ENUM_CLASS(COLLISION_LAYER::PLAYER_HIT),
				ENUM_CLASS(COLLISION_LAYER::ENEMY_ATK) | ENUM_CLASS(COLLISION_LAYER::ENEMY_BULLET_ATK),
				true, this
			};
			SphereDesc.tColDesc = colDesc;
		}
		else if (	strNameTag[i] == L"LeftHand" ||
					strNameTag[i] == L"RightHand"	)
		{
			colDesc = {					// 콜라이더 충돌 레이어 및 대상 정의
				/*ENUM_CLASS(COLLISION_LAYER::PLAYER_ATK) | */ENUM_CLASS(COLLISION_LAYER::PLAYER_HIT),
				ENUM_CLASS(COLLISION_LAYER::ENEMY_HIT),
				false, this	// 이게 공격 중에만 켜지게.?
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
	OBBDesc.vExtents = _float3(0.13f, 0.75f, 0.10f);
	OBBDesc.vCenter = _float3(0.f, OBBDesc.vExtents.y, 0.f);
	colDesc = {
		ENUM_CLASS(COLLISION_LAYER::PLAYER_HIT), 
		ENUM_CLASS(COLLISION_LAYER::ENEMY_ATK) | ENUM_CLASS(COLLISION_LAYER::ENEMY_BULLET_ATK),
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

void CPlayer::Update_ToggleColliders()
{
	_bool isFistPlaying = false;

	CModel::MODEL_ANIM_DESC tCurAnimDesc = m_pModelCom->Get_PlayingAnimDesc(PART_UPPER);
	if (tCurAnimDesc.iCurAnimIndex == MELEE_U_FIST_01 ||
		tCurAnimDesc.iCurAnimIndex == MELEE_U_FIST_02 ||
		tCurAnimDesc.iCurAnimIndex == MELEE_U_FIST_03 ||
		tCurAnimDesc.iCurAnimIndex == MELEE_U_FIST_04)
	{
		isFistPlaying = true;
	}

	// 주먹이 나가는 중이라면
	if (isFistPlaying && (m_iState & ENUM_CLASS(PLAYER_STATE::ATK)))
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

CPlayer* CPlayer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPlayer* pInstance = new CPlayer(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Create : CPlayer"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CPlayer::Clone(void* pArg)
{
	CPlayer* pInstance = new CPlayer(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CPlayer"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer::Free()
{
	__super::Free();

	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
}

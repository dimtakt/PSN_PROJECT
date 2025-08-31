#include "Player.h"
#include "GameInstance.h"

#include "Body_Player.h"
#include "Weapon.h"

CPlayer::CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CContainerObject (pDevice, pContext)
{
}

CPlayer::CPlayer(const CPlayer& Prototype)
	: CContainerObject { Prototype }
{
}

HRESULT CPlayer::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPlayer::Initialize(void* pArg)
{
	GAMEOBJECT_DESC         Desc{};
	Desc.fSpeedPerSec = 10.f;
	Desc.fRotationPerSec = XMConvertToRadians(180.0f);


	if (FAILED(__super::Initialize(&Desc)))
		return E_FAIL;

	if (FAILED(this->Ready_Components(pArg)))
		return E_FAIL;

	m_pModelCom->Add_Animation();
	m_pModelCom->Set_Animation(MOVE_U_IDLE, PART_UPPER, true);

	m_pModelCom->Add_Animation();
	m_pModelCom->Set_Animation(MOVE_L_IDLE, PART_LOWER, true);




	//if (FAILED(Ready_PartObjects()))
	//	return E_FAIL;


	m_iMaxHp	= 1;
	m_iHp		= 1;

	return S_OK;
}

void CPlayer::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);


}

void CPlayer::Update(_float fTimeDelta)
{
	// 행동 패턴 등.. 추후 컴포넌트 등을 이용하여 구현
	// 함수 꼭 분리해서 난잡하지 않게 만들기



	// 애니메이션 분기 테스트용
	// 애니메이션 목록 확인 : https://puu.sh/Kzk7z/bad5f5726a.png, Client_Defines.h 에도 있음




	Update_Transform(fTimeDelta);
	Update_AnimationState(fTimeDelta);
	Update_AnimationIndex(fTimeDelta);

	m_pModelCom->Play_Animation_AllLayer(fTimeDelta);
	//m_pModelCom->Play_Animation(fTimeDelta, PART_LOWER);
	//m_pModelCom->Play_Animation(fTimeDelta, PART_UPPER);


	__super::Update(fTimeDelta);
}

void CPlayer::Late_Update(_float fTimeDelta)
{
	m_pTransformCom->Set_State(Engine::STATE::POSITION,
		m_pNavigationCom->Compute_OnCell(m_pTransformCom->Get_State(Engine::STATE::POSITION)));

	if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::NONBLEND, this)))
		return;

	__super::Late_Update(fTimeDelta);
}

HRESULT CPlayer::Render()
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

		m_pShaderCom->Begin(0);

		m_pModelCom->Render(i);
	}


	//CPartObject* pBody = Find_PartObject(TEXT("Part_Body"));
	//if (nullptr == pBody)
	//	return E_FAIL;

	//CWeapon::WEAPON_DESC                 WeaponDesc{};
	//WeaponDesc.pState = &m_iState;
	//WeaponDesc.pSocketMatrix = dynamic_cast<CBody_Player*>(pBody)->Get_BoneMatrix("SWORD");
	//WeaponDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();

	//if (FAILED(__super::Add_PartObject(TEXT("Part_Weapon"), ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Weapon"), &WeaponDesc)))
	//	return E_FAIL;


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

	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_Enemy"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom), nullptr)))
		return E_FAIL;

	CNavigation::NAVIGATION_DESC        NaviDesc{};

	// 레벨이 추가될 때 마다 추가.. 추후 이걸 define쪽에 옮기는 것도 고려..
	switch (iDestLevelIndex)
	{
	case ENUM_CLASS(LEVEL::TEST_EXTRA1):	NaviDesc.iCurrentCellIndex = 25;	break;

	default:								NaviDesc.iCurrentCellIndex = 0;		break;
	}
	

	if (FAILED(CGameObject::Add_Component(iDestLevelIndex, TEXT("Prototype_Component_Navigation"),
		TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom), &NaviDesc)))
		return E_FAIL;

	Set_BufferRef(m_pModelCom);


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

	const LIGHT_DESC* pLightDesc = m_pGameInstance->Get_LightDesc(0);
	if (nullptr == pLightDesc)
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightDir", &pLightDesc->vDirection, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightDiffuse", &pLightDesc->vDiffuse, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightAmbient", &pLightDesc->vAmbient, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightSpecular", &pLightDesc->vSpecular, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition(), sizeof(_float4))))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer::Ready_PartObjects()
{
	CBody_Player::BODY_DESC         BodyDesc{};
	BodyDesc.pState = &m_iState;
	BodyDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();

	if (FAILED(__super::Add_PartObject(TEXT("Part_Body"), ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Body_Player"), &BodyDesc)))
		return E_FAIL;

	return S_OK;
}

void CPlayer::Update_Transform(_float fTimeDelta)
{
	_float fTmpSpeed = 1.5f * fTimeDelta;


	if (m_pGameInstance->Get_IsKeyPressing(DIK_S))
	{
		m_pTransformCom->Go_Backward(fTmpSpeed, m_pNavigationCom);
	}
	if (m_pGameInstance->Get_IsKeyPressing(DIK_A))
	{
		m_pTransformCom->Go_Left(fTmpSpeed, m_pNavigationCom);
	}
	if (m_pGameInstance->Get_IsKeyPressing(DIK_D))
	{
		m_pTransformCom->Go_Right(fTmpSpeed, m_pNavigationCom);
	}
	if (m_pGameInstance->Get_IsKeyPressing(DIK_W))
	{
		m_pTransformCom->Go_Straight(fTmpSpeed, m_pNavigationCom);
	}

	_int iMouseMove;
	if (iMouseMove = m_pGameInstance->Get_DIMouseMove(MOUSEMOVESTATE::X))
		m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * iMouseMove * m_fMouseSensor);
}

void CPlayer::Update_AnimationState(_float fTimeDelta)
{
	// 상태 추가 (켜기) → |=
	// 상태 제거 (끄기) → &= ~
	// 
	// 상태 토글 (반전) → ^=
	// 상태 확인 (켜져 있는지 검사) → &
	

	// 이동 상태 제어
	if (m_pGameInstance->Get_IsKeyPressing(DIK_S) ||
		m_pGameInstance->Get_IsKeyPressing(DIK_A) ||
		m_pGameInstance->Get_IsKeyPressing(DIK_D) ||
		m_pGameInstance->Get_IsKeyPressing(DIK_W))
	{
		m_iState |= ENUM_CLASS(PLAYER_STATE::MOVE);
		m_iState &= ~ENUM_CLASS(PLAYER_STATE::IDLE);
	}
	else
	{
		m_iState &= ~ENUM_CLASS(PLAYER_STATE::MOVE);
		m_iState |= ENUM_CLASS(PLAYER_STATE::IDLE);
	}



	// 공격 상태 제어
	if (m_pGameInstance->Get_IsKeyDown(MOUSEKEYSTATE::LB))
	{
		m_iState |= ENUM_CLASS(PLAYER_STATE::ATK);
	}
	if (m_iState & ENUM_CLASS(PLAYER_STATE::ATK) &&
		m_pModelCom->Get_PlayingAnimDesc(PART_UPPER).isFinished)	// 현재 애니메이션이 종료 될 시 state 회수
	{
		m_iState &= ~ENUM_CLASS(PLAYER_STATE::ATK);
	}



}

void CPlayer::Update_AnimationIndex(_float fTimeDelta)
{

	// ==============================

	// 가만히 있는 상태	
	if (m_iState & ENUM_CLASS(PLAYER_STATE::IDLE))
	{
		m_pModelCom->Set_Animation(MOVE_U_IDLE, PART_UPPER, true);
		m_pModelCom->Set_Animation(MOVE_L_IDLE, PART_LOWER, true);
	}


	// 이동 중에만 동작
	if (m_iState & ENUM_CLASS(PLAYER_STATE::MOVE))
	{
		if (m_pGameInstance->Get_IsKeyPressing(DIK_S))
		{	
			m_pModelCom->Set_Animation(MOVE_U_WALKING_BACK, PART_UPPER, true);
			m_pModelCom->Set_Animation(MOVE_L_WALKING_BACK, PART_LOWER, true);
		}
		if (m_pGameInstance->Get_IsKeyPressing(DIK_A))
		{	
			m_pModelCom->Set_Animation(MOVE_U_LEFT_STRAFE_WALK, PART_UPPER, true);
			m_pModelCom->Set_Animation(MOVE_L_LEFT_STRAFE_WALK, PART_LOWER, true);
		}
		if (m_pGameInstance->Get_IsKeyPressing(DIK_D))
		{	
			m_pModelCom->Set_Animation(MOVE_U_RIGHT_STRAFE_WALK, PART_UPPER, true);
			m_pModelCom->Set_Animation(MOVE_L_RIGHT_STRAFE_WALK, PART_LOWER, true);
		}
		if (m_pGameInstance->Get_IsKeyPressing(DIK_W))
		{	
			m_pModelCom->Set_Animation(MOVE_U_WALKING, PART_UPPER, true);
			m_pModelCom->Set_Animation(MOVE_L_WALKING, PART_LOWER, true);
		}
	}
	
	// 공격 중에만 동작. 누른 후 애니메이션 종료시까지만 유지 (state에서 관리)
	static _bool isFistPlaying = false;
	if (m_iState & ENUM_CLASS(PLAYER_STATE::ATK) &&
		isFistPlaying)
	{
		isFistPlaying = true;
		_uint iRandValue = static_cast<_uint>(m_pGameInstance->Rand(0, 4));

		switch (iRandValue)
		{
		default:
		case 0:		m_pModelCom->Set_Animation(MELEE_U_FIST_01, PART_UPPER, false); break;
		case 1:		m_pModelCom->Set_Animation(MELEE_U_FIST_02, PART_UPPER, false); break;
		case 2:		m_pModelCom->Set_Animation(MELEE_U_FIST_03, PART_UPPER, false); break;
		case 3:		m_pModelCom->Set_Animation(MELEE_U_FIST_04, PART_UPPER, false); break;
		}
	}
	else if (!(m_iState & ENUM_CLASS(PLAYER_STATE::ATK)))
	{
		isFistPlaying = false;
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

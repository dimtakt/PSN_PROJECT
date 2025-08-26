#include "Level_Stage.h"

#include "GameInstance.h"
#include "Camera_Free.h"
#include "Terrain.h"



CLevel_Stage::CLevel_Stage(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel{ pDevice, pContext }
{
}

HRESULT CLevel_Stage::Initialize()
{
	return S_OK;
}

void CLevel_Stage::Update(_float fTimeDelta)
{

}

HRESULT CLevel_Stage::Render()
{
	return S_OK;
}

HRESULT CLevel_Stage::Load_BinaryMap(_wstring* strLoadPath)
{
	return S_OK;
}

void CLevel_Stage::Free()
{
	__super::Free();



}

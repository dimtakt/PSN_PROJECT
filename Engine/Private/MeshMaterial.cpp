#include "MeshMaterial.h"

CMeshMaterial::CMeshMaterial(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: m_pDevice{ pDevice }
	, m_pContext{ pContext }
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CMeshMaterial::Initialize(const _char* pModelFilePath, const aiMaterial* pAIMaterial)
{
	for (size_t i = 1; i < AI_TEXTURE_TYPE_MAX; i++)
	{
		_uint		iNumTextures = pAIMaterial->GetTextureCount(static_cast<aiTextureType>(i));

		for (size_t j = 0; j < iNumTextures; j++)
		{
			ID3D11ShaderResourceView* pSRV = { nullptr };
			/* pModelFilePath : D:\Burger\153\Framework\Client\Bin\Resources\Models\Fiona\Fiona.fbx */

			/* 뽑아서 저장해뒀던 경로 + 파일이름 + 확장자 */
			aiString	strTexturePath;

			if (FAILED(pAIMaterial->GetTexture(static_cast<aiTextureType>(i), j, &strTexturePath)))
				break;

			/*strTexturePath.data*/

			/*_splitpath_s();*/

			//CreateDDSTextureFromFile();	// ksta : 수정해야함. 이게뭔지? 텍스쳐 파일 알아서 불러오는 함수 만들라는건가?
			//CreateWICTextureFromFile();	// ksta : 수정해야함. 이게뭔지? 텍스쳐 파일 알아서 불러오는 함수 만들라는건가?



			m_SRVs[i].push_back(pSRV);
		}
	}



	return S_OK;
}

CMeshMaterial* CMeshMaterial::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _char* pModelFilePath, const aiMaterial* pAIMaterial)
{
	CMeshMaterial* pInstance = new CMeshMaterial(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pModelFilePath, pAIMaterial)))
	{
		MSG_BOX(TEXT("Failed to Created : CMeshMaterial"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CMeshMaterial::Free()
{
	__super::Free();

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);

	for (auto& SRVs : m_SRVs)
	{
		for (auto& pSRV : SRVs)
			Safe_Release(pSRV);
		SRVs.clear();
	}
}
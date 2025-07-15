#include "MeshMaterial.h"
#include "Shader.h"

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
			//ID3D11ShaderResourceView* pSRV = { nullptr };
			/* pModelFilePath : D:\Burger\153\Framework\Client\Bin\Resources\Models\Fiona\Fiona.fbx */

			/* 뽑아서 저장해뒀던 경로 + 파일이름 + 확장자 */
			aiString	strTexturePath;

			if (FAILED(pAIMaterial->GetTexture(static_cast<aiTextureType>(i), j, &strTexturePath)))
				break;

			/*strTexturePath.data*/

			/*_splitpath_s();*/


			// 분할할 경로
			_char			szDrive[MAX_PATH] = {};
			_char			szDir[MAX_PATH] = {};

			_char			szFilename[MAX_PATH] = {};
			_char			szExt[MAX_PATH] = {};

			// 텍스쳐 경로가 담길 변수
			_char			szTextureFilePath[MAX_PATH] = {};

			// 3D 모델 파일의 경로로부터 경로만을 szDrive, szDir 에 분할
			_splitpath_s(pModelFilePath, szDrive, MAX_PATH, szDir, MAX_PATH, nullptr, 0, nullptr, 0);
			// 텍스쳐 파일의 경로로부터 파일명과 확장자만을 szFilename, szExt 에 분할
			_splitpath_s(strTexturePath.data, nullptr, 0, nullptr, 0, szFilename, MAX_PATH, szExt, MAX_PATH);

			// szTextureFilePath 에 4개 문자열 병합
			strcpy_s(szTextureFilePath, szDrive);
			strcat_s(szTextureFilePath, szDir);
			strcat_s(szTextureFilePath, szFilename);
			strcat_s(szTextureFilePath, szExt);

			// 경로 문자열을 _chat 을 _tchat 로 변환하여 szFullPath 에 담음.
			_tchar			szFullPath[MAX_PATH] = {};

			MultiByteToWideChar(CP_ACP, 0, szTextureFilePath, strlen(szTextureFilePath),
				szFullPath, MAX_PATH);

			// 텍스쳐 경로 정보에 기반하여 불러와 컨테이너에 담음
			ID3D11ShaderResourceView* pSRV = { nullptr };

			if (false == strcmp(szExt, ".dds"))
			{
				if (FAILED(CreateDDSTextureFromFile(m_pDevice, szFullPath, nullptr, &pSRV)))
					return E_FAIL;
			}
			else if (false == strcmp(szExt, ".tga"))
				return E_FAIL;

			else
			{
				if (FAILED(CreateWICTextureFromFile(m_pDevice, szFullPath, nullptr, &pSRV)))
					return E_FAIL;
			}

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
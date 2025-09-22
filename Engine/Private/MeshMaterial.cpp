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
			// 1. 텍스쳐 경로 저장. (지저분한 데이터라 파일명, 확장자만 뽑는 정제 필요)
			aiString	strTexturePath;
			if (FAILED(pAIMaterial->GetTexture(static_cast<aiTextureType>(i), j, &strTexturePath)))
				break;

			// 2. 정제용 분할 경로 문자열 선언.
			_char			szDrive[MAX_PATH]			= {};
			_char			szDir[MAX_PATH]				= {};
			_char			szFilename[MAX_PATH]		= {};
			_char			szExt[MAX_PATH]				= {};
			_char			szTextureFilePath[MAX_PATH] = {};	// 최종 경로 변수

			// 3.	3D 모델 파일(인자)로부터는, 저장 경로만을 szDrive, szDir 에 분할
			//		텍스쳐 파일의 경로로부터는, 파일명과 확장자만을 szFilename, szExt 에 분할
			_splitpath_s(pModelFilePath, szDrive, MAX_PATH, szDir, MAX_PATH, nullptr, 0, nullptr, 0);
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

			// 텍스쳐 로드 시도
			if (false == strcmp(szExt, ".dds"))			// dds 전용 로드 함수 사용
			{
				if (FAILED(CreateDDSTextureFromFile(m_pDevice, szFullPath, nullptr, &pSRV)))
					return E_FAIL;
			}
			else if (false == strcmp(szExt, ".tga"))	// tga는 지원X
				return E_FAIL;
			//else if (false == strcmp(szExt, ".fbm"))
			//{
			//	// fbm 넘겨
			//}
			else										// 이외 (png, jpg, bmp등) 포맷은 범용 함수 사용 
			{
				if (FAILED(CreateWICTextureFromFile(m_pDevice, szFullPath, nullptr, &pSRV)))
					return E_FAIL;
			}

			m_SRVs[i].push_back(pSRV);
		}
	}

	return S_OK;
}

HRESULT CMeshMaterial::Initialize_Binary(const _char* pModelFilePath, const MATERIAL_DESC tMatDesc)
{
	// vecTexturePaths에 잘 정리된 텍스처 타입/경로가 들어 있다면, 문제 없을 듯

	for (const auto& textureInfo : tMatDesc.vecTexturePaths)
	{
		aiTextureType		eType		= textureInfo.first;
		const aiString&		strPath		= textureInfo.second;
		
		_char		szDrive[MAX_PATH] = {};
		_char		szDir[MAX_PATH] = {};
		_char		szFilename[MAX_PATH] = {};
		_char		szExt[MAX_PATH] = {};
		_char		szTextureFilePath[MAX_PATH] = {};	// 최종 경로 변수

		// 	3D 모델 파일(인자)로부터는, 저장 경로만을 szDrive, szDir 에 분할
		//	텍스쳐 파일의 경로로부터는, 파일명과 확장자만을 szFilename, szExt 에 분할
		_splitpath_s(pModelFilePath, szDrive, MAX_PATH, szDir, MAX_PATH, nullptr, 0, nullptr, 0);
		_splitpath_s(strPath.data, nullptr, 0, nullptr, 0, szFilename, MAX_PATH, szExt, MAX_PATH);

		// szTextureFilePath 에 4개 문자열 병합
		strcpy_s(szTextureFilePath, szDrive);
		strcat_s(szTextureFilePath, szDir);
		strcat_s(szTextureFilePath, szFilename);
		strcat_s(szTextureFilePath, szExt);

		// 경로 문자열을 _chat 을 _tchat 로 변환하여 szFullPath 에 담음.
		_tchar			szFullPath[MAX_PATH] = {};
		MultiByteToWideChar(CP_ACP, 0, szTextureFilePath, strlen(szTextureFilePath),
			szFullPath, MAX_PATH);
			
		
		ID3D11ShaderResourceView* pSRV = { nullptr };

		// 텍스쳐 로드 시도
		if (false == strcmp(szExt, ".dds"))			// dds 전용 로드 함수 사용
		{
			if (FAILED(CreateDDSTextureFromFile(m_pDevice, szFullPath, nullptr, &pSRV)))
				return E_FAIL;
		}
		else if (false == strcmp(szExt, ".tga"))	// tga는 지원X
			return E_FAIL;
		else										// 이외 (png, jpg, bmp등) 포맷은 범용 함수 사용 
		{
			if (FAILED(CreateWICTextureFromFile(m_pDevice, szFullPath, nullptr, &pSRV)))
				return E_FAIL;
		}

		m_SRVs[eType].push_back(pSRV);
	}

	return S_OK;
}

HRESULT CMeshMaterial::Bind_Resources(CShader* pShader, const _char* pConstantName, aiTextureType eTextureType, _uint iIndex)
{
	if (iIndex >= m_SRVs[eTextureType].size())
		return E_FAIL;

	return pShader->Bind_SRV(pConstantName, m_SRVs[eTextureType][iIndex]);
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

CMeshMaterial* CMeshMaterial::Create_Binary(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _char* pModelFilePath, const MATERIAL_DESC tMatDesc)
{
	CMeshMaterial* pInstance = new CMeshMaterial(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Binary(pModelFilePath, tMatDesc)))
	{
		MSG_BOX(TEXT("Failed to Created : CMeshMaterial with Binary"));
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
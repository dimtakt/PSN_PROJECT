#include "Bone.h"

CBone::CBone()
{

}

HRESULT CBone::Initialize(const aiNode* pAINode)
{
	// 1. Assimp 를 통해 본의 데이터를 받아옴. 이름, 행렬, 부모관계
	// 2. 받아온 열 기준의 행렬 데이터를 DX환경에 맞게 전치 후, 본인 Transform 로컬 변수에 담음
	// 3. 최종 Transform 로컬 변수는 항등행렬로 초기화.

	strcpy_s(m_szName, pAINode->mName.data);
	memcpy(&m_TransformationMatrix, &pAINode->mTransformation, sizeof(_float4x4));

	XMStoreFloat4x4(&m_TransformationMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_TransformationMatrix)));
	XMStoreFloat4x4(&m_CombinedTransformationMatrix, XMMatrixIdentity());

	return S_OK;
}

CBone* CBone::Create(const aiNode* pAINode)
{
	CBone* pInstance = new CBone();

	if (FAILED(pInstance->Initialize(pAINode)))
	{
		MSG_BOX(TEXT("Failed to Created : CBone"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBone::Free()
{
	__super::Free();


}
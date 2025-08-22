#pragma once

#include "Component.h"
#include "Animation.h"

/* 모델이 움직인다 -> 정점이 움직인다 -> 모든 정점에 대한 움직임 정보를 저장하기가 힘들다 */
/* -> 뼈를 움직이게끔 처리해주면 조헥싿. -> 어떤 타이밍에 어떤 상태를 가지고 어떤 뼈가 움직여야하는지에 대한 정보가 필요하다. */
/* 앞에서 이야기한 정보들을 애니메이션이라고 부른다. */

/* 1. 뼈 자체의 생성. */
/* 2. 정점들은 대체 어떤 뼈의 정보를 따라서 갱신되야하는가에 대한 정보가 필요하다. */
/* 3. 애니메이션정보(뼈들의 시간에 따른 상태값들)를 로드한다. */

NS_BEGIN(Engine)

class CBone;
class CMesh;
class CMeshMaterial;

class CAnimation;

class ENGINE_DLL CModel final : public CComponent
{
private:
	CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CModel(const CModel& Prototype);
class CChannel;
	virtual ~CModel() = default;

public:
	_uint Get_NumMeshes() const {
		return m_iNumMeshes;
	}
	vector<class CMesh*> Get_Meshes() const {
		return m_Meshes;
	}
	_uint Get_NumAnim() {
		return m_iNumAnimations;
	}

	_float4x4* Get_BoneMatrix(const _char* pBoneName);
	MODELTYPE Get_Modeltype() { return m_eModelType; }
	_wstring Get_CurAnimName() { return m_Animations[m_iCurrentAnimIndex]->Get_AnimName(); };


public:
	virtual HRESULT Initialize_Prototype(MODELTYPE eModelType, const _char* pModelFilePath, _fmatrix PreTransformMatrix);
	virtual HRESULT Initialize(void* pArg);
	virtual HRESULT Render(_uint iMeshIndex);

public:
	void Set_Animation(_uint iIndex, _bool isLoop = false, _float fTransitionTime = 0.13f);

public:
	HRESULT Bind_Materials(class CShader* pShader, const _char* pConstantName, _uint iMeshIndex, aiTextureType eTextureType, _uint iIndex);
	HRESULT Bind_BoneMatrices(class CShader* pShader, const _char* pConstantName, _uint iMeshIndex);
	_bool Play_Animation(_float fTimeDelta);

public:
	HRESULT Export_ToBinary(_wstring* strSavePath);
	HRESULT Import_FromBinary(const _char* pModelFilePath, _fmatrix PreTransformMatrix);

//public:
//	FILETYPE Get_FileType() { return m_eFileType; };

private:
	/* 파일로부터 읽은 모든 정보를 다 저장해주는 구조체. */
	const aiScene*			m_pAIScene = { nullptr };
	/* raw fbx의 경우 위에 담기나, binary model 의 경우에는 아래에 담기도록. */
	BINARY_MODEL_DESC		m_BinModel = {};

	Assimp::Importer		m_Importer = {};
	MODELTYPE				m_eModelType = {};
	_float4x4				m_PreTransformMatrix = {};

	// m_pAIScene = m_Importer.ReadFile(경로);

private:
	_uint							m_iNumMeshes = {};
	vector<class CMesh*>			m_Meshes;

	_uint							m_iNumMaterials = {};
	vector<class CMeshMaterial*>	m_Materials;

	vector<class CBone*>			m_Bones;

	_uint							m_iNumAnimations = { 0 };
	vector<class CAnimation*>		m_Animations;

	_uint							m_iCurrentAnimIndex = { 0 };
	_bool							m_isLoop = {};
	_bool							m_isFinished = {};



	// 애니메이션 변경 시 이전 과정과 블렌드 처리를 위함
	// 가능하다면 조건을 애니메이션 최초 프레임 시작 시로 해도 좋을 듯
	_bool							m_isAnimChanged = false;
	
	_bool							m_isDoingTransition = false;
	_float							m_fTranslationTime = {};
	_float							m_fAnimElapsedTime = {};



	FILETYPE						m_eFileType = {};

private:
	HRESULT Ready_Meshes();
	HRESULT Ready_Materials(const _char* pModelFilePath);
	HRESULT Ready_Bones(const aiNode* pAINode, _int iParentIndex);
	HRESULT Ready_Animations();

public:
	static CModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, MODELTYPE eModelType, const _char* pModelFilePath, _fmatrix PreTransformMatrix);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;

};

NS_END
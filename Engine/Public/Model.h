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
class CChannel;
class CAnimation;

class ENGINE_DLL CModel final : public CComponent
{
public:
	typedef struct tModelSingleAnimDesc
	{
		// 현재 실행중인(했던) 애니메이션의 index
		_uint	iCurAnimIndex;
		_uint	iPrevAnimIndex = UINT_MAX;

		// 루트하는지, 루프가 아니라면 끝났는지를 저장
		_bool	isLoop;
		_bool	isFinished = false;

		// 애니메이션 블렌딩을 위한 조건 변수들
		_bool	isAnimChanged = false;
		_bool	isDoingTransition = false;

		// 애니메이션 블렌딩 시간관리 변수들
		_float	fTranslationTime = {};	// Set_Animation 에서 0.4f 로 초기화됨
		_float	fAnimElapsedTime = {};	// 직접 수정 불가능
	} MODEL_ANIM_DESC;

private:
	CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CModel(const CModel& Prototype);
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
	vector<_wstring> Get_CurAnimNames() 
	{ 
		vector<_wstring> vecAnimNames = {};
		for (auto& animDesc : m_PlayingAnimDescs)
		{
			vecAnimNames.push_back(m_Animations[animDesc.iCurAnimIndex]->Get_AnimName());
		}

		return vecAnimNames;
	};

public:
	virtual HRESULT Initialize_Prototype(MODELTYPE eModelType, const _char* pModelFilePath, _fmatrix PreTransformMatrix);
	virtual HRESULT Initialize(void* pArg);
	virtual HRESULT Render(_uint iMeshIndex);

public:
	void Add_Animation(MODEL_ANIM_DESC tAnimDesc)	{	m_PlayingAnimDescs.push_back(tAnimDesc);			};
	void Add_Animation()							{ 	m_PlayingAnimDescs.push_back(MODEL_ANIM_DESC{});	};

	_uint Get_NumPlayingAnims()						{	return static_cast<_uint>(m_PlayingAnimDescs.size()); };
	MODEL_ANIM_DESC Get_PlayingAnimDesc(_uint index){	return m_PlayingAnimDescs[index]; };

	void Remove_Animation(_uint iCurAnimDescIndex = UINT_MAX)		
	{	
		if (iCurAnimDescIndex == UINT_MAX)	m_PlayingAnimDescs.pop_back();
		else								m_PlayingAnimDescs.erase(m_PlayingAnimDescs.begin() + iCurAnimDescIndex);
	};
	void Set_Animation(_uint iIndex, _uint iTargetCurAnimIndex = 0, _bool isLoop = false, _float fTransitionTime = 0.4f);

public:
	HRESULT Bind_Materials(class CShader* pShader, const _char* pConstantName, _uint iMeshIndex, aiTextureType eTextureType, _uint iIndex);
	HRESULT Bind_BoneMatrices(class CShader* pShader, const _char* pConstantName, _uint iMeshIndex);
	_bool Play_Animation_AllLayer(_float fTimeDelta);
	_bool Play_Animation(_float fTimeDelta, _uint iTargetCurAnimIndex = 0);

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


	// 다중 애니메이션 재생, 관리를 위해 통합함
	vector<MODEL_ANIM_DESC>			m_PlayingAnimDescs = {};


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
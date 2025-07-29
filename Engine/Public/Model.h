#pragma once

#include "Component.h"

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
class CChannel;

class ENGINE_DLL CModel final : public CComponent
{
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

	_float4x4* Get_BoneMatrix(const _char* pBoneName);



public:
	virtual HRESULT Initialize_Prototype(MODELTYPE eModelType, const _char* pModelFilePath, _fmatrix PreTransformMatrix);
	virtual HRESULT Initialize(void* pArg);
	virtual HRESULT Render(_uint iMeshIndex);

public:
	void Set_Animation(_uint iIndex, _bool isLoop = false);

public:
	HRESULT Bind_Materials(class CShader* pShader, const _char* pConstantName, _uint iMeshIndex, aiTextureType eTextureType, _uint iIndex);
	HRESULT Bind_BoneMatrices(class CShader* pShader, const _char* pConstantName, _uint iMeshIndex);
	_bool Play_Animation(_float fTimeDelta);

public: // output 용
	typedef struct tagAIChannelDesc {		// 채널 저장 데이터
		// 키프레임갯수, 본 인덱스, 키프레임정보 4종
		aiString				szChannelName;

		_uint					iNumPosKeys;
		_uint					iNumRotKeys;
		_uint					iNumScaKeys;

		_uint					iNumKeyFrames;	// channel.cpp 내에서 계산하는 과정 있으니 없어도될듯
		vector<KEYFRAME>		vecKeyFrame;
	}AICHANNEL_DESC;
	typedef struct tagAIAnimationDesc {		// 애니메이션 저장 데이터
		// 애니메이션 갯수, 채널 갯수, 재생시간, 초당Tick, 채널 정보들
		_float					fDuration;
		_float					fTicksPerSecond;
		_int					iNumChannels;

		vector<AICHANNEL_DESC>	vecChannels;
	}AIANIM_DESC;
	typedef struct tagBoneDesc {			// 본 저장 데이터
		// 본이름,트랜스폼행렬, 부모본인덱스
		aiString				szBoneName;
		_float4x4				matTransformation;
		_uint					iNumChildren;
	}BONE_DESC;
	typedef struct tagMeshDesc {

	}MESH_DESC;
	typedef struct tagMaterialDesc {

	}MATERIAL_DESC;
	typedef struct tagBinaryModelDesc {		// 모델 정보 저장 데이터
		// 메쉬이름, 마테리얼인덱스, 버텍스갯수, 버텍스스트라이드, 인덱스갯수, 면갯수
		// 
		// Face갯수, Face데이터
		// 버텍스갯수, 버텍스데이터 
		// 본갯수, 본인덱스갯수, 본인덱스데이터
		MODELTYPE				eAnimtype;
		_float4x4				matPreTransformMatrix;

		_int					iNumBones;
		_int					iNumMeshes;
		_int					iNumMaterials;
		_int					iNumAnimations;
		vector<BONE_DESC>		vecBones;
		vector<MESH_DESC>		vecMeshes;
		vector<MATERIAL_DESC>	vecMaterials;
		vector<AIANIM_DESC>		vecAiAnimations;
	}MODEL_DESC;
	HRESULT Export_ToBinary(_wstring* strSavePath);

private:
	/* 파일로부터 읽은 모든 정보를 다 저장해주는 구조체. */
	const aiScene*			m_pAIScene = { nullptr };
	Assimp::Importer		m_Importer = {};
	MODELTYPE				m_eModelType = {};
	_float4x4				m_PreTransformMatrix = {};

	// m_pAIScene = m_Importer.ReadFile(경로);

private:
	_uint					m_iNumMeshes = {};
	vector<class CMesh*>	m_Meshes;

private:
	/* Diffuse, Ambient, Specular */
	_uint							m_iNumMaterials = {};
	vector<class CMeshMaterial*>	m_Materials;

private:
	vector<class CBone*>			m_Bones;

private:
	_uint							m_iCurrentAnimIndex = { 0 };
	_uint							m_iNumAnimations = { 0 };
	vector<class CAnimation*>		m_Animations;
	_bool							m_isLoop = {};
	_bool							m_isFinished = {};

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
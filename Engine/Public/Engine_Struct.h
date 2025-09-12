#ifndef Engine_Struct_h__
#define Engine_Struct_h__


namespace Engine
{
	typedef struct tagEngineDesc
	{
		HINSTANCE		hInst;
		HWND			hWnd;
		WINMODE			eWinMode;
		unsigned int	iWinSizeX, iWinSizeY;
		unsigned int	iNumLevels;
	}ENGINE_DESC;	

	typedef struct tagLightDesc
	{
		enum TYPE { DIRECTIONAL, POINT, END };

		TYPE		eType;
		XMFLOAT4	vDiffuse;
		XMFLOAT4	vAmbient;
		XMFLOAT4	vSpecular;

		XMFLOAT4	vDirection;
		XMFLOAT4	vPosition;
		float		fRange;
	}LIGHT_DESC;

	typedef struct tagVertexPosition
	{
		XMFLOAT3		vPosition;

		static const unsigned int	iNumElements = { 1 };
		static constexpr D3D11_INPUT_ELEMENT_DESC	Elements[iNumElements] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
	}VTXPOS;

	typedef struct tagVertexPositionTexcoord
	{
		XMFLOAT3		vPosition;		
		XMFLOAT2		vTexcoord;

		static const unsigned int	iNumElements = { 2 };
		static constexpr D3D11_INPUT_ELEMENT_DESC	Elements[iNumElements] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
	}VTXPOSTEX;

	typedef struct tagKeyFrame
	{
		XMFLOAT3	vScale;
		XMFLOAT4	vRotation;
		XMFLOAT3	vTranslation;

		float		fTrackPosition;
	}KEYFRAME;

	typedef struct tagVertexCube
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vTexcoord;

		static const unsigned int	iNumElements = { 2 };
		static constexpr D3D11_INPUT_ELEMENT_DESC	Elements[iNumElements] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
	}VTXCUBE;

	typedef struct tagVertexNormalTexcoord
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vNormal;
		XMFLOAT2		vTexcoord; 

		static const unsigned int	iNumElements = { 3 };
		static constexpr D3D11_INPUT_ELEMENT_DESC	Elements[iNumElements] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
	}VTXNORTEX;

	typedef struct tagVertexMesh
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vNormal;
		XMFLOAT3		vTangent;
		XMFLOAT3		vBinormal;
		XMFLOAT2		vTexcoord;

		static const unsigned int	iNumElements = { 5 };
		static constexpr D3D11_INPUT_ELEMENT_DESC	Elements[iNumElements] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 36, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 48, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
	}VTXMESH;

	typedef struct tagVertexAnimMesh
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vNormal;
		XMFLOAT3		vTangent;
		XMFLOAT3		vBinormal;
		/* 정점에게 적용되야할 뼈들의 인덱스*/
		XMUINT4			vBlendIndex;
		XMFLOAT4		vBlendWeight;
		XMFLOAT2		vTexcoord;

		static const unsigned int	iNumElements = { 7 };
		static constexpr D3D11_INPUT_ELEMENT_DESC	Elements[iNumElements] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 36, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "BLENDINDEX", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0, 48, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "BLENDWEIGHT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 64, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 80, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
	}VTXANIMMESH;






	// output 용

	// 구조?
	// 	
	// 	BINARY_MODEL_DESC
	// 	ㄴMATERIAL_DESC
	// 	ㄴBONE_DESC
	// 	ㄴMESH_DESC
	//	ㄴAIANIM_DESC
	// 	  ㄴAICHANNEL_DESC
	//		ㄴKEYFRAME


#pragma region Output Description [Binary Model]
	typedef struct tagAIChannelDesc {		// 채널 저장 데이터
		// 키프레임갯수, 본 인덱스, 키프레임정보 4종
		aiString				szChannelName;

		unsigned int			iBoneIndex;		// 해당 채널이 적용되는 본 인덱스

		unsigned int			iNumPosKeys;
		unsigned int			iNumRotKeys;
		unsigned int			iNumScaKeys;

		unsigned int			iNumKeyFrames;	// channel.cpp 내에서 계산하는 과정 있으니 없어도될듯
		vector<KEYFRAME>		vecKeyFrame;
	} AICHANNEL_DESC;

	typedef struct tagAIAnimationDesc {		// 애니메이션 저장 데이터
		// 애니메이션 갯수, 채널 갯수, 재생시간, 초당Tick, 채널 정보들
		aiString				szAnimName;

		float					fDuration;
		float					fTicksPerSecond;
		unsigned int			iNumChannels;

		vector<AICHANNEL_DESC>	vecChannels;
	} AIANIM_DESC;

	typedef struct tagBoneDesc {			// 본 저장 데이터
		aiString				szBoneName;
		XMFLOAT4X4				matTransformation;
		unsigned int			iNumChildren;

		unsigned int			iParentBoneIndex;      // 추가
		XMFLOAT4X4				matOffset;			// 스키닝용?
	} BONE_DESC;

	struct MeshFace { unsigned int iIndices[3]; };
	typedef struct tagMeshDesc {
		aiString				szMeshName;

		unsigned int			iMaterialIndex;
		unsigned int			iNumVertices;
		unsigned int			iVertexStride;
		unsigned int			iNumIndices;
		unsigned int			iNumFaces;
		vector<MeshFace>		vecFaces;

		unsigned int			iNumUsingBones;
		vector<unsigned int>	vecUsingBonesIndices;
		
		// size follows "iNumVertices" 
		vector<VTXMESH>			vecNonAnimVertices;
		vector<VTXANIMMESH>		vecAnimVertices;
	} MESH_DESC;

	typedef struct tagMaterialDesc {
		aiString				szMaterialName;

		unsigned int			iMaterialIndex;
		unsigned int			iNumTextures;

		vector<pair<aiTextureType, aiString>>	
								vecTexturePaths; // 실제 텍스처 경로 및 유형의 저장

	} MATERIAL_DESC;

	typedef struct tagBinaryModelDesc {		// 모델 정보 저장 데이터
		// 메쉬이름, 마테리얼인덱스, 버텍스갯수, 버텍스스트라이드, 인덱스갯수, 면갯수
		// 
		// Face갯수, Face데이터
		// 버텍스갯수, 버텍스데이터 
		// 본갯수, 본인덱스갯수, 본인덱스데이터
		aiString				szModelName;
		MODELTYPE				eAnimtype;
		XMFLOAT4X4				matPreTransformMatrix;

		unsigned int			iNumBones;
		unsigned int			iNumMeshes;
		unsigned int			iNumMaterials;
		unsigned int			iNumAnimations;
		vector<BONE_DESC>		vecBones;
		vector<MESH_DESC>		vecMeshes;
		vector<MATERIAL_DESC>	vecMaterials;
		vector<AIANIM_DESC>		vecAiAnimations;
	} BINARY_MODEL_DESC;
#pragma endregion


#pragma region Output Description [Binary Level]
	typedef struct tagLoadedGameObjectsDesc{

		XMFLOAT4X4				matFinalTransform;	// 최종 계산된 오브젝트의 좌표가 담김
		wstring					strFileName;		// 파일 이름
		unsigned int			iObjType;			// 오브젝트 타입 (무기냐, 정적 물체냐 등)

	}LOADED_OBJ_DESC;

	typedef struct tagMapDataDesc {

		unsigned int			iMapLevel;

		unsigned int			iNumLoadedItems;
		vector<wstring>			vecLoadedItems;		// 로드된 요소들의 파일명이 담김. 이는 프로토타입 등에 다 사용. 경로는 고정

		unsigned int			iNumGameObj;
		vector<LOADED_OBJ_DESC>	vecGameObj;			// 불러와진 게임오브젝트들의 정보들이 담길 것.

		unsigned int			iNumTerrains;
		vector<XMFLOAT4X4>		vecTerrainTransform;
		
	}MAPDATA_DESC;
#pragma endregion


#pragma region Output Description [Binary NavMesh]
	typedef struct tagNavTriDesc {

		XMFLOAT3 vTriPoints[3];

	}NAVTRI_DESC;

	typedef struct tagNavigationMeshDesc {

		unsigned int			iNumTris;
		vector<NAVTRI_DESC>		vecTris;

	}NAVMESH_DESC;
#pragma endregion



#pragma region Component : Collision Description
	typedef struct tagCollisionDesc
	{
		unsigned int		iLayerIndex;	// 비트연산자로 콜라이더 대상이 무슨 타입인지 정의
		unsigned int		iMask;			// 비트연산자로 충돌할 레이어 정의
		bool				isActive	= true;

		class CGameObject*	pOwner		= { nullptr };

	} COLLISION_DESC;
#pragma endregion

	


}


#endif // Engine_Struct_h__

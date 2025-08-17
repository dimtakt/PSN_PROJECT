#ifndef Engine_Enum_h__
#define Engine_Enum_h__

namespace Engine
{
	enum class MODELTYPE { NONANIM, ANIM, UNDEFINED, END };

	enum class COLLIDERTYPE { AABB, OBB, SPHERE, END };
	enum class CELLPOINT { A, B, C, END };
	enum class CELLLINE { AB, BC, CA, END };

	enum class D3DTS { VIEW, PROJ, END };
	enum class STATE { RIGHT, UP, LOOK, POSITION };
	enum class PROTOTYPE { GAMEOBJECT, COMPONENT };
	enum class RENDERGROUP { PRIORITY, NONBLEND, BLEND, UI, END };
	
	enum class WINMODE { FULL, WIN, END };
	enum class MOUSEKEYSTATE { LB, RB, MB, END	};
	enum class MOUSEMOVESTATE {	X, Y, Z, END	};

	enum class FILETYPE {
		FBX,			// fbx 원본
		DATMODEL,		// fbx 를 바이너리화 한 data
		DATMAP,			// map data.
		DATNAVMESH,		// map navigation mesh.

		FILETYPE_END
	};

}

#endif // Engine_Enum_h__

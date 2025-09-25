#pragma once

#include "../Default/framework.h"
#include <process.h>

/* 클라이언트에서 사용할 수 있는 공통적인 정의를 모아놓은 파일 */
namespace Client
{
	const unsigned int			g_iWinSizeX = 1280;
	const unsigned int			g_iWinSizeY = 720;

	enum class CROSSHAIR_INDEX
	{
		BASICHAND,
		BASICDOT,
		BASICPUNCH,
		GUN,
		KATANA,
		HS_HOVER,		// hotswitch
		HS_IDLE,
		HS_WAIT,
		END
	};

	enum class SCREENTEXT_INDEX
	{
		NOAMMO,
		MAGEMPTY,
		TIMEMOVES,

		TUTO_LEFTCLICK,



		LVLSTART_10_1,
		LVLSTART_10_2,
		LVLMID_10_1,
		LVLMID_10_2,
		LVLMID_10_3,




		LVLEND_SUPER,
		LVLEND_HOT,


		END
	};



	enum class ENEMY_STATE {
		IDLE		= (1 << 0),
		MOVE		= (1 << 1),
		PICKUP		= (1 << 2),
		DMGD_U		= (1 << 3),
		DMGD_L		= (1 << 4),

		// 타겟 추적하여 접근
		TRACK_WEAPON = (1 << 5),	// 근처의 무기 탐색
		TRACK_PLAYER = (1 << 6),	// 무기가 없을 시 플레이어 탐색

		// 픽업중
		PICKUP_GROUND = (1 << 7),
		PICKUP_MID = (1 << 8),

		// 조건 만족시 공격
		ATK_MELEE = (1 << 9),
		ATK_WEAPON_BLUNT = (1 << 10),
		ATK_WEAPON_GUN = (1 << 11),

		


		ENEMY_STATE_END = (1 << 12)
	};

	enum class PLAYER_STATE {
		IDLE				= (1 << 0),		// 가만히

		MOVE				= (1 << 1),		// 평상시(이동 등)

		PICKUP				= (1 << 2),		// 줍는중

		ATK					= (1 << 3),		// 공격
		
		
		
		
		//ATK_MELEE			= (1 << 3),		// 공격
		//ATK_WEAPON_BLUNT	= (1 << 4),		// 공격
		//ATK_WEAPON_GUN	= (1 << 5),		// 공격

		PLAYER_STATE_END
	};

	enum ANIM_PARTINDEX {
		PART_UPPER = 0,
		PART_LOWER = 1,
		//PART_ETC1,
		//PART_ETC2,
		//PART_ETC3, // 필요에 따라 늘리기..

		PART_END
	};

	enum ENEMY_ANIMINDEX {

		DMGD_L_BIG_STOMACH_HIT		= 0,  // [DMGD][Lower]Big Stomach Hit
		DMGD_L_HEAD_HIT				= 1,  // [DMGD][Lower]Head Hit
		DMGD_U_BIG_STOMACH_HIT		= 2,  // [DMGD][Upper]Big Stomach Hit
		DMGD_U_HEAD_HIT				= 3,  // [DMGD][Upper]Head Hit

		GUN_L_DISARMED				= 4,  // [GUN][Lower]Disarmed
		GUN_L_RIFLE_AIM_IDLE		= 5,  // [GUN][Lower]Rifle Aiming Idle
		GUN_U_DISARMED				= 6,  // [GUN][Upper]Disarmed
		GUN_U_RIFLE_AIM_IDLE		= 7,  // [GUN][Upper]Rifle Aiming Idle

		MELEE_U_BASEBALL_HIT		= 8,  // [MELEE][Upper]baseball-hit
		MELEE_U_FIST_01				= 9,  // [MELEE][Upper]fist-01
		MELEE_U_FIST_02				= 10, // [MELEE][Upper]fist-02
		MELEE_U_FIST_03				= 11, // [MELEE][Upper]fist-03
		MELEE_U_FIST_04				= 12, // [MELEE][Upper]fist-04

		MOVE_L_IDLE					= 13, // [MOVE][Lower]idle
		MOVE_L_JUMP					= 14, // [MOVE][Lower]jump
		MOVE_L_LEFT_STRAFE			= 15, // [MOVE][Lower]left strafe
		MOVE_L_LEFT_STRAFE_WALK		= 16, // [MOVE][Lower]left strafe walking
		MOVE_L_LEFT_TURN			= 17, // [MOVE][Lower]left turn
		MOVE_L_LEFT_TURN_90			= 18, // [MOVE][Lower]left turn 90
		MOVE_L_RIGHT_STRAFE			= 19, // [MOVE][Lower]right strafe
		MOVE_L_RIGHT_STRAFE_WALK	= 20, // [MOVE][Lower]right strafe walking
		MOVE_L_RIGHT_TURN			= 21, // [MOVE][Lower]right turn
		MOVE_L_RIGHT_TURN_90		= 22, // [MOVE][Lower]right turn 90
		MOVE_L_RUNNING				= 23, // [MOVE][Lower]running
		MOVE_L_WALKING				= 24, // [MOVE][Lower]walking
		MOVE_L_WALKING_BACK			= 25, // [MOVE][Lower]walking backwards

		MOVE_U_IDLE					= 26, // [MOVE][Upper]idle
		MOVE_U_JUMP					= 27, // [MOVE][Upper]jump
		MOVE_U_LEFT_STRAFE			= 28, // [MOVE][Upper]left strafe
		MOVE_U_LEFT_STRAFE_WALK		= 29, // [MOVE][Upper]left strafe walking
		MOVE_U_LEFT_TURN			= 30, // [MOVE][Upper]left turn
		MOVE_U_LEFT_TURN_90			= 31, // [MOVE][Upper]left turn 90
		MOVE_U_RIGHT_STRAFE			= 32, // [MOVE][Upper]right strafe
		MOVE_U_RIGHT_STRAFE_WALK	= 33, // [MOVE][Upper]right strafe walking
		MOVE_U_RIGHT_TURN			= 34, // [MOVE][Upper]right turn
		MOVE_U_RIGHT_TURN_90		= 35, // [MOVE][Upper]right turn 90
		MOVE_U_RUNNING				= 36, // [MOVE][Upper]running
		MOVE_U_WALKING				= 37, // [MOVE][Upper]walking
		MOVE_U_WALKING_BACK			= 38, // [MOVE][Upper]walking backwards

		ANIM_END
	};


	enum class LEVEL { 
		STATIC,
		LOADING, 
		LOGO,
		GAMEPLAY,	// 임시

		MENU,		// 메뉴
		EDITOR,		// 에디터
		DIALOG,		// 대화창

		// 이하 스테이지 열거체
		// https://superhot.fandom.com/wiki/Category:SUPERHOT_Levels
		CH01_KICK,		CH02_ALLEY,		CH03_CORRID,	CH04_DROP,
		CH05_SUBWAY,	CH06_JUMP,		CH07_SHOTS,		CH09_FIGHTC,
		CH10_DESPER,	CH11_BREAK,		CH13_TIRALL,	CH14_SERV,
		CH15_HOSPIT,	CH16_DONUT,		CH17_ELEVAT,	CH19_OLDBOY,
		CH20_BALLRO,	CH21_MEETIN,	CH22_HACKER,	CH25_FALL,
		CH26_STAIR,		CH27_OFFICE,	CH28_STATION,	CH29_TRAIN,
		CH30_GATE,		CH31_LOBBY,		CH32_CORE,		CH32_LONGWAY,
		CH34_FREE,		CH98_DOG2,		CH99_DOG1,		CH99_DOG3,

		CHSP_BREAKIN,	CHSP_BRIDGE,	CHSP_HALL,		CHSP_LAB18,
		CHSP_LOBBY,		CHSP_OFFICE,	CHSP_PWRPLANT,	CHSP_STAIRS,
		CHSP_WAREHAUS,

		TEST_EXTRA1,
		TEST_EXTRA2,
		TEST_EXTRA3,

		END
	};

	enum class GAMEOBJ_TYPE {
		PLAYER,			// 플레이어
		ENEMY,			// 적

		// 총들
		WEAPON_RANGED_PISTOL,
		WEAPON_RANGED_KARABIN,
		WEAPON_RANGED_SHOTGUN,

		// 근접 무기들
		WEAPON_MELEE_KATANA,
		WEAPON_MELEE_BAT,
		WEAPON_MELEE_KNIFE,
		WEAPON_MELEE_GOLFCLUB,

		// 기타 투척물들
		WEAPON_PROPS,

		// 총알들
		PLAYERBULLET,
		ENEMYBULLET,

		// 딱히 분류없는 고정 오브젝트들
		STATIC_PROPS,

		
		END
	};

	enum class COLLISION_LAYER {
		NONE		= 0,

		PLAYER_ATK	= (1 << 0),
		ENEMY_ATK	= (1 << 1),

		PLAYER_HIT	= (1 << 2),
		ENEMY_HIT	= (1 << 3),

		PLAYER_BULLET_ATK	= (1 << 4),
		ENEMY_BULLET_ATK	= (1 << 5),


		PICKUPABLE	= (1 << 6),
		THROWN		= (1 << 7),
		

		END			= (1 << 8)
	};


	typedef enum class SOUND_CHANNEL {
		// 1. 배경 효과음
		// 2. 목소리 효과음
		// 
		// 3. 6. 총 발사음 x2
		// 4. 7. 총 효과음 x2
		// 5. 8. 여분 + 
		// 
		// 9. 크로스헤어 효과음
		// 
		// 10. UI 효과음

		SOUND_BGM,
		SOUND_VOICE,
		
		SOUND_PLAYERWEAPON_SHOT,
		SOUND_PLAYERWEAPON_EFF,
		SOUND_PLAYERWEAPON_ETC,

		SOUND_ENEMYWEAPON_SHOT,
		SOUND_ENEMYWEAPON_EFF,
		SOUND_ENEMYWEAPON_ETC,

		SOUND_CROSSHAIR,
		SOUND_MAINUI,

		SOUND_END

	} SOUNDCH;

}

extern HWND g_hWnd;
extern HINSTANCE g_hInst;

//extern _float g_fTimeSpeedMultiplyer;

using namespace Client;
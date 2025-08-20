#pragma once

#include "../Default/framework.h"
#include <process.h>

/* 클라이언트에서 사용할 수 있는 공통적인 정의를 모아놓은 파일 */
namespace Client
{
	const unsigned int			g_iWinSizeX = 1280;
	const unsigned int			g_iWinSizeY = 720;

	enum ENEMY_STATE { 
		IDLE		= (1 << 0),
		
		// Run State
		RUN_F		= (1 << 1),
		RUN_B		= (1 << 2),
		RUN_L		= (1 << 3),
		RUN_R		= (1 << 4),

		// Attack State


		//ATTACK		= (1 << )

	};

	enum ENEMY_ANIMINDEX {
		ANIM_STAND0_L            = 0,  // root|zeta_rig_standing-0-dreptanie-left
		ANIM_STAND0_R            = 1,  // root|zeta_rig_standing-0-dreptanie-right
		ANIM_STAND1_L            = 2,  // root|zeta_rig_standing-1-dreptanie-left
		ANIM_STAND1_R            = 3,  // root|zeta_rig_standing-1-dreptanie-right
		ANIM_STAND2_L            = 4,  // root|zeta_rig_standing-2-dreptanie-left
		ANIM_STAND2_R            = 5,  // root|zeta_rig_standing-2-dreptanie-right
		ANIM_STAND3_L            = 6,  // root|zeta_rig_standing-3-dreptanie-left
		ANIM_STAND3_R            = 7,  // root|zeta_rig_standing-3-dreptanie-right
		ANIM_STAND4_L            = 8,  // root|zeta_rig_standing-4-dreptanie-left
		ANIM_STAND4_R            = 9,  // root|zeta_rig_standing-4-dreptanie-right
		ANIM_STAND5_L            = 10, // root|zeta_rig_standing-5-dreptanie-left
		ANIM_STAND5_R            = 11, // root|zeta_rig_standing-5-dreptanie-right
		ANIM_STAND6_L            = 12, // root|zeta_rig_standing-6-dreptanie-left
		ANIM_STAND6_R            = 13, // root|zeta_rig_standing-6-dreptanie-right

		ANIM_STRAFE_L            = 14, // root|zeta_rig_strafing-left
		ANIM_STRAFE_R            = 15, // root|zeta_rig_strafing-right

		ANIM_WALK_F_DREPTANIE    = 16, // root|zeta_rig_walking-dreptanie-forward
		ANIM_CROUCH_END          = 17, // root|zeta_rig_crouching-end
		ANIM_GUARD_00            = 18, // root|zeta_rig_garda-00
		ANIM_GUARD_01            = 19, // root|zeta_rig_garda-01
		ANIM_KNEEL               = 20, // root|zeta_rig_kneeing

		ANIM_MAIN_PISTOL_SHOT    = 21, // root|zeta_rig_main-pistol-strzal-epsilon
		ANIM_MAIN_SHOTGUN_SHOT   = 22, // root|zeta_rig_main-strzelba-strzal-epsilon

		ANIM_MELEE_BASEBALL_HIT  = 23, // root|zeta_rig_melee-baseball-hit
		ANIM_MELEE_FIST_01       = 24, // root|zeta_rig_melee-fist-01-epsilon
		ANIM_MELEE_FIST_02       = 25, // root|zeta_rig_melee-fist-02-epsilon
		ANIM_MELEE_FIST_03       = 26, // root|zeta_rig_melee-fist-03-epsilon
		ANIM_MELEE_FIST_04       = 27, // root|zeta_rig_melee-fist-04-epsilon
		ANIM_MELEE_MINI_BASEBALL = 28, // root|zeta_rig_melee-minibaseball-hit

		ANIM_RUN_B               = 29, // root|zeta_rig_running-backwards
		ANIM_RUN_BL              = 30, // root|zeta_rig_running-backwards-left
		ANIM_RUN_BL_2            = 31, // root|zeta_rig_running-backwards-left-left
		ANIM_RUN_BR              = 32, // root|zeta_rig_running-backwards-right
		ANIM_RUN_BR_2            = 33, // root|zeta_rig_running-backwards-right-right
		ANIM_RUN_F               = 34, // root|zeta_rig_running-forward
		ANIM_RUN_FL              = 35, // root|zeta_rig_running-forward-left
		ANIM_RUN_FR              = 36, // root|zeta_rig_running-forward-right
		ANIM_RUN_L               = 37, // root|zeta_rig_running-left
		ANIM_RUN_R               = 38, // root|zeta_rig_running-right

		ANIM_STAND_STUN_0        = 39, // root|zeta_rig_standing-stun-0-epsilon
		ANIM_STAND_STUN_1        = 40, // root|zeta_rig_standing-stun-1-epsilon
		ANIM_STAND_STUN_BRUCH_0  = 41, // root|zeta_rig_standing-stun-bruch-0-epsilon

		ANIM_WALK_B              = 42, // root|zeta_rig_walking-backwards
		ANIM_WALK_F              = 43, // root|zeta_rig_walking-forward
		ANIM_WALK_L              = 44, // root|zeta_rig_walking-left
		ANIM_WALK_R              = 45  // root|zeta_rig_walking-right
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

		END
	};

	enum class GAMEOBJ_TYPE {
		PLAYER,			// 플레이어
		ENEMY,			// 적

		// 총들
		WEAPON_RANGED_PISTOL,
		WEAPON_RANGED_RIFLE,
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

}

extern HWND g_hWnd;
extern HINSTANCE g_hInst;

//extern _float g_fTimeSpeedMultiplyer;

using namespace Client;
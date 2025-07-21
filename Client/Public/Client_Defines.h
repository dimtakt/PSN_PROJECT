#pragma once

#include "../Default/framework.h"
#include <process.h>

/* 클라이언트에서 사용할 수 있는 공통적인 정의를 모아놓은 파일 */
namespace Client
{
	const unsigned int			g_iWinSizeX = 1280;
	const unsigned int			g_iWinSizeY = 720;

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
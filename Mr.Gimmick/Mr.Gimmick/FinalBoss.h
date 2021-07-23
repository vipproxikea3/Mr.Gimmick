#pragma once
#include "GameObject.h"
#include "Brick.h"
#include "PlayScene.h"
#include "Game.h"
#include "Gimmick.h"
#include "FinalBossBigBullet.h"
#include "FinalBossSmallBullet.h"
#include "FinalBossDieEffect.h"

#define FINALBOSS_STATE_DIE					0
#define FINALBOSS_STATE_CAPE				1
#define FINALBOSS_STATE_WALK				2
#define FINALBOSS_STATE_CHOP				3
#define FINALBOSS_STATE_TRANSFORM			4
#define FINALBOSS_STATE_JUMP_IN				5
#define FINALBOSS_STATE_JUMP_OUT			6
#define FINALBOSS_STATE_SHOT_BIG_BULLET			7
#define FINALBOSS_STATE_SHOT_SMALL_BULLETS		8
#define FINALBOSS_STATE_STUN					9

#define FINALBOSS_ANI_CAPE_LEFT						0
#define FINALBOSS_ANI_CAPE_STUN_LEFT				1
#define FINALBOSS_ANI_CAPE_RIGHT					2
#define FINALBOSS_ANI_CAPE_STUN_RIGHT				3
#define FINALBOSS_ANI_TRANSFORM_LEFT				4 //600ms
#define FINALBOSS_ANI_TRANSFORM_RIGHT				5
#define FINALBOSS_ANI_WALK_LEFT						6
#define FINALBOSS_ANI_WALK_RIGHT					7
#define FINALBOSS_ANI_IDLE_LEFT						8
#define FINALBOSS_ANI_IDLE_RIGHT					9
#define FINALBOSS_ANI_JUMPOUT_LEFT					10
#define FINALBOSS_ANI_JUMPOUT_RIGHT					11
#define FINALBOSS_ANI_JUMPIN_LEFT					12
#define FINALBOSS_ANI_JUMPIN_RIGHT					13
#define FINALBOSS_ANI_CHOP_LEFT						14
#define FINALBOSS_ANI_CHOP_RIGHT					15
#define FINALBOSS_ANI_SHOT_LEFT						16
#define FINALBOSS_ANI_SHOT_RIGHT					17
#define FINALBOSS_ANI_STUN_LEFT						18
#define FINALBOSS_ANI_STUN_RIGHT					19
#define FINALBOSS_ANI_DIE_LEFT						20
#define FINALBOSS_ANI_DIE_RIGHT						21

#define FINALBOSS_GRAVITY					0.0005f

#define FINALBOSS_BBOX_WIDTH				40
#define FINALBOSS_BBOX_HEIGHT				66

#define FINALBOSS_LIGHSABER_BBOX_WIDTH					16
#define FINALBOSS_LIGHSABER_BBOX_HEIGHT					16
#define FINALBOSS_LIGHSABER_SHOT_BBOX_WIDTH				11
#define FINALBOSS_LIGHSABER_SHOT_BBOX_HEIGHT			20

#define FINALBOSS_HEART_BBOX_WIDTH					10
#define FINALBOSS_HEART_BBOX_HEIGHT					10

#define FINALBOSS_PLAYER_DISTANCE					90 //khoang cach giua walk va player
#define FINALBOSS_PLAYER_CANJUMP_DISTANCE			60 // khoang cach walk nhay
#define FINALBOSS_STAR_DETECT_RANGE						24

#define FINALBOSS_PLAYER_CHOP_DISTANCE				20 // khoang cach tu chop den player

#define FINALBOSS_ACCELERATION				0.00035f
#define FINALBOSS_WALK_SPEED				0.08f //0.1f
#define FINALBOSS_JUMP_SPEED_Y				0.2f
#define FINALBOSS_JUMP_SPEED_X				0.07f

#define FINALBOSS_BLINK_TIME				300		//time nhap nhay
#define FINALBOSS_CAPE_APPEAR_TIME			1500  // thoi gian xuat hien moi lan ban (state CAPE)
#define FINALBOSS_CAPE_STARTSHOOT_TIME			500
#define FINALBOSS_CAPE_HIDE_TIME			1500  // thoi gian tang hinh (state CAPE)
#define FINALBOSS_TRANSFORM_TIME			900
#define FINALBOSS_SHOT_TIME					600 // 600 thoi gian animation shot
#define FINALBOSS_BIGSHOT_COOLDOWN			9000
#define FINALBOSS_SMALLSHOT_COOLDOWN		4500

#define FINALBOSS_DIE_TIME				2000


#define FINALBOSS_CHOP_TIME						500	// 500 thoi gian chop
#define FINALBOSS_STUN_TIME						300

#define FINALBOSS_MAP_END_RIGHT				2045
#define FINALBOSS_MAP_END_LEFT				1800

#define FINALBOSS_CAPE_LEFT_START_X					1810
#define FINALBOSS_CAPE_RIGHT_START_X				1984

#define FINALBOSS_LIFE_COUNT				5
class CFinalBoss : public CGameObject
{
public:
	float ax = 0;
	bool onGround = false;

	int life_counts = 8;

	int alphaRender = 255; 

	bool is_blinking = false;
	ULONGLONG blink_start = 0;

	bool is_stunning = false;
	ULONGLONG stun_start = 0;

	ULONGLONG die_start = 0;

	bool hide = false;
	bool can_shoot = true;
	ULONGLONG cape_appear_start = 0;

	ULONGLONG transform_start = 0;
	ULONGLONG chop_start = 0;
	ULONGLONG shot_start = 0;

	ULONGLONG auto_bigshot_cooldown_start = 0;
	ULONGLONG auto_smallshot_cooldown_start = 0;

	CFinalBoss();
	~CFinalBoss() {}
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	virtual void Render();
	virtual void SetState(int state);
	virtual void GetBoundingBox(float& l, float& t, float& r, float& b);
	void CalculateSpeed();
	void DetectPlayer();
	void DetectStar();
	int CheckSideOfStar();
	bool onTopOf(CGameObject* object, float equal = 1);
	bool onSideOf(CGameObject* object, float equal = 1);
	void SpecialCollisionWithPlayer();
	bool CheckSwordBlockStar();
	bool CheckStarHitHeart();
	void CalculateStun();
	bool isEqual(float a, float b, float equal = 1.0f);
	void LimitPosition();
	void CalculateState();
	void CalculateBlink();
	void StartBlink();
	void ShootBigBullet(int direction);
	void ShootSmallBullet(float direction);
	void RenderBB(float l, float t, float r, float b);
	void RenderHeartBB();
	void CreateDieEffect();
	bool CheckHitPlayer();
};


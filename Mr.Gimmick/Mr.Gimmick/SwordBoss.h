#pragma once
#include "GameObject.h"
#include "Brick.h"
#include "PlayScene.h"
#include "Game.h"
#include "Gimmick.h"

#define SWORDBOSS_GRAVITY			0.0005f

#define SWORDBOSS_STATE_DIE				0
#define SWORDBOSS_STATE_WALK			1
#define SWORDBOSS_STATE_CHOP			2
#define SWORDBOSS_STATE_STUN			3

#define SWORDBOSS_ANI_IDLE_LEFT					0
#define SWORDBOSS_ANI_IDLE_RIGHT				1
#define SWORDBOSS_ANI_WALK_LEFT					2
#define SWORDBOSS_ANI_WALK_RIGHT				3
#define SWORDBOSS_ANI_CHOP_LEFT					4
#define SWORDBOSS_ANI_CHOP_RIGHT				5
#define SWORDBOSS_ANI_STUN_LEFT					6
#define SWORDBOSS_ANI_STUN_RIGHT				7
#define SWORDBOSS_ANI_DIE_LEFT					8
#define SWORDBOSS_ANI_DIE_RIGHT					9

#define SWORDBOSS_BBOX_WIDTH				33
#define SWORDBOSS_BBOX_HEIGHT				33
#define SWORDBOSS_SWORD_BBOX_WIDTH			10

#define SWORDBOSS_PLAYER_DISTANCE					80 //khoang cach giua walk va player
#define SWORDBOSS_PLAYER_CANJUMP_DISTANCE			48 // khoang cach walk nhay
#define SWORDBOSS_PLAYER_CRAZY_CANJUMP_DISTANCE			80 // khoang cach walk nhay khi crazy
#define SWORDBOSS_BLOCK_RANGE						30

#define SWORDBOSS_PLAYER_CHOP_DISTANCE				20 // khoang cach tu chop den player

#define SWORDBOSS_ACCELERATION				0.00035f
#define SWORDBOSS_WALK_SPEED				0.1f
#define SWORDBOSS_JUMP_SPEED				0.26f
#define SWORDBOSS_JUMP_SPEED_X				0.15f

#define SWORDBOSS_DEFLECT_SPEED_X			0.05f
#define SWORDBOSS_DEFLECT_SPEED_Y			0.07f

#define SWORDBOSS_CHOP_TIME						700	//thoi gian chop
#define SWORDBOSS_CHOP_COOLDOWN					1500	// 2000 cool down chop block sao va player den gan
#define SWORDBOSS_TIME_BETWEEN_AUTOCHOPS		3000 //4000: thoi gian tu dong chem (can_chop khong anh huong den tinh nang nay, luon luon tu dong chem)

#define SWORDBOSS_MAP_END_RIGHT				2032
#define SWORDBOSS_MAP_END_LEFT				1800

#define SWORDBOSS_LIFE_COUNT				5

class CSwordBoss : public CGameObject
{
public:
	float ax = 0;
	bool onGround = false;
	bool render_onground = false;

	int life_counts = SWORDBOSS_LIFE_COUNT;

	bool is_stunning = false;
	ULONGLONG stun_start = 0;

	bool can_chop = true;
	ULONGLONG canchop_start = 0;

	bool is_chopping = false;
	ULONGLONG chop_start = 0;
	ULONGLONG autochop_timing = 0;

	bool crazy = false;

	CSwordBoss(); //-1 left, 1 right
	~CSwordBoss() {}
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	virtual void Render();
	virtual void SetState(int state);
	virtual void GetBoundingBox(float& l, float& t, float& r, float& b);
	void Jump();
	void CalculateSpeed();
	void DetectPlayer();
	void DetectStar();
	int CheckSideOfStar();
	bool onTopOf(CGameObject* object, float equal = 1);
	bool onSideOf(CGameObject* object, float equal = 1);
	void SpecialCollisionWithPlayer();
	bool CheckSwordBlockStar();
	bool isEqual(float a, float b,float equal = 1.0f);
	void LimitPosition();
	void CalculateState();
	void CreateSword();
};


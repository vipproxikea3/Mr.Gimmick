#pragma once
#include "GameObject.h"
#include "Brick.h"
#include "PlayScene.h"
#include "Game.h"
#include "Gimmick.h"

#define GREENBOSS_STATE_DIE				0
#define GREENBOSS_STATE_WALK			1
#define GREENBOSS_STATE_JUMP			2

#define GREENBOSS_ANI_WALK_RIGHT				0
#define GREENBOSS_ANI_WALK_LEFT					1
#define GREENBOSS_ANI_JUMP_RIGHT_UP				2
#define GREENBOSS_ANI_JUMP_RIGHT_DOWN			3
#define GREENBOSS_ANI_JUMP_LEFT_UP				4
#define GREENBOSS_ANI_JUMP_LEFT_DOWN			5
#define GREENBOSS_ANI_DIE_LEFT					6
#define GREENBOSS_ANI_DIE_RIGHT					7

#define GREENBOSS_BBOX_WIDTH			24
#define GREENBOSS_BBOX_HEIGHT			25	

#define PLAYER_MAX_RANGE_WALK_ON_GREENBOSS		60

#define GREENBOSS_ACCELERATION					0.0005f
#define GREENBOSS_WALK_SPEED					0.1f
#define GREENBOSS_JUMP_SPEED					0.24f //0.23
#define GREENBOSS_GRAVITY						0.0006f
#define GREENBOSS_DEFLECT_SPEED_X				0.09f
#define GREENBOSS_DEFLECT_SPEED_Y				0.12f

#define GREENBOSS_STAR_DODGE_RANGE				55

#define GREENBOSS_STUN_TIME						1000

#define GREENBOSS_LIFE_COUNTS						3

class CGreenBoss : public CGameObject
{
public:
	bool canTurnAround = false;
	float ax = 0;
	bool onGround = false;
	bool canDodge = true;

	int alphaRender = 255; // de render khi trung dan

	int life_counts = GREENBOSS_LIFE_COUNTS;

	bool is_stunning = false;
	ULONGLONG stun_start = 0;

	CGreenBoss(); //-1 left, 1 right
	~CGreenBoss() {}
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	virtual void Render();
	virtual void SetState(int state);
	virtual void GetBoundingBox(float& l, float& t, float& r, float& b);
	void Jump();
	void CalculateSpeed();
	void CalculateStun();
	void TurnAroundSlowly();
	void DetectPlayer();
	void DetectStar();
	int CheckSideOfStar();
	bool onTopOf(CGameObject* object, float equal = 1);
	bool onSideOf(CGameObject* object, float equal = 1); 
	void SpecialCollisionWithPlayer();
	bool IsCollidingWithPlayer();
};


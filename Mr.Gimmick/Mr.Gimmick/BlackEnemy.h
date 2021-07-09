#pragma once
#include "GameObject.h"
#include "Brick.h"
#include "BrickPink.h"
#include "InclinedBrick.h"
#include "Conveyor.h"
#include "Swing.h"
#include "PlayScene.h"
#include "Game.h"
#include "Gimmick.h"

#define BLACKENEMY_STATE_DIE	0
#define BLACKENEMY_STATE_WALK	1
#define BLACKENEMY_STATE_TRANSFORM	2
#define BLACKENEMY_STATE_FLY		3

#define BLACKENEMY_WIDTH		16
#define BLACKENEMY_HEIGHT		16

#define BLACKENEMY_ACCELERATION		0.0002f  //0.0002f
#define BLACKENEMY_WALK_SPEED		0.075f	//0.075f
#define BLACKENEMY_GRAVITY			0.0008f //0.001f giam trong luc xuong 1 chut de khi roi khong bi xuyen qua gach
#define BLACKENEMY_JUMP_SPEED		0.23f

#define BLACKENEMY_FLY_ACCELERATION	0.0004f //0.0004f
#define BLACKENEMY_FLY_SPEED		0.165f  //0.165f
#define BLACKENEMY_FLY_SPEED_Y		0.05f

#define TURN_COUNT_TO_TRANSFORM		5 //5
#define JUMP_COUNT_TO_TRANSFORM		10 // neu can 1 lan nhay thi de = 2, vi nhay 1 lan cong 2

#define PLAYER_MAX_RANGE_WALK			48 // 60 : khoang cach 2 ben so voi gimmick de quay dau 48 la chuan
#define PLAYER_MAX_RANGE_FLY			10 // khoang cach 2 ben so voi gimmick de quay dau

#define BLACKENEMY_TRANSFORM_TIME		1500

#define BLACKENEMY_ANI_WALK_RIGHT				0
#define BLACKENEMY_ANI_WALK_LEFT				1
#define BLACKENEMY_ANI_FLY_RIGHT				2
#define BLACKENEMY_ANI_FLY_LEFT					3
#define BLACKENEMY_ANI_FLY_ONGROUND_RIGHT		4
#define BLACKENEMY_ANI_FLY_ONGROUND_LEFT		5


class CBlackEnemy : public CGameObject
{
public:
	bool onGround = true;
	bool canTurnAround = false;
	int turn_count = 0;
	int jump_count = 0;
	float ax = 0;

	DWORD transform_start = 0;
	bool transforming = false;

	bool carry_player = false;
	bool detectedStraighRoad = false;
	bool facingBrick = false; //neu va cham theo chieu ngang voi brick thi true
	bool onFastConveyor = false;
	bool onSlowConveyor = false;

	CBlackEnemy(int direction); //-1 left, 1 right
	~CBlackEnemy() {}
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	virtual void Render();
	virtual void SetState(int state);
	virtual void GetBoundingBox(float& l, float& t, float& r, float& b);
	void Jump();
	bool onTopOf(CGameObject* object);
	bool isUnder(CGameObject* object, float equal = 1);
	void CalculateSpeed();
	void TurnAroundSlowly();
	void TurnAroundImmediately();
	void DetectPlayer();
	void Transform();
	bool onSideOf(CGameObject* object, float equal = 1);
	bool InJumpablePosition();
};


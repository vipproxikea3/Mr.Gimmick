#pragma once
#include "GameObject.h"
#include "Brick.h"
#include "PlayScene.h"
#include "Game.h"
#include "Gimmick.h"

#define	ELECTRIC_BLACKENEMY_BBOX_WIDTH	16
#define	ELECTRIC_BLACKENEMY_BBOX_HEIGHT	22

#define	ELECTRIC_BLACKENEMY_STOP_BBOX_WIDTH		16
#define	ELECTRIC_BLACKENEMY_STOP_BBOX_HEIGHT	15

#define ELECTRIC_BLACKENEMY_STATE_DIE		0
#define	ELECTRIC_BLACKENEMY_STATE_FALL		1
#define	ELECTRIC_BLACKENEMY_STATE_WALK		2
#define	ELECTRIC_BLACKENEMY_STATE_SHOCK		3
#define	ELECTRIC_BLACKENEMY_STATE_STOP		4


#define	ELECTRIC_BLACKENEMY_ANI_WALKING_RIGHT	0
#define	ELECTRIC_BLACKENEMY_ANI_SHOCKING_RIGHT	1
#define	ELECTRIC_BLACKENEMY_ANI_STOP_RIGHT		2
#define	ELECTRIC_BLACKENEMY_ANI_WALKING_LEFT	3
#define	ELECTRIC_BLACKENEMY_ANI_SHOCKING_LEFT	4
#define	ELECTRIC_BLACKENEMY_ANI_STOP_LEFT		5
#define	ELECTRIC_BLACKENEMY_ANI_THUNDER_1_RIGHT	6
#define	ELECTRIC_BLACKENEMY_ANI_THUNDER_1_LEFT	7
#define	ELECTRIC_BLACKENEMY_ANI_THUNDER_2_RIGHT	8
#define	ELECTRIC_BLACKENEMY_ANI_THUNDER_2_LEFT	9
#define ELECTRIC_BLACKENEMY_ANI_DIE_RIGHT		10
#define ELECTRIC_BLACKENEMY_ANI_DIE_LEFT		11


#define ELECTRIC_BLACKENEMY_ACCELERATION		0.0002f
#define ELECTRIC_BLACKENEMY_WALK_SPEED			0.075f	
#define ELECTRIC_BLACKENEMY_GRAVITY				0.0008f

#define	DISTANCE_X		40
#define	DISTANCE_Y		70

#define ELECTRIC_BLACKENEMY_WALK_SPEED		0.075f

#define PLAYER_MAX_RANGE_FLY	10
#define PLAYER_MAX_RANGE_WALK	60

#define TIME_SHOCKING				200
#define	TIME_BETWEEN_TWO_SHOCKING	1200

#define ELECTRIC_BLACKENEMY_DEFLECT_SPEED_X		0.01f	
#define ELECTRIC_BLACKENEMY_DEFLECT_SPEED_Y		0.01f

class CElectricBlackEnemy : public CGameObject
{

public:
	float ax = 0;
	bool appear;
	bool canTurnAround = false;
	bool carry_player = false;
	bool onGround = false;

	bool shocking = false;
	bool shocking_large = false;
	bool touchGround = false;
	DWORD start_shock = 0;

	CElectricBlackEnemy(); //-1 left, 1 right
	~CElectricBlackEnemy() {}
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	virtual void Render();
	virtual void SetState(int state);
	virtual void GetBoundingBox(float& l, float& t, float& r, float& b);
	void StartShock() { start_shock = GetTickCount64(); }
	void CalculateSpeed();
	void TurnAroundSlowly();
	void DetectPlayer();
	void SpecialCollisionWithPlayer();
	bool IsCollidingWithPlayer();
	void DetectStar();
	int CheckSideOfStar();
};
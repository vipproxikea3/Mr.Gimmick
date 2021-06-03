#pragma once
#include "GameObject.h"
#include "Brick.h"
#include "BrickPink.h"
#include "InclinedBrick.h"
#include "Conveyor.h"
#include "Swing.h"

#define GIMMICK_WALKING_SPEED		0.1f
#define GIMMICK_ACCELERATION		0.0002f
#define GIMMICK_WALKING_FRICTION	0.00025f
#define GIMMICK_JUMP_SPEED_Y_MIN	0.15f
#define GIMMICK_JUMP_SPEED_Y_MAX	0.35f	//0.25f
#define GIMMICK_JUMP_ACCELERATION	0.001f	//0.001f
#define GIMMICK_GRAVITY				0.001f
#define GIMMICK_DEFLECT_SPEED_X		0.1f

#define GIMMICK_STATE_IDLE			0
#define GIMMICK_STATE_WALKING_RIGHT	100
#define GIMMICK_STATE_WALKING_LEFT	200
#define GIMMICK_STATE_JUMP			300
#define GIMMICK_STATE_STUN			400
#define GIMMICK_STATE_DIE			500

#define GIMMICK_ANI_IDLE_RIGHT		0
#define GIMMICK_ANI_IDLE_LEFT		1
#define GIMMICK_ANI_WALKING_RIGHT	2
#define GIMMICK_ANI_WALKING_LEFT	3
#define GIMMICK_ANI_JUMP_RIGHT		4
#define GIMMICK_ANI_JUMP_LEFT		5
#define GIMMICK_ANI_STUN_RIGHT		6
#define GIMMICK_ANI_STUN_LEFT		7
#define GIMMICK_ANI_DIE				8

#define GIMMICK_BBOX_WIDTH			16
#define GIMMICK_BBOX_HEIGHT			16

#define GIMMICK_UNTOUCHABLE_TIME	5000
#define GIMMICK_STUNNING_TIME		500


class CGimmick : public CGameObject
{
	int untouchable;
	DWORD untouchable_start;

	DWORD stunning_start;
	float ax = NULL;

	
	bool updated = false;
public:
	float ay = 0;
	bool onGround = false;
	bool falling = false;
	bool stunning;

	void CreateDieEffect();

	CGimmick();
	void CalculateSpeed(DWORD dt);
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* colliable_objects = NULL);
	virtual void Render();

	void SetState(int state);
	void StartUntouchable() { untouchable = 1; untouchable_start = DWORD(GetTickCount64()); }
	void SetdifY(float y) { this->y += y; };

	virtual void GetBoundingBox(float& left, float& top, float& right, float& bottom);
	bool onTopOf(CGameObject* object);
};
#pragma once
#include "GameObject.h"
#include "Brick.h"
#include "BrickPink.h"
#include "InclinedBrick.h"
#include "Conveyor.h"
#include "Swing.h"

#define GIMMICK_WALKING_SPEED_BASE	0.075f 
#define GIMMICK_WALKING_SPEED		0.1f
#define GIMMICK_ACCELERATION		0.0002f
#define GIMMICK_WALKING_FRICTION	0.00025f
#define GIMMICK_JUMP_SPEED_Y		0.3f
#define GIMMICK_GRAVITY				0.001f
#define GIMMICK_DIE_DEFLECT_SPEED	 0.5f

#define GIMMICK_STATE_IDLE			0
#define GIMMICK_STATE_WALKING_RIGHT	100
#define GIMMICK_STATE_WALKING_LEFT	200
#define GIMMICK_STATE_JUMP			300
#define GIMMICK_STATE_DIE			400

#define GIMMICK_ANI_IDLE_RIGHT		0
#define GIMMICK_ANI_IDLE_LEFT		1
#define GIMMICK_ANI_WALKING_RIGHT	2
#define GIMMICK_ANI_WALKING_LEFT	3
#define GIMMICK_ANI_JUMP_RIGHT		4
#define GIMMICK_ANI_JUMP_LEFT		5

#define GIMMICK_ANI_DIE				6

#define GIMMICK_BBOX_WIDTH  16
#define GIMMICK_BBOX_HEIGHT 16

#define GIMMICK_UNTOUCHABLE_TIME 5000


class CGimmick : public CGameObject
{
	int untouchable;
	DWORD untouchable_start;

	float ax = NULL;

	bool onGround = false;
	
public:
	bool onInclinedBrick = false;

	void CreateDieEffect();

	CGimmick();
	void CalculateSpeed(DWORD dt);
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* colliable_objects = NULL);
	virtual void Render();

	void SetState(int state);
	void StartUntouchable() { untouchable = 1; untouchable_start = DWORD(GetTickCount64()); }
	void SetdifY(float y) { this->y += y; };

	virtual void GetBoundingBox(float& left, float& top, float& right, float& bottom);
};
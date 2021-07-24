#pragma once
#include "GameObject.h"

#define BIRD_STATE_NODNOD	0
#define BIRD_STATE_IDLE	1
#define BIRD_STATE_JUMP	2
#define BIRD_STATE_DIE	3
#define BIRD_STATE_WATER_DIE	4
#define BIRD_STATE_FLY	5
#define BIRD_STATE_HIDE	6

#define BIRD_ANI_IDLE_RIGHT	0
#define BIRD_ANI_IDLE_LEFT	1
#define BIRD_ANI_NODNOD_LEFT	2
#define BIRD_ANI_NODNOD_RIGHT	3
#define BIRD_ANI_FLY_RIGHT	4
#define BIRD_ANI_FLY_LEFT	5
#define BIRD_ANI_DIE_LEFT	6
#define BIRD_ANI_DIE_RIGHT	7
#define BIRD_ANI_WATER_DIE	8

#define BIRD_BBOX_WIDTH	10
#define BIRD_BBOX_HEIGHT	10

#define BIRD_VX_JUMP	0.021f
#define BIRD_VY_JUMP	0.077f
#define	BIRD_VX_FLY		0.06f
#define	BIRD_VY_FLY		0.2f
#define BIRD_GRAVITY	0.0006f

#define BIRD_TIME_SEE_JUMP	1850
#define BIRD_TIME_NODNOD	900
#define BIRD_TIME_JUMP		500
#define BIRD_TIME_DIE		3000
#define BIRD_TIME_WATER_DIE	300

#define BIRD_LIMIT_FLY_X	24
#define BIRD_LIMIT_FLY_Y	22

class CBird : public CGameObject
{
public:
	float ay = 0;
	int type; // vat pham cua chim
	int jump = 0; // so lan nhay
	int laststate;

	DWORD nodnod_start = 0;
	DWORD see_jump_start = 0;
	DWORD jump_start = 0;
	DWORD die_start = 0;
	DWORD die_water_start = 0;

	CBird(int type) : CGameObject() {
		this->score = 30;
		this->type = type;
		SetState(BIRD_STATE_IDLE);
	}
	~CBird() {}
	int Collision(LPGAMEOBJECT object, float dy);
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	virtual void Render();
	void StartNodNod() { nodnod_start = GetTickCount64(); }
	void StartSeeJump() { see_jump_start = GetTickCount64(); }
	void StartJump() { jump_start = GetTickCount64(); }
	void StartDie() { die_start = GetTickCount64(); }
	void StartDieWater() { die_water_start = GetTickCount64(); }
	void SetState(int state);
	virtual void GetBoundingBox(float& l, float& t, float& r, float& b);
	void CreateReward();
	/*float GetX() { return this->x; };
	float GetY() { return this->y; };*/
};




#pragma once
#include "GameObject.h"

#define CAT_STATE_IDLE	0
#define CAT_STATE_SLEEP	1
#define CAT_STATE_RUN	2
#define CAT_STATE_JUMP	3
#define CAT_STATE_WATER_DIE	4
#define CAT_STATE_HIDE	5
#define CAT_STATE_CHASE	6

#define CAT_ANI_RUN_RIGHT	0
#define CAT_ANI_IDLE_RIGHT	1
#define CAT_ANI_SLEEP_RIGHT	2
#define CAT_ANI_JUMP_RIGHT	3
#define CAT_ANI_RUN_LEFT	4
#define CAT_ANI_IDLE_LEFT	5
#define CAT_ANI_SLEEP_LEFT	6
#define CAT_ANI_JUMP_LEFT	7
#define CAT_ANI_WATER_DIE	8

#define CAT_BBOX_WIDTH	24
#define CAT_BBOX_HEIGHT	19

#define CAT_GRAVITY	0.001f
#define CAT_RUN_SPEED	0.13f
#define CAT_RUN_SLIDE	0.005f

#define CAT_TIME_SLEEP	120000
#define CAT_TIME_RUN	2600
#define CAT_TIME_HIDE	300

#define CAT_LIMIT_X	160
#define CAT_LIMIT_Y	30
#define CAT_LIMIT_RUN_X	5

class CCat : public CGameObject
{
public:
	float ay = 0;
	int type; // vat pham cua meo
	float lastvx = 0;
	int stop_star = 0; // 1 co the gap sao, 0 ko the gap sao 

	DWORD sleep_start = 0;
	DWORD run_start = 0;
	DWORD water_die = 0;

	CCat(int type,int nx,int state) : CGameObject() {
		this->score = 10;
		this->type = type;
		this->nx = nx;
		SetState(state);
	}
	~CCat() {}
	int Collision(LPGAMEOBJECT object, float dy);
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	virtual void Render();
	void SetState(int state);
	virtual void GetBoundingBox(float& l, float& t, float& r, float& b);
	void StartSleep() { sleep_start = GetTickCount64(); }
	void StartRun() { run_start = GetTickCount64(); }
	void StartWaterDie() { water_die = GetTickCount64(); }
	//void CreateReward();
	/*float GetX() { return this->x; };
	float GetY() { return this->y; };*/
};






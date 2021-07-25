#pragma once
#include "GameObject.h"

#define	TURLE_GRAVITY				0.0005f
#define TURLE_WALKING_SPEED			0.01f
#define TURLE_DIE_SPEED				0.075f

#define	TURLE_WALKING_BBOX_WIDTH		20
#define	TURLE_WALKING_BBOX_HEIGHT		16
#define	TURLE_TURNING_BBOX_WIDTH		18
#define	TURLE_TURNING_BBOX_HEIGHT		16
#define	TURLE_DIE_BBOX_WIDTH			20
#define	TURLE_DIE_BBOX_HEIGHT			17

#define	TURLE_STATE_WALKING_RIGHT	100
#define	TURLE_STATE_TURNING_RIGHT	200
#define	TURLE_STATE_DIE_RIGHT		300
#define	TURLE_STATE_WALKING_LEFT	400
#define	TURLE_STATE_TURNING_LEFT	500
#define	TURLE_STATE_DIE_LEFT		600
#define	TURLE_STATE_DIE_COMPLETE_RIGHT	700
#define	TURLE_STATE_DIE_COMPLETE_LEFT	800
#define	TURLE_STATE_DISAPPEAR			900

#define TURLE_ANI_WALKING_RIGHT		0
#define TURLE_ANI_TURNING_RIGHT		1
#define TURLE_ANI_DIE_RIGHT			2
#define TURLE_ANI_WALKING_LEFT		3
#define TURLE_ANI_TURNING_LEFT		4
#define TURLE_ANI_DIE_LEFT			5

#define TURLE_UNTOUCHABLE_TIME		1500

class CTurle : public CGameObject
{
	int untouchable;
	float left = NULL;
	float right = NULL;
	bool canBeAttacked = true;
	DWORD count_down = 0;

public:
	CTurle(float left, float right);
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* colliable_objects = NULL);
	virtual void Render();
	void SetState(int state);
	virtual void GetBoundingBox(float& l, float& t, float& r, float& b);
	void StartCountDown() { count_down = GetTickCount64(); }
	bool onTopOf(CGameObject* object, float equal = 1.0f);
};


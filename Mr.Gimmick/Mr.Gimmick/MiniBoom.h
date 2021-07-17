#pragma once
#include "GameObject.h"

#define	MINIBOOM_GRAVITY				0.001f
#define MINIBOOM_WALKING_SPEED			0.1f

#define	MINIBOOM_BBOX_WIDTH				16
#define	MINIBOOM_BBOX_HEIGHT			16

#define	MINIBOOM_STATE_WALKING_RIGHT	100
#define	MINIBOOM_STATE_WALKING_LEFT		200
#define	MINIBOOM_STATE_EXPLOSIVE		300
#define	MINIBOOM_STATE_DIE				400

#define MINIBOOM_ANI_WALKING			0
#define MINIBOOM_ANI_EXPLOSIVE			1

#define MINIBOOM_ALIVE_TIME				5000
#define MINIBOOM_EXPLOSIVE_TIME			250


class CMiniBoom : public CGameObject
{
	int alive = 0;
	int explosive = 0;

	DWORD alive_start;
	DWORD explosive_start;
public:
	CMiniBoom();
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* colliable_objects = NULL);
	virtual void Render();
	void SetState(int state);
	virtual void GetBoundingBox(float& l, float& t, float& r, float& b);
};


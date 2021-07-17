#pragma once
#include "GameObject.h"
#include "Gimmick.h"

#define	BOOM_BOAT_BBOX_WIDTH	16
#define	BOOM_BOAT_BBOX_HEIGHT	16

#define	BOOM_BOAT_STATE_ALIVE		100
#define	BOOM_BOAT_STATE_FALLING		200
#define	BOOM_BOAT_STATE_EXPLODE		300
#define	BOOM_BOAT_STATE_DIE		400

#define BOOM_BOAT_ANI_ALIVE		0
#define	BOOM_BOAT_ANI_EXPLODE	1

#define	BOOM_BOAT_GRAVITY		0.00001f
#define	BOOM_BOAT_BUM_GRAVITY	0.1f

#define	BOOM_BOAT_EXPLODE_TIME	300

class CBomboat : public CGameObject
{
	float ay = 0;
	DWORD explode_start = 0;

public:
	CBomboat();
	~CBomboat() {};
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* colliable_objects = NULL);
	virtual void Render();

	void SetState(int state);
	void StartExplode() { explode_start = GetTickCount64(); }

	virtual void GetBoundingBox(float& l, float& t, float& r, float& b);
};



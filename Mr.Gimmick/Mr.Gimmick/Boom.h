#pragma once
#include "GameObject.h"
#include "Gimmick.h"

#define	BOOM_BBOX_WIDTH		14
#define	BOOM_BBOX_HEIGHT	18

#define	BOOM_BBOX_EXPLODE_WIDTH		16
#define	BOOM_BBOX_EXPLODE_HEIGHT	16


#define	BOOM_STATE_ALIVE		100
#define	BOOM_STATE_DIE			200
#define	BOOM_STATE_FALLING		300
#define	BOOM_STATE_EXPLODE		400

#define BOOM_ANI_ALIVE		0
#define BOOM_ANI_DEAD		1
#define	BOOM_ANI_EXPLODE	2

#define	BOOM_GRAVITY		0.15f

#define	BOOM_EXPLODE_TIME	300

class CBoom : public CGameObject
{
	DWORD explode_start = 0;

public:
	CBoom();
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* colliable_objects = NULL);
	virtual void Render();

	void SetState(int state);
	void StartExplode() { explode_start = GetTickCount64(); }

	virtual void GetBoundingBox(float& l, float& t, float& r, float& b);
};


#pragma once
#include "GameObject.h"

#define BOAT_ANI	0
#define BOAT_FINISH	1695.5f
#define BOAT_VX	0.045f
#define BOAT_BBOX_WIDTH	48
#define BOAT_BBOX_HEIGHT	16

class CBoat : public CGameObject
{
public:
	bool run = false;
	float gimx = x;
	CBoat() :CGameObject() {};
	~CBoat() {};
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	virtual void Render();
	void SetState(int state);
	virtual void GetBoundingBox(float& l, float& t, float& r, float& b);
	float GetVx() { return this->vx; }
	float GetFinish() { return BOAT_FINISH; }
};

#pragma once
#include "GameObject.h"



#define PORTAL_BBOX_WIDTH	16
#define PORTAL_BBOX_HEIGHT	16

class CPortal : public CGameObject
{
	float target_x = NULL;
	float target_y = NULL;
public:
	CPortal(float target_x, float target_y) : CGameObject() {
		this->target_x = target_x;
		this->target_y = target_y;
	}
	~CPortal() {}
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	virtual void Render();
	void SetState(int state);
	virtual void GetBoundingBox(float& l, float& t, float& r, float& b);
};




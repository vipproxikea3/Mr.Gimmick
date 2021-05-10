#pragma once
#include "GameObject.h"

#define BRICK_BBOX_WIDTH	16
#define BRICK_BBOX_HEIGHT	16

class CBrick : public CGameObject
{
public:
	CBrick() : CGameObject() {}
	~CBrick() {}
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	virtual void Render();
	void SetState(int state);
	virtual void GetBoundingBox(float& l, float& t, float& r, float& b);
};


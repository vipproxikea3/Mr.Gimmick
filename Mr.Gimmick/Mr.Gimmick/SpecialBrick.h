#pragma once
#include "GameObject.h"

#define SPECIALBRICK_BBOX_WIDTH		16
#define SPECIALBRICK_BBOX_HEIGHT	16

class CSpecialBrick : public CGameObject
{
	float w = 16;
	float h = 16;
public:
	int type = 0; // 0. Gimmick || 1. Cat
	CSpecialBrick(float w, float h, int type) : CGameObject() {
		this->w = w;
		this->h = h;
		this->type = type;
	}
	~CSpecialBrick() {}
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	virtual void Render();
	void SetState(int state);
	virtual void GetBoundingBox(float& l, float& t, float& r, float& b);
};


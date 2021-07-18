#pragma once
#include "GameObject.h"

class CWaterDie : public CGameObject
{
	float w = 16;
	float h = 16;
public:
	CWaterDie(float w, float h) : CGameObject() {
		this->w = w;
		this->h = h;
	}
	~CWaterDie() {}
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	virtual void Render();
	void SetState(int state);
	virtual void GetBoundingBox(float& l, float& t, float& r, float& b);
};

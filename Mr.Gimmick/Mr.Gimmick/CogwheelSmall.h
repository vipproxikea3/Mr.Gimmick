#pragma once
#include "GameObject.h"
#include "Utils.h"

#define COGWHEELSMALL_ANI_SPIN	0

#define COGWHEELSMALL_BBOX_WIDTH	16
#define COGWHEELSMALL_BBOX_HEIGHT	16

class CCogwheelSmall : public CGameObject
{
public:
	CCogwheelSmall() : CGameObject()
	{
	}
	~CCogwheelSmall() {}
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	virtual void Render();
	void SetState(int state);
	virtual void GetBoundingBox(float& l, float& t, float& r, float& b);
};


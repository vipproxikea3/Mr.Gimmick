#pragma once
#include "GameObject.h"
#include "Utils.h"

#define COGWHEEL_ANI_TOP_RIGHT	0
#define COGWHEEL_ANI_BOT_RIGHT	1
#define COGWHEEL_ANI_BOT_LEFT	2
#define COGWHEEL_ANI_TOP_LEFT	3

#define COGWHEEL_BBOX_WIDTH	16
#define COGWHEEL_BBOX_HEIGHT	16

class CCogwheel : public CGameObject
{
	int type;
public:
	CCogwheel(int type) : CGameObject()
	{
		this->type = type;
	}
	~CCogwheel() {}
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	virtual void Render();
	void SetState(int state);
	virtual void GetBoundingBox(float& l, float& t, float& r, float& b);
};


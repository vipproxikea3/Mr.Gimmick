#pragma once
#include "GameObject.h"
#include "Utils.h"

#define BLUEFIRE_ANI_BURN		0

#define BLUEFIRE_BBOX_WIDTH		16
#define BLUEFIRE_BBOX_HEIGHT	16

class CBlueFire : public CGameObject
{
public:
	CBlueFire() : CGameObject()
	{
	}
	~CBlueFire() {}
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	virtual void Render();
	void SetState(int state);
	virtual void GetBoundingBox(float& l, float& t, float& r, float& b);
};


#pragma once
#include "GameObject.h"
#include "Gimmick.h"


#define BRICK_BBOX_WIDTH	32
#define BRICK_BBOX_HEIGHT	16
#define BRICK_SPEED	0.05f

class CBrickPink : public CGameObject
{
	float left, top, leftrans, toptrans,av;
	bool vyb;
public:
	CBrickPink(float left, float top, float leftrans, float toptrans,float av);
	/*~CBrickPink() {}*/
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	virtual void Render();
	//void SetState(int state);
	virtual void GetBoundingBox(float& l, float& t, float& r, float& b);
};


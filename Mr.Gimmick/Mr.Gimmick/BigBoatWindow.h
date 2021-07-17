#pragma once
#include "GameObject.h"

#define BIG_BOAT_WINDOW_ANI	0
#define BIG_BOAT_WINDOW_BBOX_WIDTH	32
#define BIG_BOAT_WINDOW_BBOX_HEIGHT	48

class CBigBoatWindow : public CGameObject
{
public:
	CBigBoatWindow() : CGameObject() {};
	~CBigBoatWindow() {};
	virtual void Render();
	virtual void GetBoundingBox(float& l, float& t, float& r, float& b);
};

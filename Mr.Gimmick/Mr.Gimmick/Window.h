#pragma once
#include "GameObject.h"

#define WINDOW_ANI_TYPE_1	0
#define WINDOW_ANI_TYPE_2	1
#define WINDOW_ANI_TYPE_3	2
#define WINDOW_ANI_TYPE_4	3

#define WINDOW_BBOX_WIDTH	16
#define WINDOW_BBOX_HEIGHT	16

class CWindow : public CGameObject
{
	int type;
public:
	CWindow(int type) : CGameObject() {
		this->type = type;
	}
	~CWindow() {}
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	virtual void Render();
	void SetState(int state);
	virtual void GetBoundingBox(float& l, float& t, float& r, float& b);
};


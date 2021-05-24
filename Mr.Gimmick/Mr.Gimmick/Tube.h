#pragma once
#include "GameObject.h"

#define TUBE_ANI_TYPE_1		0
#define TUBE_ANI_TYPE_2		1
#define TUBE_ANI_TYPE_3		2
#define TUBE_ANI_TYPE_4		3
#define TUBE_ANI_TYPE_5		4
#define TUBE_ANI_TYPE_6		5
#define TUBE_ANI_TYPE_7		6
#define TUBE_ANI_TYPE_8		7
#define TUBE_ANI_TYPE_9		8
#define TUBE_ANI_TYPE_10	9

#define TUBE_BBOX_WIDTH	16
#define TUBE_BBOX_HEIGHT	16

class CTube : public CGameObject
{
	int type;
public:
	CTube(int type) : CGameObject() {
		this->type = type;
	}
	~CTube() {}
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	virtual void Render();
	void SetState(int state);
	virtual void GetBoundingBox(float& l, float& t, float& r, float& b);
};


#pragma once
#include "GameObject.h"

#define SEWER_TYPE_1	0
#define SEWER_TYPE_2	1
#define SEWER_TYPE_3	2
#define SEWER_TYPE_4	3
#define SEWER_TYPE_5	4
#define SEWER_TYPE_6	5

#define SEWER_BBOX_WIDTH	16
#define SEWER_BBOX_HEIGHT	16

class CSewer : public CGameObject
{
public:
	int type;
	CSewer(float type) : CGameObject() {
		this->type = type;
	}
	~CSewer() {}
	int Collision(LPGAMEOBJECT object, float dy);
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	virtual void Render();
	void SetState(int state);
	virtual void GetBoundingBox(float& l, float& t, float& r, float& b);
	float GetX() { return this->x; };
	float GetY() { return this->y; };
};


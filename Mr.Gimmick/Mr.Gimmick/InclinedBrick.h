#pragma once
#include "GameObject.h"

#define INCLINEDBRICK_BBOX_WIDTH	16
#define INCLINEDBRICK_BBOX_HEIGHT	16

class CInclinedBrick : public CGameObject
{
	float ly, ry;
	int isT;
public:
	CInclinedBrick(float ly, float ry, int isT) : CGameObject() {
		this->ly = ly;
		this->ry = ry;
		this->isT = isT;
	}
	~CInclinedBrick() {}
	void Collision(LPGAMEOBJECT object, float dy);
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	virtual void Render();
	void SetState(int state);
	virtual void GetBoundingBox(float& l, float& t, float& r, float& b);
};


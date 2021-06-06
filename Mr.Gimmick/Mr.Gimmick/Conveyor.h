#pragma once
#include "GameObject.h"

#define CONVEYOR_ANI_RIGHT		0
#define CONVEYOR_ANI_LEFT		1

#define CONVEYOR_BBOX_WIDTH		16
#define CONVEYOR_BBOX_HEIGHT	16

class CConveyor : public CGameObject
{
	int direction;
public:
	CConveyor(int direction) : CGameObject() {
		this->direction = direction;
	}
	int GetDirection() { return this->direction; }
	~CConveyor() {}
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	virtual void Render();
	void SetState(int state);
	virtual void GetBoundingBox(float& l, float& t, float& r, float& b);
};


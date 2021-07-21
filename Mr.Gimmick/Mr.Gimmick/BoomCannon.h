#pragma once
#include "GameObject.h"
#include "Game.h"
#include "PlayScene.h"

#define BOOM_CANNON_BOX_WIDTH	15
#define BOOM_CANNON_BOX_HEIGHT	15
#define BOOM_CANNON_GRAVITY				0.01f
#define BOOM_CANNON_TIMEDELETE          1500
#define BOOM_CANNON_TIMEPREDELETE		300

#define BOOM_CANNON_STATE_MOVE			0
#define BOOM_CANNON_STATE_NORMAL		1
#define	BOOM_CANNON_STATE_DIE			2

#define BOOM_CANNON_ANI_MOVE			0

class CBoomCannon : public CGameObject
{
public:

	bool stopUpdate = false;
	int timeDelete = BOOM_CANNON_TIMEDELETE;
	int timePreDelete = BOOM_CANNON_TIMEPREDELETE;
	bool isDelete = false;

	CBoomCannon();
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	virtual void Render();
	virtual void GetBoundingBox(float& l, float& t, float& r, float& b);
	void SetState(int state);
};

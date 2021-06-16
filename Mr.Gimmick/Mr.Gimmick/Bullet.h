#pragma once
#include "GameObject.h"
#include "Game.h"
#include "PlayScene.h"

#define BULLET_BOX_WIDTH 12
#define BULLET_BOX_HEIGHT 12
#define BULLET_GRAVITY				0.005f
#define BULLET_TIMEDELETE           7000
#define BULLET_TIMEPREDELETE		300

#define BULLET_STATE_MOVE	0
#define BULLET_STATE_EXPLODE	1

#define BULLET_ANI_MOVE	0
#define BULLET_ANI_EXPLODE	1

class CBullet : public CGameObject
{
public:
	bool onInclinedBrick = false;
	bool onGround = false;
	bool stopUpdate = false;
	int timeDelete = BULLET_TIMEDELETE;
	int timePreDelete = BULLET_TIMEPREDELETE;
	bool isDelete = false;

	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	virtual void Render();
	void SetState(int state);
	virtual void GetBoundingBox(float& l, float& t, float& r, float& b);
};

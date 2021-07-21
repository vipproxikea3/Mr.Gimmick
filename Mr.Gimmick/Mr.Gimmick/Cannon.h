#pragma once
#include "GameObject.h"
#include "Game.h"
#include "Gimmick.h"
#include "PlayScene.h"
#include "Brick.h"


#define CANNON_BBOX_WIDTH	24
#define CANNON_BBOX_HEIGHT	30
#define CANNON_DELAYTIME	3000
#define CANNON_GRAVITY		0.009f
#define CANNON_ACCELERATION	0.00015f

#define CANNON_STATE_IDLE	0
#define CANNON_STATE_MOVE	1

#define CANNON_DELAY_TIME 3000

#define CANNON_ANI_IDLE		0
#define CANNON_ANI_MOVE		1

class CCannon : public CGameObject
{
private:
	int delay = 0;

public:
	CCannon();
	~CCannon();
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	virtual void Render();
	virtual void GetBoundingBox(float& l, float& t, float& r, float& b);
};


#pragma once
#include "GameObject.h"
#include "Game.h"
#include "Gimmick.h"
#include "Bullet.h"
#include "PlayScene.h"
#include "Brick.h"


#define GUN_BBOX_WIDTH	16
#define GUN_BBOX_HEIGHT	15
#define GUN_DELAYTIME	3000
#define GUN_GRAVITY				0.001f
#define GUN_ACCELERATION		0.00015f

#define GUN_STATE_WALKING	0

#define GUN_ANI_GREEN	0
#define GUN_ANI_RED	1

class CGun : public CGameObject
{
private:
	int delay = 0;
public:
	bool onInclinedBrick = false;
	bool onGround = false;

	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	virtual void Render();
	virtual void GetBoundingBox(float& l, float& t, float& r, float& b);
	void updateX(float a) {
		this->x += a;
	}
};


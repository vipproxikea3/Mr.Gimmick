#pragma once
#include "GameObject.h"
#include "Brick.h"
#include "PlayScene.h"
#include "Game.h"					

#define SWORD_STATE_FLY			1
#define SWORD_STATE_ONGROUND	2

#define SWORD_ANI_FLY				0
#define SWORD_ANI_ONGROUND			1

#define SWORD_ANISET_ID			520001

#define SWORD_BBOX_WIDTH		22
#define SWORD_BBOX_HEIGHT		24

#define SWORD_FLY_RANGE_Y		80

#define SWORD_FLY_SPEED_Y		0.2f
#define SWORD_FLY_SPEED_X		0.02f

class CSword : public CGameObject
{
public:
	float start_y;
	CSword(float init_x, float init_y);
	~CSword() {}
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	virtual void Render();
	virtual void SetState(int state);
	virtual void GetBoundingBox(float& l, float& t, float& r, float& b);
};



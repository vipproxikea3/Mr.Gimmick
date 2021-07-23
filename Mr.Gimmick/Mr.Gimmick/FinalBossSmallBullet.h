#pragma once
#include "GameObject.h"
#include "Brick.h"
#include "PlayScene.h"
#include "Game.h"		

#define SMALLBULLET_STATE_FLY			1
#define SMALLBULLET_STATE_EXPLOSE		2

#define SMALLBULLET_ANI_FLY				0
#define SMALLBULLET_ANI_EXPLOSE			1

#define SMALLBULLET_SPEED_X				0.1f
#define SMALLBULLET_SPEED_Y				0.08f

#define SMALLBULLET_ANISET_ID			550001

#define SMALLBULLET_BBOX_WIDTH			4
#define SMALLBULLET_BBOX_HEIGHT			4


class CFinalBossSmallBullet: public CGameObject
{
public:
	ULONGLONG explose_start = 0;
	CFinalBossSmallBullet(float init_vx, float init_vy);
	~CFinalBossSmallBullet() {}
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	virtual void Render();
	virtual void SetState(int state);
	virtual void GetBoundingBox(float& l, float& t, float& r, float& b);
	void SpecialCollisionWithPlayer();
	void DetectStar();
};


#pragma once
#include "GameObject.h"
#include "Brick.h"
#include "PlayScene.h"
#include "Game.h"
#include "Gimmick.h"

#define FINALBOSS_BIG_BULLET_ANISET_ID		540001

#define FINALBOSS_BIG_BULLET_BBOX_WIDTH			10
#define FINALBOSS_BIG_BULLET_BBOX_HEIGHT		50

#define FINALBOSS_BIG_BULLET_STATE_LEFT				1
#define FINALBOSS_BIG_BULLET_STATE_RIGHT			2

#define FINALBOSS_BIG_BULLET_ANI_LEFT			0
#define FINALBOSS_BIG_BULLET_ANI_RIGHT			1

#define FINALBOSS_BIG_BULLET_SPEED				0.1f
class CFinalBossBigBullet : public CGameObject
{
public: 

	CFinalBossBigBullet(int direction); 
	~CFinalBossBigBullet() {}
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	virtual void Render();
	virtual void SetState(int state);
	virtual void GetBoundingBox(float& l, float& t, float& r, float& b);
	void SpecialCollisionWithPlayer();
	void DetectStar();
};


#pragma once
#include "GameObject.h"

#define ENEMY_BBOX_WIDTH	16
#define ENEMY_BBOX_HEIGHT	15

#define ENEMY_DIE_DEFLECT_SPEED	 0.075f
#define ENEMY_GRAVITY	0.0004f

#define ENEMY_STATE_STAND	0
#define ENEMY_STATE_DIE	100

#define ENEMY_ANI_STAND	0
#define ENEMY_ANI_DIE	1


class CStandBlackEnemy : public CGameObject
{
public:
	CStandBlackEnemy();
	~CStandBlackEnemy() {}
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	virtual void Render();
	void SetState(int state);
	virtual void GetBoundingBox(float& l, float& t, float& r, float& b);
};


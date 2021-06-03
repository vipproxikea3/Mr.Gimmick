#pragma once
#include "GameObject.h"
#include "Brick.h"

#define BLACKENEMY_STATE_DIE	0
#define BLACKENEMY_STATE_WALK	1

#define BLACKENEMY_WIDTH		16
#define BLACKENEMY_HEIGHT		16

#define BLACKENEMY_WALK_SPEED		0.03f
#define BLACKENEMY_GRAVITY			0.001f
#define BLACKENEMY_JUMP_SPEED		0.3f

#define BLACKENEMY_ANI_WALK_RIGHT	0
#define BLACKENEMY_ANI_WALK_LEFT	1

class CBlackEnemy : public CGameObject
{
public:
	bool onGround = true;
	CBlackEnemy();
	~CBlackEnemy() {}
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	virtual void Render();
	virtual void SetState(int state);
	virtual void GetBoundingBox(float& l, float& t, float& r, float& b);
	void Jump();
	bool onTopOf(CGameObject* object);
};


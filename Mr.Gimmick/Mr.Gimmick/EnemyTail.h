#pragma once
#include "GameObject.h"

#define	ENEMYTAIL_GRAVITY				0.0005f
#define ENEMYTAIL_WALKING_SPEED			0.01f

#define	ENEMYTAIL_BBOX_WIDTH			16
#define	ENEMYTAIL_BBOX_HEIGHT			16

#define	ENEMYTAIL_STATE_WALKING_RIGHT	100
#define	ENEMYTAIL_STATE_WALKING_LEFT	200

#define ENEMYTAIL_ANI_WALKING_RIGHT		0
#define ENEMYTAIL_ANI_WALKING_LEFT		1

class CEnemyTail : public CGameObject
{
	float left = NULL;
	float right = NULL;
public:
	CEnemyTail(float left, float right);
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* colliable_objects = NULL);
	virtual void Render();
	void SetState(int state);
	virtual void GetBoundingBox(float& l, float& t, float& r, float& b);
};


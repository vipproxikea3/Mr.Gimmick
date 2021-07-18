#pragma once
#include "GameObject.h"

#define	ENEMYBOOM_GRAVITY				0.0005f
#define ENEMYBOOM_WALKING_SPEED			0.05f
#define ENEMYBOOM_DIE_SPEED				0.075f

#define	ENEMYBOOM_BBOX_WIDTH			16
#define	ENEMYBOOM_BBOX_HEIGHT			32

#define	ENEMYBOOM_STATE_WALKING_RIGHT	100
#define	ENEMYBOOM_STATE_WALKING_LEFT	200
#define	ENEMYBOOM_STATE_DIE_RIGHT		300
#define	ENEMYBOOM_STATE_DIE_LEFT		400

#define ENEMYBOOM_ANI_WALKING_RIGHT		0
#define ENEMYBOOM_ANI_WALKING_LEFT		1
#define ENEMYBOOM_ANI_DIE_RIGHT			2
#define ENEMYBOOM_ANI_DIE_LEFT			3

class CEnemyBoom : public CGameObject
{
	float left = NULL;
	float right = NULL;
public:
	CEnemyBoom(float left, float right);
	void Shot();
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* colliable_objects = NULL);
	virtual void Render();
	void SetState(int state);
	virtual void GetBoundingBox(float& l, float& t, float& r, float& b);
};


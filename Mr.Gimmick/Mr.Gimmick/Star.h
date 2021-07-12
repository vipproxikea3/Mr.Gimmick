#pragma once
#include "GameObject.h"

#define	STAR_BBOX_WIDTH		16
#define	STAR_BBOX_HEIGHT	16

#define	STAR_STATE_DISABLE	100
#define	STAR_STATE_ACTIVE	200

#define STAR_ANI_ACTIVE		0

#define	STAR_GRAVITY		0.15f

class CStar : public CGameObject
{

public:
	CStar();
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* colliable_objects = NULL);
	virtual void Render();
	void SetState(int state);
	virtual void GetBoundingBox(float& l, float& t, float& r, float& b);
};


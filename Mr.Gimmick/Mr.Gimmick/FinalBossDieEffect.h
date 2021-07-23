#pragma once
#include "Utils.h"
#include "GameObject.h"

#define FINALBOSSDIEEFFECT_ANI_SET		560001

#define FINALBOSSDIEEFFECT_ANI_WHITE		0
#define FINALBOSSDIEEFFECT_ANI_WHITE2		1

#define FINALBOSSDIEEFFECT_STATE_WHITE			0
#define FINALBOSSDIEEFFECT_STATE_RED			1

#define FINALBOSSDIEEFFECT_BBOX_WIDTH		8
#define FINALBOSSDIEEFFECT_BBOX_HEIGHT		8

class CFinalBossDieEffect : public CGameObject
{
public:
	CFinalBossDieEffect(float init_x, float init_y);
	~CFinalBossDieEffect() {}
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	virtual void Render();
	void SetState(int state);
	virtual void GetBoundingBox(float& l, float& t, float& r, float& b);
	ULONGLONG render_time = 0;
};


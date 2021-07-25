#pragma once
#include "GameObject.h"

#define	SWING_BBOX_WIDTH	38
#define	SWING_BBOX_HEIGHT	68

//#define	SWING_OPEN_1_BBOX_WIDTH		38
//#define	SWING_OPEN_1_BBOX_HEIGHT	76
//
//#define	SWING_OPEN_2_BBOX_WIDTH		38
//#define	SWING_OPEN_2_BBOX_HEIGHT	78

#define	SWING_STATE_STAND	100
#define	SWING_STATE_MOVE	200
#define	SWING_STATE_OPEN	300
#define	SWING_STATE_STOP	400

#define	SWING_ANI_STAND		0
#define	SWING_ANI_MOVE		1
#define	SWING_ANI_OPEN		2
#define	SWING_ANI_STOP		3

#define	SWING_SPEED_X		0.015
#define SWING_TIME_OPEN		300
#define	SWING_TIME_STOP		200

#define	SWING_LIMIT_X		1920


class CSwing : public CGameObject
{
	DWORD open_start = 0;

public:
	CSwing();
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* colliable_objects = NULL);
	void Revival();
	virtual void Render();
	void StartOpen() { open_start = GetTickCount64(); }
	void SetState(int state);

	virtual void GetBoundingBox(float& l, float& t, float& r, float& b);
};
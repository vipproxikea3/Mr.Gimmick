#pragma once
#include "GameObject.h"
#include "Brick.h"
#include "Conveyor.h"
#include "InclinedBrick.h"
#include "Sewer.h"

#define	STAR_GRAVITY			0.001f
#define STAR_VX					0.15f
#define STAR_WALKING_FRICTION	0.0008f

#define	STAR_BBOX_WIDTH			16
#define	STAR_BBOX_HEIGHT		16

#define	STAR_STATE_HIDE				100
#define	STAR_STATE_PENDING			200
#define	STAR_STATE_CREATED			300
#define	STAR_STATE_READY			400
#define	STAR_STATE_WALKING_RIGHT	500
#define	STAR_STATE_WALKING_LEFT		600
#define	STAR_STATE_EXPLOSIVE		700

#define STAR_ANI_SMALL			0
#define STAR_ANI_CREATED		1
#define STAR_ANI_MAIN			2
#define STAR_ANI_EXPLOSIVE		3

#define STAR_SMALL_V			0.05f

#define STAR_SMALL_1_X0			0
#define STAR_SMALL_1_Y0			32
#define STAR_SMALL_1_VX			0
#define STAR_SMALL_1_VY			-1

#define STAR_SMALL_2_X0			30.4f
#define STAR_SMALL_2_Y0			9.92f
#define STAR_SMALL_2_VX			-0.95f
#define STAR_SMALL_2_VY			-0.31f

#define STAR_SMALL_3_X0			18.9f
#define STAR_SMALL_3_Y0			-25.92f
#define STAR_SMALL_3_VX			-0.59f
#define STAR_SMALL_3_VY			0.81f

#define STAR_SMALL_4_X0			-18.9f
#define STAR_SMALL_4_Y0			-25.92f
#define STAR_SMALL_4_VX			0.59f
#define STAR_SMALL_4_VY			0.81f

#define STAR_SMALL_5_X0			-30.4f
#define STAR_SMALL_5_Y0			9.92f
#define STAR_SMALL_5_VX			0.95f
#define STAR_SMALL_5_VY			-0.31f

#define STAR_EXPLOSIVE_TIME		250

#define STAR_DELAY_FRAME		2


class CStar : public CGameObject
{
	float star_small_1_x = STAR_SMALL_1_X0;
	float star_small_1_y = STAR_SMALL_1_Y0;

	float star_small_2_x = STAR_SMALL_2_X0;
	float star_small_2_y = STAR_SMALL_2_Y0;

	float star_small_3_x = STAR_SMALL_3_X0;
	float star_small_3_y = STAR_SMALL_3_Y0;

	float star_small_4_x = STAR_SMALL_4_X0;
	float star_small_4_y = STAR_SMALL_4_Y0;

	float star_small_5_x = STAR_SMALL_5_X0;
	float star_small_5_y = STAR_SMALL_5_Y0;

	DWORD explosive_start;

	int current_frame = 0;
	float star_delay_1_x = 0;
	float star_delay_1_y = 0;
	float star_delay_2_x = 0;
	float star_delay_2_y = 0;
	float star_delay_3_x = 0;
	float star_delay_3_y = 0;

public:
	CStar();
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* colliable_objects = NULL);
	void Ready();
	void Shot();
	virtual void Render();
	void SetState(int state);
	virtual void GetBoundingBox(float& l, float& t, float& r, float& b);
};


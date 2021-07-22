#pragma once
#include "GameObject.h"
#include "Gimmick.h"

#define	MEDICINE_ORANGE_BBOX_WIDTH	14
#define	MEDICINE_ORANGE_BBOX_HEIGHT	16

#define MEDICINE_PINK_BBOX_WIDTH	14
#define	MEDICINE_PINK_BBOX_HEIGHT	14	

#define	MEDICINE_TYPE_3_4_BBOX_WIDTH	14
#define	MEDICINE_TYPE_3_4_BBOX_HEIGHT	15

#define	VASE_FLOWER_BBOX_WIDTH		22
#define	VASE_FLOWER_BBOX_HEIGHT		43

#define HOUR_GLASS_BBOX_WIDTH		17
#define HOUR_GLASS_BBOX_HEIGHT		28

#define MEDICINE_ANISET_ID			170001

#define	MEDICINE_ANI_ORANGE			0
#define	MEDICINE_ANI_PINK			1
#define	MEDICINE_ANI_TYPE_3			2
#define	MEDICINE_ANI_TYPE_4			3
#define	VASE_ANI_FLOWER				4
#define	HOUR_ANI_GLASS				5

#define	MEDICINE_STATE_APPEAR		100
#define	MEDICINE_STATE_DISAPPEAR	200

#define	MEDICINE_GRAVITY			0.0005f

class CMedicine : public CGameObject
{
	int type;

public:
	CMedicine(int type);
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* colliable_objects = NULL);
	virtual void Render();

	void SetState(int state);
	virtual void GetBoundingBox(float& l, float& t, float& r, float& b);
};
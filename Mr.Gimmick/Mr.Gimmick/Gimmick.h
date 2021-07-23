#pragma once
#include "GameObject.h"
#include "Brick.h"
#include "BrickPink.h"
#include "InclinedBrick.h"
#include "Conveyor.h"
#include "Swing.h"
#include "Medicine.h"
#include "Sewer.h"
#include "CloudEnemy.h"


#define GIMMICK_WALKING_SPEED		0.1f
#define GIMMICK_ACCELERATION		0.0002f
#define GIMMICK_WALKING_FRICTION	0.00025f
#define GIMMICK_JUMP_SPEED_Y_MIN	0.15f
#define GIMMICK_JUMP_SPEED_Y_MAX	0.35f	//0.25f
#define GIMMICK_JUMP_ACCELERATION	0.001f	//0.001f
#define GIMMICK_GRAVITY				0.001f
#define GIMMICK_DEFLECT_SPEED_X		0.1f

#define GIMMICK_STATE_IDLE			0
#define GIMMICK_STATE_WALKING_RIGHT	100
#define GIMMICK_STATE_WALKING_LEFT	200
#define GIMMICK_STATE_JUMP			300
#define GIMMICK_STATE_STUN			400
#define GIMMICK_STATE_DIE			500
#define GIMMICK_STATE_IN_SEWER		600

#define GIMMICK_ANI_IDLE_RIGHT		0
#define GIMMICK_ANI_IDLE_LEFT		1
#define GIMMICK_ANI_WALKING_RIGHT	2
#define GIMMICK_ANI_WALKING_LEFT	3
#define GIMMICK_ANI_JUMP_RIGHT		4
#define GIMMICK_ANI_JUMP_LEFT		5
#define GIMMICK_ANI_STUN_RIGHT		6
#define GIMMICK_ANI_STUN_LEFT		7
#define GIMMICK_ANI_DIE				8
#define GIMMICK_ANI_IN_SEWER_LEFT	9
#define GIMMICK_ANI_IN_SEWER_RIGHT	10

#define GIMMICK_BBOX_WIDTH			16
#define GIMMICK_BBOX_HEIGHT			16

#define GIMMICK_UNTOUCHABLE_TIME	2000
#define GIMMICK_STUNNING_TIME		500
#define GIMMICK_REVIVAL_TIME		2000


class CGimmick : public CGameObject
{
	DWORD untouchable_start;

	DWORD die_start;

	DWORD stunning_start;
	float ax = NULL;

	
	bool updated = false;
public:
	int untouchable;
	float ay = 0;
	bool onGround = false;
	bool falling = false;
	bool jumping = false;
	bool stunning = false;
	bool onEnemy = false;
	bool facingBrick = false; //fix loi dung tren lung quai bi xuyen gach
	bool underBrick = false;
	bool inSewer = false;// check in sewer
	bool equalinSewer = false; // check in sewer3
	float XSewer, YSewer, nSewer; // check out sewer 
	bool onStar = false;
	bool onBoat = false; // check on boat

	void CreateDieEffect();

	CGimmick();
	void CalculateSpeed(DWORD dt);
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* colliable_objects = NULL);
	virtual void Render();

	void SetState(int state);
	void StartUntouchable() { untouchable = 1; untouchable_start = DWORD(GetTickCount64()); }
	void SetdifY(float y) { this->y += y; };

	virtual void GetBoundingBox(float& left, float& top, float& right, float& bottom);
	bool onTopOf(CGameObject* object, float equal = 1);
	bool onSideOf(CGameObject* object, float equal = 1);
	void standOn(CGameObject* object);
	bool isUnder(CGameObject* object, float equal = 1);
	int getUntouchable() { return untouchable; }
	void DetectStar();
	void Revival();

	void UpdateWithBird(float x, float y) { this->x += x; this->y += y; };
};
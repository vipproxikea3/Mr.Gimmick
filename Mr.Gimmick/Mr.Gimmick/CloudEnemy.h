#pragma once
#include "GameObject.h"

#define CLOUD_BBOX_WIDTH	24
#define CLOUD_BBOX_HEIGHT	25

#define CLOUD_WALKING_SPEED		0.025f//0.075f 
#define CLOUD_WALKING_ATTACK	0.075f
#define CLOUD_DIE_DEFLECT_SPEED	 0.05f
#define CLOUD_GRAVITY	0.0004f
#define CLOUD_ACCELERATION		0.0002f

#define CLOUD_STATE_IDLE	0
#define CLOUD_STATE_WALK	100
#define CLOUD_STATE_ATTACK 200
#define CLOUD_STATE_BEING_ATTACK	300
#define CLOUD_STATE_CARRY	400
#define CLOUD_STATE_STUN	500


#define CLOUD_ANI_IDLE_RIGHT	0
#define CLOUD_ANI_GO_RIGHT	1
#define CLOUD_ANI_IDLE_LEFT	2
#define CLOUD_ANI_GO_LEFT	3



class CCloudEnemy : public CGameObject
{

public:
	int direction = 1;
	int turnTime = 2000;
	bool carryPlayer = false;
	float limitRight = 99999, limitLeft = -99999;
	int tmpState = CLOUD_STATE_WALK;

	CCloudEnemy();
	CCloudEnemy(int left, int right);
	~CCloudEnemy() {}
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	virtual void Render();
	void SetState(int state = -1);
	virtual void GetBoundingBox(float& l, float& t, float& r, float& b);
};


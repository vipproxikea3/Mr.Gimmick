#pragma once
#include "GameObject.h"
#include <math.h> 

#define BLACKBIRD_STATE_TRANSFORM		1	
#define BLACKBIRD_STATE_WAITPLAYER		2	
#define BLACKBIRD_STATE_CARRYPLAYER		3	
#define BLACKBIRD_STATE_DROPPLAYER		4	

#define BLACKBIRD_BBOX_WIDTH	20
#define BLACKBIRD_BBOX_HEIGHT	16
#define BLACKBIRD_COUNTONBIRD	5		


#define BLACKBIRD_VX			0.03f
#define BLACKBIRD_VY			0.01f

class CBlackBird : public CGameObject
{
	int sinX = 0;			
	int countOnBird = -1;	
	float endPoint = 850;
	int direction = 1;		// Huong ->> 1,  <-- -1

	int timeTransform = 1000;
	int timeGoWaitPlace = 1500;

public:
	CBlackBird() {};
	CBlackBird(int direction, float endPoint); 
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	virtual void Render();
	virtual void GetBoundingBox(float& l, float& t, float& r, float& b);
	void Revival();

	void setOnBird(bool x); 
	bool DropPlayer(); 
	
};


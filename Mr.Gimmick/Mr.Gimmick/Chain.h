#pragma once
#include "GameObject.h"
#include "Utils.h"

#define CHAIN_ANI_TOP_RIGHT	0
#define CHAIN_ANI_BOT_RIGHT	1
#define CHAIN_ANI_BOT_LEFT	2
#define CHAIN_ANI_TOP_LEFT	3
#define CHAIN_ANI_DOWN		4
#define CHAIN_ANI_UP		5

#define CHAIN_BBOX_WIDTH	16
#define CHAIN_BBOX_HEIGHT	16

class CChain : public CGameObject
{
	int type;
public:
	CChain(int type) : CGameObject()
	{
		this->type = type;
	}
	~CChain() {}
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	virtual void Render();
	void SetState(int state);
	virtual void GetBoundingBox(float& l, float& t, float& r, float& b);
};


#pragma once
#include "GameObject.h"

#define BRIDGE_ANI_TYPE_1_TOP	0
#define BRIDGE_ANI_TYPE_1_DOWN	1
#define BRIDGE_ANI_TYPE_2_TOP	2
#define BRIDGE_ANI_TYPE_2_DOWN	3
#define BRIDGE_ANI_TYPE_3_TOP	4
#define BRIDGE_ANI_TYPE_3_DOWN	5
#define BRIDGE_ANI_TYPE_4_TOP	6
#define BRIDGE_ANI_TYPE_4_DOWN	7

#define BRIDGE_BBOX_WIDTH		16
#define BRIDGE_BBOX_HEIGHT		16

class CBridge : public CGameObject
{
	int type;
public:
	CBridge(int type) : CGameObject() {
		this->type = type;
	}
	~CBridge() {}
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	virtual void Render();
	void SetState(int state);
	virtual void GetBoundingBox(float& l, float& t, float& r, float& b);
};


#pragma once
#include "GameObject.h"
#include "Gimmick.h"
#include "PlayScene.h"
#include "algorithm"

#define	DOOR_BBOX_WIDTH		48
#define	DOOR_BBOX_HEIGHT	16

#define DOOR_STATE_CLOSE	0
#define	DOOR_STATE_OPEN		1
#define	DOOR_STATE_STOP		2

#define	DOOR_ANI_CLOSE		0
#define	DOOR_ANI_OPEN		1
#define	DOOR_ANI_STOP		2

#define	DOOR_LIMIT_TIME		100

class CDoor : public CGameObject
{
	DWORD open_start = 0;

public:
	CDoor();
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* colliable_objects = NULL);
	virtual void Render();
	void StartOpen() { open_start = GetTickCount64(); }
	void SetState(int state);
	virtual void GetBoundingBox(float& left, float& top, float& right, float& bottom);
};


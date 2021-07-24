#pragma once

#include "GameObject.h"
#include "Game.h"
#include "Utils.h"
#include "Backup.h"


#define HUD_HEIGHT	32
#define HUD_WIDTH	256
#define HUD_ZONE_HEIGHT	32

//Thoi gian tang giam diem so
#define HUB_TIME_DP	2000

class CHud : public CGameObject
{
	//Score
	int timeDP = HUB_TIME_DP; // thời gian để nó tăng
	int dpScore = 0;
	int curScore = 0;
	float displayScore = 0;

	//Rest
	int rest = 5;

	//Light
	int light1 = 3, light2 = 3, light3 = 3, light4 = 3;

	//Item
	int item1 = 0, item2 = 0, item3 = 0;


public:
	CHud() {
		CBackup* backup = CBackup::GetInstance();
		this->score = backup->score;
		this->dpScore = backup->score;
		this->curScore = backup->score;
		this->rest = backup->rest;
		if (backup->lifeStack == 0) {
			this->light1 = 0;
			this->light2 = 0;
			this->light3 = 0;
			this->light4 = 0;
		}
		if (backup->lifeStack == 1) {
			this->light1 = 3;
			this->light2 = 0;
			this->light3 = 0;
			this->light4 = 0;
		}
		if (backup->lifeStack == 2) {
			this->light1 = 3;
			this->light2 = 3;
			this->light3 = 0;
			this->light4 = 0;
		}
		if (backup->lifeStack == 3) {
			this->light1 = 3;
			this->light2 = 3;
			this->light3 = 3;
			this->light4 = 0;
		}
		if (backup->lifeStack == 4) {
			this->light1 = 3;
			this->light2 = 3;
			this->light3 = 3;
			this->light4 = 3;
		}

		switch (backup->item_1)
		{
		case BACKUP_ITEM_TYPE_NULL:
			this->item1 = 0;
			break;
		case BACKUP_ITEM_TYPE_MEDICINE:
			this->item1 = 4;
			break;
		case BACKUP_ITEM_TYPE_BOOM:
			this->item1 = 5;
			break;
		case BACKUP_ITEM_TYPE_CIRCLE:
			this->item1 = 6;
			break;
		default:
			break;
		}

		switch (backup->item_2)
		{
		case BACKUP_ITEM_TYPE_NULL:
			this->item1 = 0;
			break;
		case BACKUP_ITEM_TYPE_MEDICINE:
			this->item2 = 1;
			break;
		case BACKUP_ITEM_TYPE_BOOM:
			this->item2 = 2;
			break;
		case BACKUP_ITEM_TYPE_CIRCLE:
			this->item2 = 3;
			break;
		default:
			break;
		}

		switch (backup->item_3)
		{
		case BACKUP_ITEM_TYPE_NULL:
			this->item3 = 0;
			break;
		case BACKUP_ITEM_TYPE_MEDICINE:
			this->item3 = 1;
			break;
		case BACKUP_ITEM_TYPE_BOOM:
			this->item3 = 2;
			break;
		case BACKUP_ITEM_TYPE_CIRCLE:
			this->item3 = 3;
			break;
		default:
			break;
		}
	}

	virtual void Update(DWORD dt);
	virtual void Render();
	virtual void GetBoundingBox(float& l, float& t, float& r, float& b);

	void UpdateScore(int score) {
		dpScore = score - this->curScore;
		this->curScore = score;
		timeDP = HUB_TIME_DP;
	}
	void RenderScore();


	void UpdateRest(int rest) {
		this->rest = rest;
	}
	void RenderRest();


	//0 tat, 1 , 2 , 3 do sang tang dan
	void UpdateLight(int l1, int l2, int l3, int l4) {
		light1 = l1;
		light2 = l2;
		light3 = l3;
		light4 = l4;
	};
	void RenderLight();



	void UpdateItem(int item1, int item2, int item3) {
		this->item1 = item1;
		this->item2 = item2;
		this->item3 = item3;
	}
	void RenderItem();
};
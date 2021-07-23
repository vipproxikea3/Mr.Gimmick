#pragma once

#include "GameObject.h"
#include "Game.h"
#include "Utils.h"


#define HUD_HEIGHT	32
#define HUD_WIDTH	256
#define HUD_ZONE_HEIGHT	32

//Thoi gian tang giam diem so
#define HUB_TIME_DP	3000

class CHud : public CGameObject
{
	//Score
	int timeDP = HUB_TIME_DP; // thời gian để nó tăng
	int dpScore = 0;
	int curScore = 0;
	int displayScore = 0;

	//Rest
	int rest = 5;

	//Light
	int light1 = 3, light2 = 3, light3 = 3, light4 = 3;

	//Item
	int item1 = 6, item2 = 4, item3 = 1;


public:
	CHud() {
		UpdateScore(100);
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
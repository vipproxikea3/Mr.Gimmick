#include "Hud.h"


void CHud::Update(DWORD dt)
{
	this->dt = dt;
}

void CHud::Render()
{

	CAnimationSets* animation_sets = CAnimationSets::GetInstance();
	LPANIMATION_SET ani_set = animation_sets->Get(300001);
	this->SetAnimationSet(ani_set);



	//background
	animation_set->at(0)->Render(x, y, 255);

	RenderScore();
	RenderRest();
	RenderLight();
	RenderItem();
}

void CHud::GetBoundingBox(float& l, float& t, float& r, float& b)
{
	l = x;
	r = x + HUD_WIDTH;
	t = y;
	b = t - HUD_HEIGHT;
}

void CHud::RenderScore()
{
	//DebugOut(L"%d    %d    %d\n", timeDP, dt, timeDP - dt);

	/*if (timeDP - int(dt) > 0) {
		displayScore += int(float(dpScore) / HUB_TIME_DP * dt);
		timeDP = timeDP - dt;
	}
	else {
		displayScore = curScore;
		timeDP = 0;
	}*/

	displayScore = 99;

	int tmpScore = displayScore;

	float x_Score = x + 80;
	while (tmpScore > 0) {
		int i = tmpScore % 10;
		tmpScore /= 10;
		animation_set->at(1)->Render(x_Score - 8, y - 16, 255, i);
		x_Score -= 8;
	}
	
	
}

void CHud::RenderRest()
{
	int tmpRest = rest;

	float x_Rest = x + 112;
	while (tmpRest > 0) {
		int i = tmpRest % 10;
		tmpRest /= 10;
		animation_set->at(1)->Render(x_Rest - 8, y - 16, 255, i);
		x_Rest -= 8;
	}

}



void CHud::RenderLight()
{
	if (light1 > 0) {
		animation_set->at(2)->Render(x + 126, y - 11, 255, (light1 - 1) * 4);
	}
	if (light2 > 0) {
		animation_set->at(2)->Render(x + 137, y - 11, 255, (light2 - 1) * 4 + 1);
	}

	if (light3 > 0) {
		animation_set->at(2)->Render(x + 148, y - 11, 255, (light3 - 1) * 4 + 2);
	}
	if (light4 > 0) {
		animation_set->at(2)->Render(x + 159, y - 11, 255, (light4 - 1) * 4 + 3);
	}
}

void CHud::RenderItem()
{

	if (item1 > 0)
		animation_set->at(3)->Render(x + 177, y, 255, item1 - 1);
	if (item2 > 0)
		animation_set->at(3)->Render(x + 201, y, 255, item2 - 1);
	if (item3 > 0)
		animation_set->at(3)->Render(x + 225, y, 255, item3 - 1);

}

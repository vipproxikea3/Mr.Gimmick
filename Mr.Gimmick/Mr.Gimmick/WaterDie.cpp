#include "WaterDie.h"
#include "Brick.h"
#include "Star.h"
#include "PlayScene.h"
#include "Game.h"
#include "Gimmick.h"


void CWaterDie::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects) {
	CGimmick* gimmick = ((CPlayScene*)CGame::GetInstance()->GetCurrentScene())->GetPlayer();
	if (this->CheckAABB(gimmick)) {
		if (gimmick->GetState() != GIMMICK_STATE_DIE) {
			gimmick->SetState(GIMMICK_STATE_DIE);
		}
	}
	CStar* star = ((CPlayScene*)CGame::GetInstance()->GetCurrentScene())->GetStar();
	if (this->CheckAABB(star)&&(star->GetState() == STAR_STATE_WALKING_LEFT|| star->GetState() == STAR_STATE_WALKING_RIGHT))
		star->SetState(STAR_STATE_EXPLOSIVE);

	for (UINT i = 0; i < coObjects->size(); i++)
	{
		if (dynamic_cast<CBlackEnemy*>(coObjects->at(i))) {
			CBlackEnemy* enemy = dynamic_cast<CBlackEnemy*>(coObjects->at(i));
			if (CheckAABB(enemy) && enemy->state != BLACKENEMY_STATE_DIE)
				enemy->SetState(BLACKENEMY_STATE_DIE);
		}

		if (dynamic_cast<CElectricBlackEnemy*>(coObjects->at(i))) {
			CElectricBlackEnemy* enemy = dynamic_cast<CElectricBlackEnemy*>(coObjects->at(i));
			if (CheckAABB(enemy) && enemy->state != ELECTRIC_BLACKENEMY_STATE_DIE)
				enemy->SetState(ELECTRIC_BLACKENEMY_STATE_DIE);
		}
	}
}

void CWaterDie::Render()
{
	RenderBoundingBox();
}

void CWaterDie::SetState(int state) {}

void CWaterDie::GetBoundingBox(float& l, float& t, float& r, float& b)
{
	l = x;
	t = y;
	r = l + this->w;
	b = t - this->h;
}
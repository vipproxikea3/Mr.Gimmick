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
#include "BlueFire.h"
#include "Gimmick.h"
#include "PlayScene.h"
#include "Star.h"

void CBlueFire::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects) {
	CGimmick* gimmick = ((CPlayScene*)CGame::GetInstance()->GetCurrentScene())->GetPlayer();
	if (this->CheckAABB(gimmick)) {
		if (gimmick->GetState() != GIMMICK_STATE_DIE) {
			gimmick->SetState(GIMMICK_STATE_DIE);
		}
	}

	CStar* star = ((CPlayScene*)CGame::GetInstance()->GetCurrentScene())->GetStar();

	if (this->CheckAABB(star) && (star->GetState() == STAR_STATE_WALKING_LEFT || star->GetState() == STAR_STATE_WALKING_RIGHT))
		star->SetState(STAR_STATE_EXPLOSIVE);
}

void CBlueFire::Render()
{
	int ani = BLUEFIRE_ANI_BURN;

	int alpha = 255;
	animation_set->at(ani)->Render(x, y, alpha);
}

void CBlueFire::SetState(int state) {}

void CBlueFire::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
	left = x;
	top = y;
	right = left + BLUEFIRE_BBOX_WIDTH;
	bottom = top - BLUEFIRE_BBOX_HEIGHT;
}
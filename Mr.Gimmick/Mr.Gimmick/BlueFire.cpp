#include "BlueFire.h"
#include "Gimmick.h"
#include "PlayScene.h"

void CBlueFire::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects) {
	CGimmick* gimmick = ((CPlayScene*)CGame::GetInstance()->GetCurrentScene())->GetPlayer();
	if (this->CheckAABB(gimmick)) {
		if (gimmick->GetState() != GIMMICK_STATE_DIE) {
			gimmick->SetState(GIMMICK_STATE_DIE);
		}
	}
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
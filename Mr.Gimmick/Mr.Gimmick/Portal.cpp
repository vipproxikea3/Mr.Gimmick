#include "Portal.h"
#include "PlayScene.h"
#include "Game.h"
#include "Gimmick.h"
#include "Utils.h"
void CPortal::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects) {
	CGimmick* gimmick = ((CPlayScene*)CGame::GetInstance()->GetCurrentScene())->GetPlayer();
	if (this->CheckAABB(gimmick)) {
		if (gimmick->GetState() != GIMMICK_STATE_DIE) {
			gimmick->SetPosition(target_x, target_y);
			if (gimmick->y > 300 && gimmick->x > 1280)
			{
				gimmick->vx = 0.1f;
				gimmick->inSewer = true;
				gimmick->YSewer = 0;
				gimmick->XSewer = gimmick->x + 16;
				gimmick->nSewer = 1;
			}
			else if(gimmick->y > 300 && gimmick->x < 1280)
			{
				gimmick->vx = -0.1f;
				gimmick->inSewer = true;
				gimmick->YSewer = 0;
				gimmick->XSewer = gimmick->x - 16;
				gimmick->nSewer = -1;
			}
			else if (gimmick->y < 300 && gimmick->x < 1280)
			{
				gimmick->vy = -0.1f;
				gimmick->equalinSewer = false;
				gimmick->inSewer = true;
				gimmick->YSewer = gimmick->y -16;
				gimmick->XSewer = 0;
				gimmick->nSewer = -1;
			}
			else if (gimmick->y < 300 && gimmick->x > 1280)
			{
				gimmick->vy = -0.1f;
				gimmick->equalinSewer = false;
				gimmick->inSewer = true;
				gimmick->YSewer = gimmick->y - 16;
				gimmick->XSewer = 0;
				gimmick->nSewer = -1;
			}
		}
	}
}

void CPortal::Render()
{
	//RenderBoundingBox();
}

void CPortal::SetState(int state) {}

void CPortal::GetBoundingBox(float& l, float& t, float& r, float& b)
{
	l = x;
	t = y;
	r = l + PORTAL_BBOX_WIDTH;
	b = t - PORTAL_BBOX_HEIGHT;
}
#include "FinalBossBigBullet.h"
#include "Utils.h"

CFinalBossBigBullet::CFinalBossBigBullet(int direction)
{
	if (direction == -1)
		SetState(FINALBOSS_BIG_BULLET_STATE_LEFT);
	else
		SetState(FINALBOSS_BIG_BULLET_STATE_RIGHT);
}

void CFinalBossBigBullet::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	if (!CGame::GetInstance()->InCamera(this) && CGame::GetInstance()->InLargeCamera(this) && this->visible == true) {
		this->visible = false;
	}
	if (!visible)
		return;
	DetectStar();
	SpecialCollisionWithPlayer();

	CGameObject::Update(dt, coObjects);

	x += dx;
	y += dy;
}

void CFinalBossBigBullet::Render()
{
	if (!visible)
		return;

	int ani = 0;
	if (state == FINALBOSS_BIG_BULLET_STATE_LEFT) { //==============WALK
		ani = FINALBOSS_BIG_BULLET_ANI_LEFT;
	}
	else if (state == FINALBOSS_BIG_BULLET_STATE_RIGHT) {
		ani = FINALBOSS_BIG_BULLET_ANI_RIGHT;
	}
	animation_set->at(ani)->Render(x, y );

	//RenderBoundingBox();
}

void CFinalBossBigBullet::SetState(int state)
{
	CGameObject::SetState(state);
	switch (state)
	{
	case FINALBOSS_BIG_BULLET_STATE_LEFT:
		vx = -FINALBOSS_BIG_BULLET_SPEED; // moi vo di sang Phai
		break;
	case FINALBOSS_BIG_BULLET_STATE_RIGHT:
		vx = FINALBOSS_BIG_BULLET_SPEED; // moi vo di sang Phai
		break;

	}
}

void CFinalBossBigBullet::GetBoundingBox(float& l, float& t, float& r, float& b)
{
	l = x;
	t = y;
	r = x + FINALBOSS_BIG_BULLET_BBOX_WIDTH;
	b = y - FINALBOSS_BIG_BULLET_BBOX_HEIGHT;
}

void CFinalBossBigBullet::SpecialCollisionWithPlayer()
{
	CScene* scene = CGame::GetInstance()->GetCurrentScene();
	CGimmick* player = ((CPlayScene*)scene)->GetPlayer();

	if (player->state != GIMMICK_STATE_DIE && player->getUntouchable() != 1)
	{
		if (CheckAABB(player))
		{
			if (x < player->x)
			{
				//player->vx = GIMMICK_DEFLECT_SPEED_X;
				//player->nx = 1.0;
			}
			else
			{
				//player->vx = -GIMMICK_DEFLECT_SPEED_X;
				//player->nx = -1.0;
			}

			player->SetState(GIMMICK_STATE_STUN);
			player->StartUntouchable();
		}
	}
}

void CFinalBossBigBullet::DetectStar()
{
	CScene* scene = CGame::GetInstance()->GetCurrentScene();
	CStar* star = ((CPlayScene*)scene)->GetStar();

	if (CheckAABB(star) && (star->state == STAR_STATE_WALKING_LEFT || star->state == STAR_STATE_WALKING_RIGHT))
	{
		star->SetState(STAR_STATE_EXPLOSIVE);
	}
}

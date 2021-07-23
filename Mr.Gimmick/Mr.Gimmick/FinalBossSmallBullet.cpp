#include "FinalBossSmallBullet.h"
#include "Utils.h"

CFinalBossSmallBullet::CFinalBossSmallBullet(float init_vx, float init_vy)
{
	vx = init_vx* SMALLBULLET_SPEED_X;
	vy = init_vy* SMALLBULLET_SPEED_Y;
	SetState(SMALLBULLET_STATE_FLY);
}

void CFinalBossSmallBullet::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	CGameObject::Update(dt, coObjects);

	if (state == SMALLBULLET_STATE_EXPLOSE && GetTickCount64() - explose_start >= 150)
	{
		visible = false;
		explose_start = 0;
	}
	if (!visible)
		return;

	DetectStar();
	SpecialCollisionWithPlayer();

	vector<LPGAMEOBJECT> newCoObjects;
	for (UINT i = 0; i < coObjects->size(); i++)
	{
		if (dynamic_cast<CBrick*>(coObjects->at(i))) newCoObjects.push_back(coObjects->at(i));
	}

	vector<LPCOLLISIONEVENT> coEvents;
	vector<LPCOLLISIONEVENT> coEventsResult;

	coEvents.clear();

	CalcPotentialCollisions(&newCoObjects, coEvents);

	if (coEvents.size() == 0)
	{
		x += dx;
		y += dy;
	}
	else
	{
		float min_tx, min_ty, nx = 0, ny;
		float rdx = 0;
		float rdy = 0;

		float x0 = x;
		float y0 = y;

		FilterCollision(coEvents, coEventsResult, min_tx, min_ty, nx, ny, rdx, rdy);

		for (UINT i = 0; i < coEventsResult.size(); i++)
		{
			LPCOLLISIONEVENT e = coEventsResult[i];

			if (dynamic_cast<CBrick*>(e->obj))
			{
				CBrick* brick = dynamic_cast<CBrick*>(e->obj);
				x = x0 + min_tx * dx + nx * 0.1f;
				y = y0 + min_ty * dy + ny * 0.1f;
				if(state == SMALLBULLET_STATE_FLY)
					SetState(SMALLBULLET_STATE_EXPLOSE);
			}
		}
	}
	// clean up newCoObjects
	for (UINT i = 0; i < newCoObjects.size(); i++) newCoObjects[i] = nullptr;
	// clean up collision events
	for (UINT i = 0; i < coEvents.size(); i++) delete coEvents[i];
}

void CFinalBossSmallBullet::Render()
{
	if (!visible) return;

	int ani = 0;
	if (state == SWORD_STATE_FLY) {
		ani = SWORD_ANI_FLY;
	}
	else
		ani = SWORD_ANI_ONGROUND;

	animation_set->at(ani)->Render(x, y + 9);

	//RenderBoundingBox();
}

void CFinalBossSmallBullet::SetState(int state)
{
	CGameObject::SetState(state);
	switch (state)
	{
	case SMALLBULLET_STATE_FLY:
		break;
	case SMALLBULLET_STATE_EXPLOSE:
		vx = 0;
		vy = 0;
		explose_start = GetTickCount64();
		break;
	}
}

void CFinalBossSmallBullet::GetBoundingBox(float& l, float& t, float& r, float& b)
{
	l = x;
	t = y;
	r = x + SMALLBULLET_BBOX_WIDTH;
	b = y - SMALLBULLET_BBOX_HEIGHT;
}

void CFinalBossSmallBullet::SpecialCollisionWithPlayer()
{
	CScene* scene = CGame::GetInstance()->GetCurrentScene();
	CGimmick* player = ((CPlayScene*)scene)->GetPlayer();

	if (player->state != GIMMICK_STATE_DIE && player->getUntouchable() != 1
		&& state == SMALLBULLET_STATE_FLY)
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

void CFinalBossSmallBullet::DetectStar()
{
	CScene* scene = CGame::GetInstance()->GetCurrentScene();
	CStar* star = ((CPlayScene*)scene)->GetStar();

	if (state == SMALLBULLET_STATE_FLY)
	{
		if (CheckAABB(star) && (star->state == STAR_STATE_WALKING_LEFT || star->state == STAR_STATE_WALKING_RIGHT))
		{
			star->SetState(STAR_STATE_EXPLOSIVE);

			SetState(SMALLBULLET_STATE_EXPLOSE);
		}
	}
}

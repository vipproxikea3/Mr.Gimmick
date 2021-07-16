#include "Bullet.h"
#include "Brick.h"
#include "InclinedBrick.h"
#include "BlueFire.h"
#include "Gun.h"
#include "Gimmick.h"
#include "Star.h"

void CBullet::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	if (this->GetState() == BULLET_STATE_EXPLODE && !CGame::GetInstance()->InCamera(this) && CGame::GetInstance()->InLargeCamera(this) && this->visible == true) {
		this->visible = false;
	}
	if (!visible)
		return;
	if (state != BULLET_STATE_EXPLODE) 
	{
		CGimmick* gimmick = ((CPlayScene*)CGame::GetInstance()->GetCurrentScene())->GetPlayer();
		CStar* star = ((CPlayScene*)CGame::GetInstance()->GetCurrentScene())->GetStar();

		if (this->CheckAABB(star) && this->state != BULLET_STATE_EXPLODE && (star->GetState() == STAR_STATE_WALKING_LEFT || star->GetState() == STAR_STATE_WALKING_RIGHT))
			this->SetState(BULLET_STATE_EXPLODE);

		if (this->CheckAABB(gimmick) && this->state != BULLET_STATE_EXPLODE)
			gimmick->SetState(GIMMICK_STATE_STUN);

		vy -= GIMMICK_GRAVITY * dt;
		CGameObject::Update(dt);


		onInclinedBrick = false;
		int direction = 0;

		vector<LPGAMEOBJECT> newCoObjects;
		for (UINT i = 0; i < coObjects->size(); i++)
		{
			if (dynamic_cast<CInclinedBrick*>(coObjects->at(i))) 
			{
				CInclinedBrick* brick = dynamic_cast<CInclinedBrick*>(coObjects->at(i));
				int tmp = brick->Collision(this, dy);
				if (tmp != 0)
					direction = tmp;
			}

			if (dynamic_cast<CBrick*>(coObjects->at(i))) newCoObjects.push_back(coObjects->at(i));
			if (dynamic_cast<CBlueFire*>(coObjects->at(i))) newCoObjects.push_back(coObjects->at(i));

		}

		if (onInclinedBrick == true && direction != 0)
		{
			if (direction == -1)
			{
				this->x -= 0.5;
				if (vx > -0.1) 
				{
					vx -= 0.01;
				}
			}
			if (direction == 1)
			{
				this->x += 0.5;
				if (vx < 0.1) 
				{
					vx += 0.01;
				}
			}
			if (direction == -2) 
			{
				this->x -= 1;
			}
			if (direction == 2) 
			{
				this->x += 1;
			}
		}

		vector<LPCOLLISIONEVENT> coEvents;
		vector<LPCOLLISIONEVENT> coEventsResult;

		coEvents.clear();
		CalcPotentialCollisions(&newCoObjects, coEvents);

		if (coEvents.size() == 0)
		{
			x += dx;
			if (!onInclinedBrick)
				y += dy;
		}
		else
		{
			float min_tx, min_ty, nx, ny;
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
					x = x0 + min_tx * dx + nx * 0.1f;
					if (onInclinedBrick) x = x0 + dx;
					y = y0 + min_ty * dy + ny * 0.1f;


					if (e->ny != 0) vy = 0;

				}
			}

			for (UINT i = 0; i < coEvents.size(); i++)
			{
				LPCOLLISIONEVENT e = coEvents[i];

				if (dynamic_cast<CBlueFire*>(e->obj))
				{
					SetState(BULLET_STATE_EXPLODE);
				}
			}

		}

		timeDelete -= dt;
		if (timeDelete <= 0)
		{
			SetState(BULLET_STATE_EXPLODE);
		}
	}
	else {
		timePreDelete -= dt;
		if (timePreDelete <= 0)
		{
			isDelete = true;
			visible = false;
		}
	}

}

void CBullet::SetState(int state)
{
	CGameObject::SetState(state);

	switch (state)
	{
	case BULLET_STATE_MOVE:
		nx = 1;
		break;
	case BULLET_STATE_EXPLODE:
		vx = 0;
		break;
	}
}

void CBullet::Render()
{
	if (!visible)
		return;

	int ani = -1;
	if (state == BULLET_STATE_MOVE)
	{
		ani = BULLET_ANI_MOVE;
	}
	else
	{
		ani = BULLET_ANI_EXPLODE;
	}

	int alpha = 255;
	animation_set->at(ani)->Render(x, y, alpha);

}
void CBullet::GetBoundingBox(float& l, float& t, float& r, float& b)
{
	l = x;
	t = y;
	r = l + BULLET_BOX_WIDTH;
	b = t - BULLET_BOX_HEIGHT;
}

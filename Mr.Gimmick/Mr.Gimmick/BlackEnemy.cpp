#include "BlackEnemy.h"
#include "Utils.h"
#include "InclinedBrick.h"
CBlackEnemy::CBlackEnemy()
{
	SetState(BLACKENEMY_STATE_WALK);
}

void CBlackEnemy::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	CGameObject::Update(dt, coObjects);

	vy -= BLACKENEMY_GRAVITY * dt;

	onGround = false;
	for (UINT i = 0; i < coObjects->size(); i++)
	{
		if (dynamic_cast<CInclinedBrick*>(coObjects->at(i))) {
			CInclinedBrick* brick = dynamic_cast<CInclinedBrick*>(coObjects->at(i));
			brick->Collision(this, dy);
		}
		if (dynamic_cast<CBrick*>(coObjects->at(i))) {
			CBrick* brick = dynamic_cast<CBrick*>(coObjects->at(i));
			if (onTopOf(brick)) this->onGround = true;
		}
	}

	vector<LPCOLLISIONEVENT> coEvents;
	vector<LPCOLLISIONEVENT> coEventsResult;

	coEvents.clear();

	if (state != BLACKENEMY_STATE_DIE)
		CalcPotentialCollisions(coObjects, coEvents);

	if (coEvents.size() == 0)
	{
		x += dx;
		if (!onInclinedBrick)
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

		x = x0 + min_tx * dx + nx * 0.3f;
		y = y0 + min_ty * dy + ny * 0.3f;

		float l, t, r, b;
		GetBoundingBox(l, t, r, b);
		for (UINT i = 0; i < coEventsResult.size(); i++)
		{
			LPCOLLISIONEVENT e = coEventsResult[i];
			float ol, ot, or , ob;
			e->obj->GetBoundingBox(ol, ot, or , ob);

			if (e->ny > 0) onGround = true;

			if (dynamic_cast<CBrick*>(e->obj))
			{
				CBrick *brick = dynamic_cast<CBrick*>(e->obj);
				if (onInclinedBrick) x = x0 + dx;
				if (e->nx != 0)
				{
					//x = x0 + min_tx * dx + nx * 5.0f;
					if (this->onGround)
						Jump();
					/*else
						vx = -vx;*/
				}
				if (e->ny > 0)
				{
					vy = 0;
					if ((l <= ol && vx < 0) || (r >= or && vx > 0)) Jump();
				}
				if (e->ny < 0)
					vy = -BLACKENEMY_GRAVITY;
			}
		}
	}
}

void CBlackEnemy::Render()
{
	int ani = 0;
	if (state == BLACKENEMY_STATE_WALK) {
		if (vx > 0)
			ani = BLACKENEMY_ANI_WALK_RIGHT;
		else
			ani = BLACKENEMY_ANI_WALK_LEFT;
	}

	animation_set->at(ani)->Render(x, y);

	//RenderBoundingBox();
}

void CBlackEnemy::SetState(int state)
{
	CGameObject::SetState(state);
	switch (state)
	{
	case BLACKENEMY_STATE_WALK:
		vx = BLACKENEMY_WALK_SPEED;
		break;
	}
}

void CBlackEnemy::GetBoundingBox(float& l, float& t, float& r, float& b)
{
	l = x;
	t = y;
	r = x + BLACKENEMY_WIDTH;
	b = y - BLACKENEMY_HEIGHT;
}

void CBlackEnemy::Jump()
{
	vy = BLACKENEMY_JUMP_SPEED;
}

bool CBlackEnemy::onTopOf(CGameObject* object)
{
	float ol, ot, or , ob;
	object->GetBoundingBox(ol, ot, or , ob);
	float l, t, r, b;
	GetBoundingBox(l, t, r, b);
	if (r >= ol && l <= or && abs(b - ot) < 1)
		return true;
	return false;
}
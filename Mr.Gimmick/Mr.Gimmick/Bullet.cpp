#include "Bullet.h"
#include "Brick.h"
#include "InclinedBrick.h"
#include "BlueFire.h"
#include "Gun.h"


void CBullet::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{

	vy -= BULLET_GRAVITY * dt;
	CGameObject::Update(dt);

	onGround = false;
	onInclinedBrick = false;
	int direction = -1;  //trai false, phai true

	vector<LPGAMEOBJECT> newCoObjects;
	for (UINT i = 0; i < coObjects->size(); i++)
	{
		if (dynamic_cast<CBrick*>(coObjects->at(i))) newCoObjects.push_back(coObjects->at(i));
		if (dynamic_cast<CInclinedBrick*>(coObjects->at(i))) {
			CInclinedBrick* brick = dynamic_cast<CInclinedBrick*>(coObjects->at(i));
			direction = brick->Collision(this, dy);

		}
		if (dynamic_cast<CGun*>(coObjects->at(i))) newCoObjects.push_back(coObjects->at(i));
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
		float min_tx, min_ty, nx = 0, ny;
		float rdx = 0;
		float rdy = 0;

		float x0 = x;
		float y0 = y;


		FilterCollision(coEvents, coEventsResult, min_tx, min_ty, nx, ny, rdx, rdy);

		for (UINT i = 0; i < coEventsResult.size(); i++)
		{
			LPCOLLISIONEVENT e = coEventsResult[i];

			if (dynamic_cast<CBrick*>(e->obj)) {
				x = x0 + min_tx * dx + nx * 0.1f;
				if (onInclinedBrick) {
					x = x0 + dx;
				}
				y = y0 + min_ty * dy + ny * 0.1f;

				if (e->nx != 0) vx = 0;
				if (e->ny != 0) vy = 0;

				if (e->ny == 1)
					this->onGround = true;
			}

			if (dynamic_cast<CGun*>(e->obj)) {
				x = x0 + min_tx * dx + nx * 0.1f;
				if (onInclinedBrick) {
					x = x0 + dx;
				}
				y = y0 + min_ty * dy + ny * 0.1f;

				if (e->nx != 0) vx = -vx;
				if (e->ny != 0) vy = 0;

				if (e->ny == 1)
					this->onGround = true;
			}

		}
	}

	for (UINT i = 0; i < newCoObjects.size(); i++) newCoObjects[i] = nullptr;
	for (UINT i = 0; i < coEvents.size(); i++) delete coEvents[i];

	//DebugOut(L"Direct %d\n", direction);
	if (onInclinedBrick)
		if (direction == 0)
			vx = vx <= -0.1 ? -0.1 : vx -= 0.001;
		else if (direction == 1)
			vx = vx >= 0.1 ? 0.1 : vx += 0.001;

	/*timeDelete -= dt;
	if (timeDelete <= 0)
		isDelete = true;*/
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
	//RenderBoundingBox();
}
void CBullet::GetBoundingBox(float& l, float& t, float& r, float& b)
{
	l = x;
	t = y;
	r = l + BULLET_BOX_WIDTH;
	b = t - BULLET_BOX_HEIGHT;
}

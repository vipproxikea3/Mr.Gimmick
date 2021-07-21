#include "Sword.h"
#include "Utils.h"

CSword::CSword(float init_x, float init_y)
{
	x = init_x;
	y = init_y;
	start_y = y;
	SetState(SWORD_STATE_FLY);
}

void CSword::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	CGameObject::Update(dt, coObjects);

	if (state == SWORD_STATE_FLY && vy > 0 && y >= start_y + SWORD_FLY_RANGE_Y)
		vy = - SWORD_FLY_SPEED_Y;

	vector<LPGAMEOBJECT> newCoObjects;
	for (UINT i = 0; i < coObjects->size(); i++)
	{
		if (dynamic_cast<CBrick*>(coObjects->at(i))) newCoObjects.push_back(coObjects->at(i));
	}

	vector<LPCOLLISIONEVENT> coEvents;
	vector<LPCOLLISIONEVENT> coEventsResult;

	coEvents.clear();

	//if (state != GREENBOSS_STATE_DIE)
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
				SetState(SWORD_STATE_ONGROUND);
			}
		}
	}
	// clean up newCoObjects
	for (UINT i = 0; i < newCoObjects.size(); i++) newCoObjects[i] = nullptr;
	// clean up collision events
	for (UINT i = 0; i < coEvents.size(); i++) delete coEvents[i];
	DebugOut(L"%d", start_y);
}

void CSword::Render()
{
	int ani = 0;
	if (state == SWORD_STATE_FLY) {
		ani = SWORD_ANI_FLY;
	}
	else
		ani = SWORD_ANI_ONGROUND;

	animation_set->at(ani)->Render(x, y );

	//RenderBoundingBox();
}

void CSword::SetState(int state)
{
	CGameObject::SetState(state);
	switch (state)
	{
	case SWORD_STATE_FLY:
		vy = SWORD_FLY_SPEED_Y;
		vx = SWORD_FLY_SPEED_X;
		break;
	case SWORD_STATE_ONGROUND:
		vx = 0;
		vy = 0;
		break;
	}
}

void CSword::GetBoundingBox(float& l, float& t, float& r, float& b)
{
	l = x;
	t = y;
	r = x + SWORD_BBOX_WIDTH;
	b = y - SWORD_BBOX_HEIGHT;
}
#include "Worm.h"
#include "Brick.h"
#include "Conveyor.h"
#include "InclinedBrick.h"
#include "Utils.h"
#include "Gimmick.h"

CWorm::CWorm() : CGameObject()
{
	
	SetState(WORM_STATE_WALKING_RIGHT);
}

CWorm::CWorm(int l) : CGameObject()
{
	length = l;
	SetState(WORM_STATE_WALKING_RIGHT);
}

void CWorm::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	if (firstLocation)
	{
		float tmp = y;
		GetPosition(leftLimit, tmp);
		rightLimit = leftLimit + (length - 1) * 16;
		firstLocation = false;
	}
	// Calculate dx, dy 
	CGameObject::Update(dt);

	// Simple fall down
	vy -= WORM_GRAVITY * dt;

	vector<LPCOLLISIONEVENT> coEvents;
	vector<LPCOLLISIONEVENT> coEventsResult;

	coEvents.clear();

	// turn off collision when die 
	if (state != WORM_STATE_DIE)
		CalcPotentialCollisions(coObjects, coEvents);

	float mLeft, mTop, mRight, mBottom;
	float oLeft, oTop, oRight, oBottom;

	if (coEvents.size() == 0)
	{
		if (x >= (rightLimit) || x <= (leftLimit))
		{
			vx = -vx;
			x -= dx;
		}
		else
		{
			x += dx;
		}
		y += dy;
	}
	else
	{
		float min_tx, min_ty, nx = 0, ny = 0;
		float rdx = 0;
		float rdy = 0;

		float x0 = x;
		float y0 = y;
		/*if (x >= (rightLimit) || x <= (leftLimit))
		{
			x = x0 - dx;
		}
		else
		{
			x = x0 + dx;
		}*/
		x = x0 + dx;
		y = y0 + dy;
		FilterCollision(coEvents, coEventsResult, min_tx, min_ty, nx, ny, rdx, rdy);

		for (UINT i = 0; i < coEventsResult.size(); i++)
		{
			LPCOLLISIONEVENT e = coEventsResult[i];
			GetBoundingBox(mLeft, mTop, mRight, mBottom);
			e->obj->GetBoundingBox(oLeft, oTop, oRight, oBottom);

			if (dynamic_cast<CBrick*>(e->obj)) 
			{
				x = x0 + min_tx * dx + nx * 0.1f;
				y = y0 + min_ty * dy + ny * 0.1f;
				if (e->nx != 0)
				{	
					if (ceil(mBottom) != oTop)
					{
						if (x >= rightLimit || x <= leftLimit)
							vx = -vx;
					}
						
				}
				if (e->ny != 0)
				{
					vy = 0;
				}
			}
		}
	}

	// clean up collision events
	for (UINT i = 0; i < coEvents.size(); i++) delete coEvents[i];
}

void CWorm::Render()
{
	int ani = -1;
	if (state == WORM_STATE_DIE)
	{
		if (nx > 0)
			ani = WORM_ANI_DIE_RIGHT;
		else
			ani = WORM_ANI_DIE_LEFT;
	}
	else
	{
		if (vx > 0)
		{
			ani = WORM_ANI_RIGHT;
		}
		else
		{
			ani = WORM_ANI_LEFT;
		}
	}

	int alpha = 255;
	animation_set->at(ani)->Render(x, y, alpha);
	RenderBoundingBox();
}

void CWorm::SetState(int state)
{
	CGameObject::SetState(state);

	switch (state)
	{
	case WORM_STATE_WALKING_RIGHT:
		vx = WORM_WALKING_SPEED;
		nx = 1;
		break;
	case WORM_STATE_WALKING_LEFT:
		vx = -WORM_WALKING_SPEED;
		nx = -1;
		break;
	case WORM_STATE_DIE:
		vx = 0;
		break;
	}
}

void CWorm::GetBoundingBox(float& l, float& t, float& r, float& b)
{
	l = x;
	t = y;
	r = l + WORM_BBOX_WIDTH;
	b = t - WORM_BBOX_HEIGHT;
}
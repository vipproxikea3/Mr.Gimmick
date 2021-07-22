#include "EnemyTail.h"
#include "Brick.h"

CEnemyTail::CEnemyTail(float left, float right) : CGameObject()
{
	this->SetState(ENEMYTAIL_STATE_WALKING_RIGHT);
	this->left = left;
	this->right = right;
}

void CEnemyTail::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
	left = x;
	top = y;
	right = left + ENEMYTAIL_BBOX_WIDTH;
	bottom = top - ENEMYTAIL_BBOX_HEIGHT;
}

void CEnemyTail::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	// Calculate dx, dy 
	CGameObject::Update(dt);

	// Simple fall down
	vy -= ENEMYTAIL_GRAVITY * dt;

	float x0 = x;
	float y0 = y;

	x = x0 + dx;
	y = y0 + dy;

	vector<LPCOLLISIONEVENT> coEvents;
	vector<LPCOLLISIONEVENT> coEventsResult;

	coEvents.clear();

	CalcPotentialCollisions(coObjects, coEvents);

	if (coEvents.size() == 0)
	{
	}
	else
	{
		float min_tx, min_ty, nx = 0, ny = 0;
		float rdx = 0;
		float rdy = 0;

		FilterCollision(coEvents, coEventsResult, min_tx, min_ty, nx, ny, rdx, rdy);

		for (UINT i = 0; i < coEventsResult.size(); i++)
		{
			LPCOLLISIONEVENT e = coEventsResult[i];

			if (dynamic_cast<CBrick*>(e->obj))
			{
				this->y = y0 + min_ty * dy + e->ny * 0.2f;
				this->x = x0 + min_tx * dx + e->nx * 0.2f;

				if (e->nx > 0 && this->state == ENEMYTAIL_STATE_WALKING_LEFT)
					this->SetState(ENEMYTAIL_STATE_WALKING_RIGHT);
				if (e->nx < 0 && this->state == ENEMYTAIL_STATE_WALKING_RIGHT)
					this->SetState(ENEMYTAIL_STATE_WALKING_LEFT);

				if (e->ny != 0)
					this->vy = 0;
			}
		}
	}

	if (this->x < this->left && this->state == ENEMYTAIL_STATE_WALKING_LEFT)
		this->SetState(ENEMYTAIL_STATE_WALKING_RIGHT);
	if (this->x + ENEMYTAIL_BBOX_WIDTH > this->right && this->state == ENEMYTAIL_STATE_WALKING_RIGHT)
		this->SetState(ENEMYTAIL_STATE_WALKING_LEFT);

	// clean up collision events
	for (UINT i = 0; i < coEvents.size(); i++) delete coEvents[i];
}

void CEnemyTail::Render()
{
	int alpha = 255;

	switch (this->GetState())
	{
	case ENEMYTAIL_STATE_WALKING_RIGHT:
		animation_set->at(ENEMYTAIL_ANI_WALKING_RIGHT)->Render(x - 6, y, alpha);
		break;
	case ENEMYTAIL_STATE_WALKING_LEFT:
		animation_set->at(ENEMYTAIL_ANI_WALKING_LEFT)->Render(x, y, alpha);
		break;
	default:
		animation_set->at(ENEMYTAIL_ANI_WALKING_RIGHT)->Render(x - 6, y, alpha);
		break;
	}

	
	//RenderBoundingBox();
}

void CEnemyTail::SetState(int state)
{
	CGameObject::SetState(state);

	switch (state)
	{
	case ENEMYTAIL_STATE_WALKING_RIGHT:
		this->vx = ENEMYTAIL_WALKING_SPEED;
		break;
	case ENEMYTAIL_STATE_WALKING_LEFT:
		this->vx = -ENEMYTAIL_WALKING_SPEED;
		break;
	}
}
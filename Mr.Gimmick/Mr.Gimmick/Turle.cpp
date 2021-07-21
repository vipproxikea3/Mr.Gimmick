#include "Turle.h"
#include "Brick.h"
#include "Utils.h"
#include "Game.h"
#include "PlayScene.h"
#include "Star.h"
#include "Gimmick.h"

CTurle::CTurle(float left, float right) : CGameObject()
{
	this->SetState(TURLE_STATE_WALKING_RIGHT);
	this->left = left;
	this->right = right;
}

void CTurle::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
	if (state == TURLE_STATE_WALKING_RIGHT
		|| state == TURLE_STATE_WALKING_LEFT) {
		left = x;
		top = y;
		right = left + TURLE_WALKING_BBOX_WIDTH;
		bottom = top - TURLE_WALKING_BBOX_HEIGHT;
	}
	if (state == TURLE_STATE_TURNING_RIGHT
		|| state == TURLE_STATE_TURNING_LEFT) {
		left = x;
		top = y;
		right = left + TURLE_TURNING_BBOX_WIDTH;
		bottom = top - TURLE_TURNING_BBOX_HEIGHT;
	}
	if (state == TURLE_STATE_DIE_RIGHT
		|| state == TURLE_STATE_DIE_LEFT) {
		left = x;
		top = y;
		right = left + TURLE_DIE_BBOX_WIDTH;
		bottom = top - TURLE_DIE_BBOX_HEIGHT;
	}
	if (state == TURLE_STATE_DIE_COMPLETE) {
		left = x;
		top = y;
		right = left + TURLE_DIE_BBOX_WIDTH;
		bottom = top - TURLE_DIE_BBOX_HEIGHT;
	}
}

void CTurle::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	CScene* scene = CGame::GetInstance()->GetCurrentScene();
	CStar* star = ((CPlayScene*)scene)->GetStar();

	if (CheckAABB(star) && this->GetState() != TURLE_STATE_DIE_COMPLETE && star->state != STAR_STATE_HIDE
		&& star->state != STAR_STATE_EXPLOSIVE && star->state != STAR_STATE_PENDING && star->state != STAR_STATE_READY)
	{
		if (this->state == TURLE_STATE_WALKING_RIGHT)
			this->SetState(TURLE_STATE_DIE_RIGHT);
		else if(this->state == TURLE_STATE_WALKING_LEFT)
			this->SetState(TURLE_STATE_DIE_LEFT);
	}

	/*if (CheckAABB(star) && (this->GetState() == TURLE_STATE_DIE_RIGHT || this->GetState() == TURLE_STATE_DIE_LEFT) && this->GetState() != TURLE_STATE_DIE_COMPLETE && star->state != STAR_STATE_HIDE
		&& star->state != STAR_STATE_EXPLOSIVE && star->state != STAR_STATE_PENDING && star->state != STAR_STATE_READY)
	{
		this->SetState(TURLE_STATE_DIE_COMPLETE);
	}*/

	CGimmick* player = ((CPlayScene*)scene)->GetPlayer();

	if (CheckAABB(player) && (this->state == TURLE_STATE_WALKING_RIGHT || this->state == TURLE_STATE_WALKING_LEFT))
	{
		player->SetState(GIMMICK_STATE_STUN);
	}

	// Calculate dx, dy 
	CGameObject::Update(dt);

	// Simple fall down
	vy -= TURLE_GRAVITY * dt;

	float x0 = x;
	float y0 = y;

	x = x0 + dx;
	y = y0 + dy;

	if (this->state == TURLE_STATE_DIE_COMPLETE)
		return;

	vector<LPCOLLISIONEVENT> coEvents;
	vector<LPCOLLISIONEVENT> coEventsResult;

	coEvents.clear();

	CalcPotentialCollisions(coObjects, coEvents);

	DebugOut(L"state = %d\n", state);

	if (coEvents.size() == 0)
	{
		/*x = x0 + dx;
		y = y0 + dy;*/
	}
	else
	{
		float min_tx, min_ty, nx = 0, ny = 0;
		float rdx = 0;
		float rdy = 0;

		/*x = x0 + dx;
		y = y0 + dy;*/

		FilterCollision(coEvents, coEventsResult, min_tx, min_ty, nx, ny, rdx, rdy);

		for (UINT i = 0; i < coEventsResult.size(); i++)
		{
			LPCOLLISIONEVENT e = coEventsResult[i];

			if (dynamic_cast<CBrick*>(e->obj))
			{
				this->y = y0 + min_ty * dy + e->ny * 0.2f;
				this->x = x0 + min_tx * dx + e->nx * 0.2f;				

				if (e->nx > 0 && this->state == TURLE_STATE_WALKING_LEFT)
					this->SetState(TURLE_STATE_WALKING_RIGHT);
				if (e->nx < 0 && this->state == TURLE_STATE_WALKING_RIGHT)
					this->SetState(TURLE_STATE_WALKING_LEFT);
				
				if (e->ny != 0) {
					this->vy = 0;
					if (state == TURLE_STATE_DIE_RIGHT || state == TURLE_STATE_DIE_LEFT)
						this->vx = 0;
				}
					
			}
		}
	}

	if (this->x < this->left + 1 && this->state == TURLE_STATE_WALKING_LEFT)
		this->SetState(TURLE_STATE_WALKING_RIGHT);
	if (this->x + TURLE_WALKING_BBOX_WIDTH > this->right - 1 && this->state == TURLE_STATE_WALKING_RIGHT)
		this->SetState(TURLE_STATE_WALKING_LEFT);

	// clean up collision events
	for (UINT i = 0; i < coEvents.size(); i++) delete coEvents[i];
}

void CTurle::Render()
{
	int alpha = 255;
	int ani = -1;

	switch (this->GetState())
	{
	case TURLE_STATE_WALKING_RIGHT:
		ani = TURLE_ANI_WALKING_RIGHT;
		break;
	case TURLE_STATE_WALKING_LEFT:
		ani = TURLE_ANI_WALKING_LEFT;
		break;
	case TURLE_STATE_DIE_RIGHT:
		//ani = TURLE_ANI_DIE_RIGHT
		ani = TURLE_ANI_DIE_LEFT;;
		break;
	case TURLE_STATE_DIE_LEFT:
		//ani = TURLE_ANI_DIE_LEFT;
		ani = TURLE_ANI_DIE_RIGHT;
		break;
	}

	animation_set->at(ani)->Render(x, y, alpha);
	RenderBoundingBox();
}

void CTurle::SetState(int state)
{
	CGameObject::SetState(state);

	switch (state)
	{
	case TURLE_STATE_WALKING_RIGHT:
		this->vx = TURLE_WALKING_SPEED;
		break;
	case TURLE_STATE_WALKING_LEFT:
		this->vx = -TURLE_WALKING_SPEED;
		break;
	case TURLE_STATE_DIE_RIGHT:
		this->vx = -0.01f;
		this->vy = TURLE_DIE_SPEED;
		break;
	case TURLE_STATE_DIE_LEFT:
		this->vx = 0.01f;
		this->vy = TURLE_DIE_SPEED;
		break;
	}
}
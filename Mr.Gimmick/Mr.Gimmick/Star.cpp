#include "Star.h"
#include "Utils.h"
#include "InclinedBrick.h"
#include "Game.h"
#include "Conveyor.h"
#include "Gimmick.h"
#include "Game.h"
#include "PlayScene.h"

CStar::CStar() : CGameObject()
{
	this->state = STAR_STATE_HIDE;
	explosive_start = NULL;
}

void CStar::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
	left = x + 0.5f;
	top = y - 0.5f;
	right = left + STAR_BBOX_WIDTH - 0.5f;
	bottom = top - STAR_BBOX_HEIGHT + 0.5f;
}

void CStar::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	if (state == STAR_STATE_HIDE) {

		vx = 0;
		vy = 0;

		star_small_1_x = STAR_SMALL_1_X0;
		star_small_1_y = STAR_SMALL_1_Y0;

		star_small_2_x = STAR_SMALL_2_X0;
		star_small_2_y = STAR_SMALL_2_Y0;

		star_small_3_x = STAR_SMALL_3_X0;
		star_small_3_y = STAR_SMALL_3_Y0;

		star_small_4_x = STAR_SMALL_4_X0;
		star_small_4_y = STAR_SMALL_4_Y0;

		star_small_5_x = STAR_SMALL_5_X0;
		star_small_5_y = STAR_SMALL_5_Y0;
	}
	else if (state == STAR_STATE_PENDING) {
		star_small_1_x += STAR_SMALL_1_VX * STAR_SMALL_V * dt;
		star_small_1_y += STAR_SMALL_1_VY * STAR_SMALL_V * dt;

		star_small_2_x += STAR_SMALL_2_VX * STAR_SMALL_V * dt;
		star_small_2_y += STAR_SMALL_2_VY * STAR_SMALL_V * dt;

		star_small_3_x += STAR_SMALL_3_VX * STAR_SMALL_V * dt;
		star_small_3_y += STAR_SMALL_3_VY * STAR_SMALL_V * dt;

		star_small_4_x += STAR_SMALL_4_VX * STAR_SMALL_V * dt;
		star_small_4_y += STAR_SMALL_4_VY * STAR_SMALL_V * dt;

		star_small_5_x += STAR_SMALL_5_VX * STAR_SMALL_V * dt;
		star_small_5_y += STAR_SMALL_5_VY * STAR_SMALL_V * dt;

		if (star_small_1_y <= 0) {
			this->SetState(STAR_STATE_READY);
		}
	}
	else if (state == STAR_STATE_WALKING_RIGHT || state == STAR_STATE_WALKING_LEFT || state == STAR_STATE_EXPLOSIVE) {

		if (vx == 0 && vy == 0 && (state != STAR_STATE_EXPLOSIVE))
			this->SetState(STAR_STATE_EXPLOSIVE);

		// Calculate dx, dy 
		CGameObject::Update(dt);

		// cal vx
		if (vx > 0) {
			vx -= STAR_WALKING_FRICTION;
			if (vx <= 0)
				vx = 0;
		} else if (vx < 0) {
			vx += STAR_WALKING_FRICTION;
			if (vx >= 0)
				vx = 0;
		}

		// Simple fall down
		if (state != STAR_STATE_EXPLOSIVE)
			vy -= STAR_GRAVITY * dt;

		// hướng của gạch nghiêng
		int direction = 0;
		onInclinedBrick = false;
		for (UINT i = 0; i < coObjects->size(); i++)
		{
			if (dynamic_cast<CInclinedBrick*>(coObjects->at(i))) {
				CInclinedBrick* brick = dynamic_cast<CInclinedBrick*>(coObjects->at(i));
				int tmp = brick->Collision(this, dy);
				if (tmp != 0)
					direction = tmp;
			}
		}

		if (onInclinedBrick == true && direction != 0) {
			if (direction == -1) {
				this->x -= 0.5;
			}
			if (direction == 1) {
				this->x += 0.5;
			}
			if (direction == -2) {
				this->x -= 1;
			}
			if (direction == 2) {
				this->x += 1;
			}
		}

		for (UINT i = 0; i < coObjects->size(); i++)
		{
			if (state != STAR_STATE_EXPLOSIVE) {
				if (dynamic_cast<CBrick*>(coObjects->at(i))) {
					CBrick* brick = dynamic_cast<CBrick*>(coObjects->at(i));
					if (this->CheckAABB(brick) && onInclinedBrick == false)
						this->SetState(STAR_STATE_EXPLOSIVE);
				}
				if (dynamic_cast<CConveyor*>(coObjects->at(i))) {
					CConveyor* conveyor = dynamic_cast<CConveyor*>(coObjects->at(i));
					if (this->CheckAABB(conveyor))
						this->SetState(STAR_STATE_EXPLOSIVE);
				}
			}
		}

		if (state == STAR_STATE_EXPLOSIVE && (GetTickCount64() - explosive_start >= STAR_EXPLOSIVE_TIME)) {
			this->SetState(STAR_STATE_HIDE);
		}

		float x0 = x;
		float y0 = y;

		vector<LPCOLLISIONEVENT> coEvents;
		vector<LPCOLLISIONEVENT> coEventsResult;

		coEvents.clear();

		CalcPotentialCollisions(coObjects, coEvents);

		if (coEvents.size() == 0)
		{
			x = x0 + dx;
			y = y0 + dy;
		}
		else
		{
			float min_tx, min_ty, nx = 0, ny = 0;
			float rdx = 0;
			float rdy = 0;

			x = x0 + dx;
			y = y0 + dy;

			FilterCollision(coEvents, coEventsResult, min_tx, min_ty, nx, ny, rdx, rdy);

			for (UINT i = 0; i < coEventsResult.size(); i++)
			{
				LPCOLLISIONEVENT e = coEventsResult[i];

				if (dynamic_cast<CBrick*>(e->obj))
				{
					y = y0 + min_ty * dy + ny * 0.1f;
					

					if (e->ny != 0) {
						vy = -0.75 * vy;
						if (abs(vy) < 0.1f) vy = 0;
					}
					if (e->nx != 0 && this->onInclinedBrick == false) {
						x = x0 + min_tx * dx + nx * 0.1f;
						vx = -vx;
					}
				}

				if (dynamic_cast<CConveyor*>(e->obj))
				{
					y = y0 + min_ty * dy + ny * 0.1f;
					x = x0 + min_tx * dx + nx * 0.1f;

					if (e->ny != 0) {
						vy = -0.75 * vy;
						if (abs(vy) < 0.1f) vy = 0;
					}
					if (e->nx != 0)
						vx = -vx;
				}
			}
		}

		if (CGame::GetInstance()->InCamera(this) == false)
			this->SetState(STAR_STATE_HIDE);

		// clean up collision events
		for (UINT i = 0; i < coEvents.size(); i++) delete coEvents[i];
	}
}

void CStar::Ready() {
	if (this->GetState() == STAR_STATE_HIDE)
		this->SetState(STAR_STATE_PENDING);
}

void CStar::Shot() {
	CGimmick* gimmick = ((CPlayScene*)CGame::GetInstance()->GetCurrentScene())->GetPlayer();
	if (this->GetState() == STAR_STATE_PENDING || this->GetState() == STAR_STATE_HIDE) {
		this->SetState(STAR_STATE_HIDE);
	}
	else if (this->GetState() == STAR_STATE_READY) {
		if (gimmick->nx > 0) {
			this->SetState(STAR_STATE_WALKING_RIGHT);
		}
		else {
			this->SetState(STAR_STATE_WALKING_LEFT);
		}
		this->vy = gimmick->vy;
		this->vx += 0.5 * gimmick->vx;
	}
}

void CStar::Render()
{
	int alpha = 255;

	if (state == STAR_STATE_HIDE) {
		//animation_set->at(STAR_ANI_MAIN)->Render(x, y, alpha);
	}

	if (state == STAR_STATE_PENDING) {
		animation_set->at(STAR_ANI_SMALL)->Render(x + star_small_1_x -5, y + star_small_1_y + 7, alpha);
		animation_set->at(STAR_ANI_SMALL)->Render(x + star_small_2_x -5, y + star_small_2_y + 7, alpha);
		animation_set->at(STAR_ANI_SMALL)->Render(x + star_small_3_x -5, y + star_small_3_y + 7, alpha);
		animation_set->at(STAR_ANI_SMALL)->Render(x + star_small_4_x -5, y + star_small_4_y + 7, alpha);
		animation_set->at(STAR_ANI_SMALL)->Render(x + star_small_5_x -5, y + star_small_5_y + 7, alpha);
	}
	if (state == STAR_STATE_READY || state == STAR_STATE_WALKING_LEFT || state == STAR_STATE_WALKING_RIGHT) {
		animation_set->at(STAR_ANI_MAIN)->Render(x - 5, y + 7, alpha);
	}

	if (state == STAR_STATE_EXPLOSIVE) {
		animation_set->at(STAR_ANI_EXPLOSIVE)->Render(x, y, alpha);
	}

	//RenderBoundingBox();
}

void CStar::SetState(int state)
{
	CGameObject::SetState(state);

	switch (state)
	{
	case STAR_STATE_HIDE:
		break;
	case STAR_STATE_PENDING:
		break;
	case STAR_STATE_CREATED:
		break;
	case STAR_STATE_READY:
		break;
	case STAR_STATE_WALKING_RIGHT:
		vx = STAR_VX;
		break;
	case STAR_STATE_WALKING_LEFT:
		vx = -STAR_VX;
		break;
	case STAR_STATE_EXPLOSIVE:
		this->explosive_start = GetTickCount64();
		vx = 0;
		vy = 0;
		break;
	}
}
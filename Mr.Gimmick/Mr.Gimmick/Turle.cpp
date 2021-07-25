#include "Turle.h"
#include "Brick.h"
#include "Utils.h"
#include "Game.h"
#include "PlayScene.h"
#include "Star.h"
#include "Gimmick.h"
#include "WaterDie.h"
#include "Backup.h"

CTurle::CTurle(float left, float right) : CGameObject()
{
	this->score = 200;
	this->SetState(TURLE_STATE_WALKING_RIGHT);
	this->left = left;
	this->right = right;
	this->canBeAttacked = true;
}

void CTurle::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
	if (state == TURLE_STATE_WALKING_RIGHT
		|| state == TURLE_STATE_WALKING_LEFT) {
		left = x;
		top = y;
		right = left + TURLE_WALKING_BBOX_WIDTH;
		bottom = top - TURLE_WALKING_BBOX_HEIGHT + 0.5f;
	}
	if (state == TURLE_STATE_TURNING_RIGHT
		|| state == TURLE_STATE_TURNING_LEFT) {
		left = x;
		top = y;
		right = left + TURLE_TURNING_BBOX_WIDTH;
		bottom = top - TURLE_TURNING_BBOX_HEIGHT;
	}
	if (state == TURLE_STATE_DIE_RIGHT
		|| state == TURLE_STATE_DIE_LEFT
		|| state == TURLE_STATE_DIE_COMPLETE_RIGHT
		|| state == TURLE_STATE_DIE_COMPLETE_LEFT) {
		left = x + 2.0f;
		top = y - 0.2f;
		right = left + TURLE_DIE_BBOX_WIDTH - 2.0f;
		bottom = top - TURLE_DIE_BBOX_HEIGHT + 0.5f;
	}
}

void CTurle::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	CScene* scene = CGame::GetInstance()->GetCurrentScene();
	CStar* star = ((CPlayScene*)scene)->GetStar();

	if (CheckAABB(star) && this->canBeAttacked && (this->GetState() == TURLE_STATE_DIE_RIGHT || this->GetState() == TURLE_STATE_DIE_LEFT)
		&& star->state != STAR_STATE_HIDE
		&& star->state != STAR_STATE_EXPLOSIVE 
		&& star->state != STAR_STATE_PENDING 
		&& star->state != STAR_STATE_READY) {

		if (this->GetState() == TURLE_STATE_DIE_RIGHT) {
			this->SetState(TURLE_STATE_DIE_COMPLETE_RIGHT);
		}	
		else if (this->GetState() == TURLE_STATE_DIE_LEFT) {
			this->SetState(TURLE_STATE_DIE_COMPLETE_LEFT);
		}
	}
		
	if (CheckAABB(star) && this->canBeAttacked && (this->GetState() != TURLE_STATE_DIE_COMPLETE_RIGHT || this->GetState() != TURLE_STATE_DIE_COMPLETE_LEFT) 
		&& star->state != STAR_STATE_HIDE
		&& star->state != STAR_STATE_EXPLOSIVE 
		&& star->state != STAR_STATE_PENDING 
		&& star->state != STAR_STATE_READY) {

		if (this->state == TURLE_STATE_WALKING_RIGHT)
			this->SetState(TURLE_STATE_DIE_RIGHT);
		else if (this->state == TURLE_STATE_WALKING_LEFT)
			this->SetState(TURLE_STATE_DIE_LEFT);

		this->canBeAttacked = false;
	}
	
	
	CGimmick* player = ((CPlayScene*)scene)->GetPlayer();

	if (CheckAABB(player) && (this->state == TURLE_STATE_WALKING_RIGHT || this->state == TURLE_STATE_WALKING_LEFT))
	{
		if (player->untouchable == 0)
			player->SetState(GIMMICK_STATE_STUN);
	}

	// Calculate dx, dy 
	CGameObject::Update(dt);

	// Simple fall down
	vy -= TURLE_GRAVITY * dt;
	
	for (UINT i = 0; i < coObjects->size(); i++)
	{
		if (dynamic_cast<CBrick*>(coObjects->at(i))) {
			CBrick* brick = dynamic_cast<CBrick*>(coObjects->at(i));
			if (onTopOf(brick, 3.0f) && state != TURLE_STATE_DIE_COMPLETE_LEFT && state!= TURLE_STATE_DIE_COMPLETE_RIGHT)
				vy = 0;
		}
	}

	float x0 = x;
	float y0 = y;

	x = x0 + dx;
	y = y0 + dy;

	if (this->state == TURLE_STATE_DIE_COMPLETE_RIGHT || this->state == TURLE_STATE_DIE_COMPLETE_LEFT)
		return;

	vector<LPCOLLISIONEVENT> coEvents;
	vector<LPCOLLISIONEVENT> coEventsResult;

	coEvents.clear();

	CalcPotentialCollisions(coObjects, coEvents);

	if (!canBeAttacked) {
		if (count_down == 0) {
			StartCountDown();
		}
		if (GetTickCount64() - count_down >= 1000) {
			this->canBeAttacked = true;
		}
	}

	if (coEvents.size() == 0)
	{
	}
	else
	{
		float min_tx, min_ty, nx = 0, ny = 0;
		//float min_tx, min_ty, nx, ny;
		float rdx = 0;
		float rdy = 0;

		FilterCollision(coEvents, coEventsResult, min_tx, min_ty, nx, ny, rdx, rdy);

		for (UINT i = 0; i < coEventsResult.size(); i++)
		{
			LPCOLLISIONEVENT e = coEventsResult[i];

			if (dynamic_cast<CBrick*>(e->obj))
			{				
				if (e->ny != 0) {
					this->vy = 0;
					if (this->state == TURLE_STATE_WALKING_LEFT
						|| this->state == TURLE_STATE_WALKING_RIGHT) {
						this->y = y0 + min_ty * dy + e->ny * 0.1f;
						this->x = x0 + min_tx * dx + e->nx * 0.2f;
					}

					else if (this->state == TURLE_STATE_DIE_RIGHT || this->state == TURLE_STATE_DIE_LEFT) {
						this->vx = 0;
						this->vy = 0;
						this->y = y0 + min_ty * dy + e->ny * 0.4f;
						this->x = x0 + min_tx * dx + e->nx * 0.2f;
					}
				}	
			}
			if (dynamic_cast<CWaterDie*>(e->obj))
			{
				CWaterDie* waterDie = dynamic_cast<CWaterDie*>(e->obj);
				if (e->ny != 0)
				{
					if (this->state == TURLE_STATE_DIE_RIGHT)
						SetState(TURLE_STATE_DISAPPEAR);
					else if (this->state == TURLE_STATE_DIE_LEFT)
						SetState(TURLE_STATE_DISAPPEAR);
				}
			}
		}
	}

	if (this->x < this->left + 1.0f && this->state == TURLE_STATE_WALKING_LEFT)
		this->SetState(TURLE_STATE_WALKING_RIGHT);
	if (this->x + TURLE_WALKING_BBOX_WIDTH > this->right - 1.0f && this->state == TURLE_STATE_WALKING_RIGHT)
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
	case TURLE_STATE_DIE_COMPLETE_RIGHT:
		ani = TURLE_ANI_DIE_RIGHT;
		break;
	case TURLE_STATE_DIE_LEFT:
	case TURLE_STATE_DIE_COMPLETE_LEFT:
		ani = TURLE_ANI_DIE_LEFT;
		break;
	}
	
	animation_set->at(ani)->Render(x, y, alpha);
	//RenderBoundingBox();
}

void CTurle::SetState(int state)
{
	CBackup* backup = CBackup::GetInstance();
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
		this->vx = 0;
		break;
	case TURLE_STATE_DIE_COMPLETE_RIGHT:
		backup->UpdateScore(backup->score + this->score);
		this->vx = -0.03;
		this->vy = 0.03;
		break;
	case TURLE_STATE_DIE_LEFT:
		this->vx = 0;
		break;
	case TURLE_STATE_DIE_COMPLETE_LEFT:
		backup->UpdateScore(backup->score + this->score);
		this->vx = 0.03f;
		this->vy = 0.03;
		break;
	}
}

bool CTurle::onTopOf(CGameObject* object, float equal)
{
	float ol, ot, or , ob;
	object->GetBoundingBox(ol, ot, or , ob);
	float l, t, r, b;
	GetBoundingBox(l, t, r, b);
	if (r >= ol && l <= or && abs(b - ot) < equal)
		return true;
	return false;
}
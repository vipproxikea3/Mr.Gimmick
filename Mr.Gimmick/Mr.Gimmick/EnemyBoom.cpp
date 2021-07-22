#include "EnemyBoom.h"
#include "Brick.h"
#include "Utils.h"
#include "Game.h"
#include "PlayScene.h"
#include "Star.h"
#include "Gimmick.h"
#include "MiniBoom.h"

CEnemyBoom::CEnemyBoom(float left, float right) : CGameObject()
{
	this->SetState(ENEMYBOOM_STATE_WALKING_RIGHT);
	this->left = left;
	this->right = right;
}

void CEnemyBoom::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
	left = x;
	top = y;
	right = left + ENEMYBOOM_BBOX_WIDTH;
	bottom = top - ENEMYBOOM_BBOX_HEIGHT;
}

void CEnemyBoom::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	if ((this->GetState() == ENEMYBOOM_STATE_DIE_RIGHT || this->GetState() == ENEMYBOOM_STATE_DIE_LEFT) && !CGame::GetInstance()->InCamera(this) && CGame::GetInstance()->InLargeCamera(this) && this->visible == true) {
		this->visible = false;
	}
	if (!visible)
		return;

	CScene* scene = CGame::GetInstance()->GetCurrentScene();
	CStar* star = ((CPlayScene*)scene)->GetStar();

	if (CheckAABB(star) && this->GetState() != ENEMYBOOM_STATE_DIE_RIGHT && this->GetState() != ENEMYBOOM_STATE_DIE_LEFT && star->state != STAR_STATE_HIDE
		&& star->state != STAR_STATE_EXPLOSIVE && star->state != STAR_STATE_PENDING && star->state != STAR_STATE_READY)
	{
		Shot();
		if (this->state == ENEMYBOOM_STATE_WALKING_RIGHT)
			this->SetState(ENEMYBOOM_STATE_DIE_RIGHT);
		else
			this->SetState(ENEMYBOOM_STATE_DIE_LEFT);
	}

	CGimmick* player = ((CPlayScene*)scene)->GetPlayer();

	if (CheckAABB(player) && (this->state == ENEMYBOOM_STATE_WALKING_RIGHT || this->state == ENEMYBOOM_STATE_WALKING_LEFT))
	{
		if (player->untouchable == 0)
			player->SetState(GIMMICK_STATE_STUN);
	}

	// Calculate dx, dy 
	CGameObject::Update(dt);

	// Simple fall down
	vy -= ENEMYBOOM_GRAVITY * dt;

	float x0 = x;
	float y0 = y;

	x = x0 + dx;
	y = y0 + dy;

	if (this->state == ENEMYBOOM_STATE_DIE_RIGHT || this->state == ENEMYBOOM_STATE_DIE_LEFT)
		return;

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

				if (e->nx > 0 && this->state == ENEMYBOOM_STATE_WALKING_LEFT)
					this->SetState(ENEMYBOOM_STATE_WALKING_RIGHT);
				if (e->nx < 0 && this->state == ENEMYBOOM_STATE_WALKING_RIGHT)
					this->SetState(ENEMYBOOM_STATE_WALKING_LEFT);

				if (e->ny != 0)
					this->vy = 0;
			}
		}
	}

	if (this->x < this->left && this->state == ENEMYBOOM_STATE_WALKING_LEFT)
		this->SetState(ENEMYBOOM_STATE_WALKING_RIGHT);
	if (this->x + ENEMYBOOM_BBOX_WIDTH > this->right && this->state == ENEMYBOOM_STATE_WALKING_RIGHT)
		this->SetState(ENEMYBOOM_STATE_WALKING_LEFT);

	// clean up collision events
	for (UINT i = 0; i < coEvents.size(); i++) delete coEvents[i];
}

void CEnemyBoom::Shot() {
	CMiniBoom* boom = new CMiniBoom();

	boom->SetPosition(this->x, this->y);
	if (this->state == ENEMYBOOM_STATE_WALKING_RIGHT)
		boom->SetState(MINIBOOM_STATE_WALKING_RIGHT);
	else
		boom->SetState(MINIBOOM_STATE_WALKING_LEFT);

	CAnimationSets* animation_sets = CAnimationSets::GetInstance();
	LPANIMATION_SET ani_set = animation_sets->Get(410001);
	boom->SetAnimationSet(ani_set);
	((CPlayScene*)CGame::GetInstance()->GetCurrentScene())->PushBackObj(boom);
}

void CEnemyBoom::Render()
{
	if (!visible)
		return;

	int alpha = 255;

	int ani = -1;

	switch (this->GetState())
	{
	case ENEMYBOOM_STATE_WALKING_RIGHT:
		ani = ENEMYBOOM_ANI_WALKING_RIGHT;
		break;
	case ENEMYBOOM_STATE_WALKING_LEFT:
		ani = ENEMYBOOM_ANI_WALKING_LEFT;
		break;
	case ENEMYBOOM_STATE_DIE_RIGHT:
		ani = ENEMYBOOM_ANI_DIE_RIGHT;
		break;
	case ENEMYBOOM_STATE_DIE_LEFT:
		ani = ENEMYBOOM_ANI_DIE_RIGHT;
		break;
	default:
		ani = ENEMYBOOM_ANI_WALKING_RIGHT;
		break;
	}

	animation_set->at(ani)->Render(x, y, alpha);
	//RenderBoundingBox();
}

void CEnemyBoom::SetState(int state)
{
	CGameObject::SetState(state);

	switch (state)
	{
	case ENEMYBOOM_STATE_WALKING_RIGHT:
		this->vx = ENEMYBOOM_WALKING_SPEED;
		break;
	case ENEMYBOOM_STATE_WALKING_LEFT:
		this->vx = -ENEMYBOOM_WALKING_SPEED;
		break;
	case ENEMYBOOM_STATE_DIE_RIGHT:
		this->y -= 16;
		this->vy = ENEMYBOOM_DIE_SPEED;
		break;
	case ENEMYBOOM_STATE_DIE_LEFT:
		this->y -= 16;
		this->vy = ENEMYBOOM_DIE_SPEED;
		break;
	}
}
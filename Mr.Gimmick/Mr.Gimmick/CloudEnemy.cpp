#include "CloudEnemy.h"
#include "Brick.h"
#include "Utils.h"
#include "Gimmick.h"
#include "Game.h"
#include "Star.h"
#include "PlayScene.h"



CCloudEnemy::CCloudEnemy() : CGameObject()
{
	SetState(CLOUD_STATE_WALK);
}

CCloudEnemy::CCloudEnemy(int left, int right)
{
	limitRight = right;
	limitLeft = left;
	SetState(CLOUD_STATE_WALK);

}

void CCloudEnemy::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	/*if (attack)
	{
		SetState(CLOUD_STATE_ATTACK);
	}*/
	vy -= GIMMICK_GRAVITY * dt;
	SetState();

	CStar* star = ((CPlayScene*)CGame::GetInstance()->GetCurrentScene())->GetStar();
	if (this->CheckAABB(star) && (star->GetState() == STAR_STATE_WALKING_LEFT || star->GetState() == STAR_STATE_WALKING_RIGHT))
	{
		star->SetState(STAR_STATE_EXPLOSIVE);
		tmpState = state;
		vx = -direction * CLOUD_WALKING_SPEED;
		SetState(CLOUD_STATE_STUN);
	}




	CGameObject::Update(dt);
	vector<LPGAMEOBJECT> newCoObjects;
	for (UINT i = 0; i < coObjects->size(); i++)
	{
		if (dynamic_cast<CBrick*>(coObjects->at(i))) newCoObjects.push_back(coObjects->at(i));
	}
	vector<LPCOLLISIONEVENT> coEvents;
	vector<LPCOLLISIONEVENT> coEventsResult;
	coEvents.clear();
	CalcPotentialCollisions(&newCoObjects, coEvents);

	if (coEvents.size() == 0)
	{
		x += dx;
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
				y = y0 + min_ty * dy + ny * 0.1f;

				if (e->nx != 0) {
					direction *= -1;
					SetState(CLOUD_STATE_WALK);
				}
				if (e->ny != 0) vy = 0;
			}

		}
	}



	//// clean up collision events
	for (UINT i = 0; i < coEvents.size(); i++) delete coEvents[i];

}

void CCloudEnemy::Render()
{

	int ani = -1;
	if (vx == 0)
	{
		if (direction > 0)
			ani = CLOUD_ANI_IDLE_RIGHT;
		else
			ani = CLOUD_ANI_IDLE_LEFT;
	}
	else
	{
		if (direction > 0)
		{
			ani = CLOUD_ANI_GO_RIGHT;
		}
		else
		{
			ani = CLOUD_ANI_GO_LEFT;
		}
	}

	int alpha = 255;
	animation_set->at(ani)->Render(x, y, alpha);
	//RenderBoundingBox();
}

void CCloudEnemy::SetState(int state_)
{
	if (state_ != -1)
	CGameObject::SetState(state_);

	switch (state)
	{
	case CLOUD_STATE_IDLE:
		vx = 0;
		vy = 0;
		break;
	case CLOUD_STATE_WALK:
		if (x >= limitRight || x <= limitLeft)
			direction = -1 * direction;
		vx = direction * CLOUD_WALKING_SPEED;
		break;
	case CLOUD_STATE_ATTACK:
		if (carryPlayer)
		{
			if (direction < 0) 
			{
				if (x < limitLeft)
					vx = 0;
				else
					vx = vx >= 0 ? 0 : (vx + 0.00005 * dt);
			}

			if (direction > 0) 
			{
				if (x > limitRight)
					vx = 0;
				else
					vx = vx <= 0 ? 0 : (vx - 0.00005 * dt);
			}
		}
		else 
		{
			vx = direction * CLOUD_WALKING_SPEED * 2;

			CGimmick* gimmick = ((CPlayScene*)(CGame::GetInstance()->GetCurrentScene()))->GetPlayer();
			float gimmickX = gimmick->x;
			float tmp_maxX = gimmickX + 50 < limitRight ? gimmickX + 50 : limitRight;
			float tmp_minX = gimmickX - 50 > limitLeft ? gimmickX - 50 : limitLeft;

			if (direction > 0)
			{
				if (x > tmp_maxX) 
				{
					if (x < gimmickX)
					{
						vx = 0;
					}
					else 
					{
						direction *= -1;
						vx = direction * CLOUD_WALKING_SPEED * 2;
					}
				}
			}
			else if (direction < 0)
			{
				if (x < tmp_minX) 
				{
					if (x > gimmickX)
					{
						vx = 0;
					}
					else 
					{
						direction *= -1;
						vx = direction * CLOUD_WALKING_SPEED * 2;
					}
				}

			}
		}

		carryPlayer = false;
		break;
	case CLOUD_STATE_STUN:
		if (vx * direction < 0)
		{
			vx *= 0.97;
			if (abs(vx) < 0.005)
				vx = -vx;
		}
		else 
		{
			vx /= 0.97;
			if (abs(vx) > CLOUD_WALKING_SPEED)
				SetState(tmpState);
		}

	default:
		break;
	}

}

void CCloudEnemy::GetBoundingBox(float& l, float& t, float& r, float& b)
{
	l = x;
	t = y;
	r = l + CLOUD_BBOX_WIDTH;
	b = t - CLOUD_BBOX_HEIGHT;
}


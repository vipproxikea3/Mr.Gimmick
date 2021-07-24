#include "Medicine.h"
#include "PlayScene.h"
#include "Gimmick.h"
#include "Brick.h"
#include "Backup.h"

CMedicine::CMedicine(int type) : CGameObject()
{
	this->type = type;
	SetState(MEDICINE_STATE_APPEAR);
}

void CMedicine::GetBoundingBox(float& l, float& t, float& r, float& b)
{
	if (state != MEDICINE_STATE_DISAPPEAR)
	{
		if (type == 1)
		{
			l = x;
			t = y;
			r = l + MEDICINE_ORANGE_BBOX_WIDTH;
			b = t - MEDICINE_ORANGE_BBOX_HEIGHT + 0.5f;
		}
		else if (type == 2)
		{
			l = x;
			t = y;
			r = l + MEDICINE_PINK_BBOX_WIDTH;
			b = t - MEDICINE_PINK_BBOX_HEIGHT + 0.5f;
		}
		else if (type == 3 || type == 4)
		{
			l = x;
			t = y;
			r = l + MEDICINE_TYPE_CIRCLE_BBOX_WIDTH;
			b = t - MEDICINE_TYPE_CIRCLE_BBOX_HEIGHT + 0.5f;
		}
		else if (type == 5)
		{
			l = x;
			t = y;
			r = l + VASE_FLOWER_BBOX_WIDTH;
			b = t - VASE_FLOWER_BBOX_HEIGHT + 0.5f;
		}
		else if (type == 6)
		{
			l = x;
			t = y;
			r = l + HOUR_GLASS_BBOX_WIDTH;
			b = t - HOUR_GLASS_BBOX_HEIGHT + 0.5f;
		}
	}
}

void CMedicine::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects) 
{
	if (state == MEDICINE_STATE_DISAPPEAR)
		return;

	CScene* scene = CGame::GetInstance()->GetCurrentScene();
	CGimmick* player = ((CPlayScene*)scene)->GetPlayer();
	CBackup* backup = CBackup::GetInstance();

	if (CheckAABB(player) && this->state == MEDICINE_STATE_APPEAR) {
		SetState(MEDICINE_STATE_DISAPPEAR);
		if (type != 5 && type != 6)
		{
			Sound::GetInstance()->Play("SOUND_Effect_35", 0, 1);
		}
		else
			Sound::GetInstance()->Play("SOUND_Effect_80", 0, 1);
		switch (this->type)
		{
		case 1:
			backup->UpdateLifeStack(backup->lifeStack + 1);
			break;
		case 5:
			backup->UpdateScore(backup->score + 50000);
			backup->UpdateRest(backup->rest + 2);
			break;
		case 6:
			backup->UpdateScore(backup->score + 50000);
			backup->UpdateRest(backup->rest + 2);
			break;
		default:
			break;
		}
	}

	for (UINT i = 0; i < coObjects->size(); i++)
	{
		if (dynamic_cast<CBrick*>(coObjects->at(i))) {
			CBrick* brick = dynamic_cast<CBrick*>(coObjects->at(i));
			if (CheckAABB(brick)) {
				switch (this->type)
				{
				case 1:
					this->y = brick->y + 16 + 1;
					break;
				case 2:
					this->y = brick->y + 14 + 1;
					break;
				case 3:
					this->y = brick->y + 15 + 1;
					break;
				case 4:
					this->y = brick->y + 15 + 1;
					break;
				case 5:
					this->y = brick->y + 43 + 1;
					break;
				case 6:
					this->y = brick->y + 28 + 1;
					break;
				default:
					break;
				}
			}
		}
	}
	CGameObject::Update(dt);

	vector<LPGAMEOBJECT> newCoObjects;

	if (state != MEDICINE_STATE_DISAPPEAR)
		vy -= MEDICINE_GRAVITY * dt;

	//float x0 = x;
	float y0 = y;

	//x = x0 + dx;
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
				this->y = y0 + min_ty * dy + e->ny * 0.4f;
				if (e->ny != 0) {
					this->vy = 0;
				}
					
			}
		}
	}

	// clean up collision events
	for (UINT i = 0; i < coEvents.size(); i++) delete coEvents[i];
}


void CMedicine::Render()
{
	int ani;

	if (this->state == MEDICINE_STATE_DISAPPEAR)
		return;
	else
	{
		switch (this->type)
		{
		case 1:
			ani = MEDICINE_ANI_ORANGE;
			break;
		case 2:
			ani = MEDICINE_ANI_PINK;
			break;
		case 3:
			ani = MEDICINE_ANI_TYPE_PINK_CIRCLE;
			break;
		case 4:
			ani = MEDICINE_ANI_TYPE_BLACK_CIRCLE;
			break;
		case 5:
			ani = VASE_ANI_FLOWER;
			break;
		case 6:
			ani = HOUR_ANI_GLASS;
			break;
		}
	}
	int alpha = 255;
	animation_set->at(ani)->Render(x, y, alpha);
	//RenderBoundingBox();
}

void CMedicine::SetState(int state) 
{
	if (this->state == MEDICINE_STATE_DISAPPEAR)
		return;
	CGameObject::SetState(state);

	switch (state)
	{
	case MEDICINE_STATE_APPEAR:
		this->vx = 0;
		this->vy = 0;
		break;
	}
}
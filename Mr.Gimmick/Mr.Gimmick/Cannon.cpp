#include "Cannon.h"
#include "BoomCannon.h"
#include "Bullet.h"

CCannon::CCannon()
{
	SetState(CANNON_STATE_IDLE);
}

void CCannon::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	vy -= CANNON_GRAVITY * dt;
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

	CScene* scene = CGame::GetInstance()->GetCurrentScene();
	CGimmick* gimmick = ((CPlayScene*)scene)->GetPlayer();
	if (state == CANNON_STATE_MOVE) {
		if (gimmick->GetState() == GIMMICK_STATE_IDLE) {
			SetState(CANNON_STATE_IDLE);
		}
	}

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
				
				

				if (e->nx != 0){
					vx = 0;
					this->x = x0 + min_tx * dx + nx * 0.4f;
				}
				if (e->ny != 0) vy = 0;

			}
		}
	}
	//vx *= 0.98;

	delay -= dt;
	if (delay <= 0)
	{
		delay = CANNON_DELAY_TIME;
		CBoomCannon* boom_cannon = new CBoomCannon();
		boom_cannon->SetPosition(x, y);
		boom_cannon->SetSpeed(-0.08f, 0);
		//boom_cannon->SetSpeed(-0.03f, 0);
		CAnimationSets* animation_sets = CAnimationSets::GetInstance();
		LPANIMATION_SET ani_set = animation_sets->Get(900001);
		boom_cannon->SetAnimationSet(ani_set);
		((CPlayScene*)CGame::GetInstance()->GetCurrentScene())->PushBackObj(boom_cannon);
	}
}

void CCannon::Render()
{
	int ani = 0;
	int alpha = 255;
	if (state == CANNON_STATE_MOVE)
		ani = CANNON_ANI_MOVE;
	animation_set->at(ani)->Render(x, y, alpha);

	//RenderBoundingBox();
}



void CCannon::GetBoundingBox(float& l, float& t, float& r, float& b)
{
	l = x;
	t = y;
	r = l + CANNON_BBOX_WIDTH - 1.0f;
	b = t - CANNON_BBOX_HEIGHT + 1.0f;
}

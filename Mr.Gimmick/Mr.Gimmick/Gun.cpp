#include "Gun.h"
#include "Bullet.h"

void CGun::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	vy -= GIMMICK_GRAVITY * dt;
	CGameObject::Update(dt);


	onInclinedBrick = false;
	int direction = 0;

	vector<LPGAMEOBJECT> newCoObjects;
	for (UINT i = 0; i < coObjects->size(); i++)
	{
		if (dynamic_cast<CInclinedBrick*>(coObjects->at(i)))
		{
			CInclinedBrick* brick = dynamic_cast<CInclinedBrick*>(coObjects->at(i));
			int tmp = brick->Collision(this, dy);
			if (tmp != 0)
				direction = tmp;
		}

		if (dynamic_cast<CBrick*>(coObjects->at(i))) newCoObjects.push_back(coObjects->at(i));
		if (dynamic_cast<CSewer*>(coObjects->at(i))) {
			CSewer* sewer = dynamic_cast<CSewer*>(coObjects->at(i));
			if (CheckAABB(sewer))
				visible = false;
		}

	}

	if (onInclinedBrick == true && direction != 0)
	{
		if (direction == -1) 
		{
			this->x -= 0.5;
			if (vx > -0.1) 
			{
				vx -= 0.0005;
			}
		}
		if (direction == 1) 
		{
			this->x += 0.5;
			this->x -= 0.5;
			if (vx < 0.1) 
			{
				vx += 0.0005;
			}
		}
		if (direction == -2)
		{
			this->x -= 1;
		}
		if (direction == 2)
		{
			this->x += 1;
		}
	}

	vector<LPCOLLISIONEVENT> coEvents;
	vector<LPCOLLISIONEVENT> coEventsResult;

	coEvents.clear();
	CalcPotentialCollisions(&newCoObjects, coEvents);

	if (coEvents.size() == 0)
	{
		x += dx;
		if (!onInclinedBrick)
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
				if (onInclinedBrick) x = x0 + dx;
				y = y0 + min_ty * dy + ny * 0.1f;

				if (e->nx != 0)vx = 0;

				if (e->ny != 0) vy = 0;

			}

		}
	}
	vx *= 0.98;

	delay -= dt;
	if (delay <= 0)
	{
		delay = GUN_DELAY_TIME;
		CBullet* bullet = new CBullet();

		bullet->SetPosition(x + 16, y);
		bullet->SetSpeed(0.075, 0);

		CAnimationSets* animation_sets = CAnimationSets::GetInstance();
		LPANIMATION_SET ani_set = animation_sets->Get(210001);
		bullet->SetAnimationSet(ani_set);
		((CPlayScene*)CGame::GetInstance()->GetCurrentScene())->PushBackObj(bullet);
	}
}

void CGun::Render()
{
	int alpha = 255;
	animation_set->at(0)->Render(x, y, alpha);

}



void CGun::GetBoundingBox(float& l, float& t, float& r, float& b)
{
	l = x + 0.5f;
	t = y - 0.5f;
	r = l + 16 - 0.5f;
	b = t - 16 + 0.5f;
}

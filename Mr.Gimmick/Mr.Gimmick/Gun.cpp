#include "Gun.h"


void CGun::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{	

	vy -= GIMMICK_GRAVITY * dt;
	CGameObject::Update(dt);

	onGround = false;
	onInclinedBrick = false;


	int direction = -1;  //trai false, phai true

	vector<LPGAMEOBJECT> newCoObjects;
	for (UINT i = 0; i < coObjects->size(); i++)
	{
		if (dynamic_cast<CBrick*>(coObjects->at(i))) newCoObjects.push_back(coObjects->at(i));
		if (dynamic_cast<CInclinedBrick*>(coObjects->at(i))) {
			CInclinedBrick* brick = dynamic_cast<CInclinedBrick*>(coObjects->at(i));
			direction = brick->Collision(this, dy);
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
		float min_tx, min_ty, nx = 0, ny;
		float rdx = 0;
		float rdy = 0;

		float x0 = x;
		float y0 = y;


		FilterCollision(coEvents, coEventsResult, min_tx, min_ty, nx, ny, rdx, rdy);

		for (UINT i = 0; i < coEventsResult.size(); i++)
		{
			LPCOLLISIONEVENT e = coEventsResult[i];

			if (dynamic_cast<CBrick*>(e->obj)) {
				x = x0 + min_tx * dx + nx * 0.1f;
				if (onInclinedBrick) {
					x = x0 + dx;
				}
				y = y0 + min_ty * dy + ny * 0.1f;

				if (e->nx != 0) vx = 0;
				if (e->ny != 0) vy = 0;

				if (e->ny == 1)
					this->onGround = true;
			}

		}
	}

	for (UINT i = 0; i < newCoObjects.size(); i++) newCoObjects[i] = nullptr;
	for (UINT i = 0; i < coEvents.size(); i++) delete coEvents[i];

	if (onInclinedBrick)
		if (direction == 0)
			vx = vx <= -0.1 ? -0.1 : vx -= 0.003;
		else if (direction == 1)
			vx = vx >= 0.1 ? 0.1 : vx += 0.003;
	vx *= 0.95;


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
		vector<LPGAMEOBJECT>* objects = ((CPlayScene*)CGame::GetInstance()->GetCurrentScene())->GetListObject();
		objects->push_back(bullet);

	}

}

void CGun::Render()
{
	int alpha = 255;
	animation_set->at(0)->Render(x, y, alpha);
	//RenderBoundingBox();
}



void CGun::GetBoundingBox(float& l, float& t, float& r, float& b)
{
	l = x + 0.5f;
	t = y - 0.5f;
	r = l + 16 - 0.5f;
	b = t - 16 + 0.5f;
}

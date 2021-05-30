#include "BrickPink.h"
#include "Utils.h"
#include "PlayScene.h"
#include "Gimmick.h"


CBrickPink::CBrickPink(float left, float top, float leftrans, float toptrans,float av) : CGameObject() {
	this->left = left;
	this->top = top;
	this->leftrans = leftrans;
	this->toptrans = toptrans;
	this->av = av;
	if (left == leftrans)
	{
		this->vy = BRICK_SPEED*av;
		vyb = 1;
	}
	if (top == toptrans)
	{
		this->vx = BRICK_SPEED*av;
		this->nx = 1;
		vyb = 0;
	}
}

void CBrickPink::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects) {
	// Calculate dx, dy 
	CGameObject::Update(dt);

	if (vyb == 1)
	{
		if (y > top && vy > 0)
		{
			vy = -vy;
		}
		if (y < toptrans && vy < 0)
			vy = -vy;
	}
	if (vyb == 0)
	{
		if (x > leftrans && vx > 0)
		{
			vx = -vx;
			nx = -nx;
		}
		if (x < left && vx < 0)
		{
			vx = -vx;
			nx = -nx;
		}
	}
	//DebugOut(L"\timedie = %f\n", x);
	//DebugOut(L"\timedie = %f\n", y);
	x += dx;
	y += dy;

	//vector<LPCOLLISIONEVENT> coEvents;
	//vector<LPCOLLISIONEVENT> coEventsResult;

	////coEvents.clear();

	//CalcPotentialCollisions(coObjects, coEvents);		
	//
	//
	//float min_tx, min_ty, nx = 0, ny = 0;
	//float rdx = 0;
	//float rdy = 0;

	//float x0 = x;
	//float y0 = y;
	//FilterCollision(coEvents, coEventsResult, min_tx, min_ty, nx, ny, rdx, rdy);

	//for (UINT i = 0; i < coEventsResult.size(); i++)
	//{
	//	LPCOLLISIONEVENT e = coEventsResult[i];
	//	if (dynamic_cast<CGimmick*>(e->obj))
	//	{
	//		CGimmick* Gimmick = dynamic_cast<CGimmick*>(e->obj);
	//		if (e->ny < 0)
	//		{
	//			Gimmick->x += dx;
	//			Gimmick->y += dy;
	//		}
	//	}
	//}
	//
	// //clean up collision events
	//for (UINT i = 0; i < coEvents.size(); i++) delete coEvents[i];
}

void CBrickPink::Render()
{
	animation_set->at(0)->Render(x, y);
	//RenderBoundingBox();
}

//void CBrickPink::SetState(int state) {}

void CBrickPink::GetBoundingBox(float& l, float& t, float& r, float& b)
{
	l = x;
	t = y;
	r = l + BRICK_BBOX_WIDTH;
	b = t - BRICK_BBOX_HEIGHT;
}
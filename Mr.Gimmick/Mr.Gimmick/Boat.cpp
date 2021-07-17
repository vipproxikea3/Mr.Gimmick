#include "Boat.h"
#include "Gimmick.h"
#include "PlayScene.h"
#include "Game.h"

//void CBoat::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects) {
//	CGameObject::Update(dt);
//	CGimmick* gimmick = ((CPlayScene*)CGame::GetInstance()->GetCurrentScene())->GetPlayer();
//
//	coObjects->push_back(gimmick); //Them Gimmick vao
//
//	vector<LPCOLLISIONEVENT> coEvents;
//	vector<LPCOLLISIONEVENT> coEventsResult;
//
//	coEvents.clear();
//
//	CalcPotentialCollisions(coObjects, coEvents);
//
//	if (coEvents.size() == 0)
//	{
//	}
//	else
//	{
//		float min_tx, min_ty, nx = 0, ny;
//		float rdx = 0;
//		float rdy = 0;
//
//		float x0 = x;
//		float y0 = y;
//
//		FilterCollision(coEvents, coEventsResult, min_tx, min_ty, nx, ny, rdx, rdy);
//
//		float l, t, r, b;
//		GetBoundingBox(l, t, r, b);
//		for (UINT i = 0; i < coEventsResult.size(); i++)
//		{
//			LPCOLLISIONEVENT e = coEventsResult[i];
//			float ol, ot, or , ob;
//			e->obj->GetBoundingBox(ol, ot, or , ob);
//
//			if (dynamic_cast<CGimmick*>(e->obj))
//			{
//				if (e->nx != 0)
//					e->obj->x -= (min_tx * dx + nx * 0.3f); //Day gimmick ra xa
//
//			}
//		}
//	}
//	// clean up collision events
//	for (UINT i = 0; i < coEvents.size(); i++) delete coEvents[i];
//}
void CBoat::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects) {

	float lastx = x;
	if (x >= BOAT_FINISH)
		run = false;
	if (run == true)
		vx = BOAT_VX;
	else
		vx = 0;
	CGameObject::Update(dt);
	x += dx;
	//CGimmick* gimmick = ((CPlayScene*)CGame::GetInstance()->GetCurrentScene())->GetPlayer();
	//if (this->CheckAABB(gimmick)) {
	//	gimmick->x = x;
	//	//DebugOut(L"\nVao day %d", gimmick->x);
	//}
}

void CBoat::Render()
{
	int ani;
	ani = BOAT_ANI;
	int alpha = 255;
	animation_set->at(ani)->Render(x, y, alpha);
	//RenderBoundingBox();
}

void CBoat::SetState(int state) {}
void CBoat::GetBoundingBox(float& l, float& t, float& r, float& b)
{
	l = x;
	t = y-3;
	r = x + BOAT_BBOX_WIDTH;
	b = y -	BOAT_BBOX_HEIGHT;
}
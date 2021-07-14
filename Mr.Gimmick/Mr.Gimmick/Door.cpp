#include "Door.h"

CDoor::CDoor() : CGameObject()
{
	SetState(DOOR_STATE_CLOSE);
}

void CDoor::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects) {
	// Calculate dx, dy 
	CGameObject::Update(dt);

	CGimmick* gimmick = ((CPlayScene*)CGame::GetInstance()->GetCurrentScene())->GetPlayer();

	if (state != DOOR_STATE_STOP)
	{
		if (gimmick->x - x <= 56 && y - gimmick->y <= 70) {
			SetState(DOOR_STATE_OPEN);
		}
	}
	if (state == DOOR_STATE_OPEN)
	{
		if (open_start == 0)
		{
			StartOpen();
		}
		if (GetTickCount64() - open_start >= DOOR_LIMIT_TIME)
		{
			SetState(DOOR_STATE_STOP);
		}
	}
}

void CDoor::Render()
{
	int ani;
	if (state == DOOR_STATE_CLOSE) {
		ani = DOOR_ANI_CLOSE;
	}
	else if (state == DOOR_STATE_OPEN) {
		ani = DOOR_ANI_OPEN;
	}
	else {
		ani = DOOR_ANI_STOP;
	}
	int alpha = 255;
	animation_set->at(ani)->Render(x, y, alpha);
	RenderBoundingBox();
}

void CDoor::SetState(int state)
{
	CGameObject::SetState(state);
}

void CDoor::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
	left = x;
	top = y;
	right = left + DOOR_BBOX_WIDTH;
	bottom = top - DOOR_BBOX_HEIGHT;
}

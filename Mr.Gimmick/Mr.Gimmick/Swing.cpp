#include "Swing.h"
#include "Gimmick.h"
#include "PlayScene.h"
#include "algorithm"

CSwing::CSwing() : CGameObject()
{
	SetState(SWING_STATE_STAND);
}

void CSwing::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
	left = x;
	top = y - 62.0f;
	right = x + SWING_BBOX_WIDTH;
	bottom = y - SWING_BBOX_HEIGHT;
}

void CSwing::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	// Calculate dx, dy 
	CGameObject::Update(dt);

	x += dx;

	CGimmick* gimmick = ((CPlayScene*)CGame::GetInstance()->GetCurrentScene())->GetPlayer();


	if (x + SWING_BBOX_WIDTH / 2  >= SWING_LIMIT_X)
	{
		if (state == SWING_STATE_MOVE)
			SetState(SWING_STATE_OPEN);
	}
	
	if (state == SWING_STATE_OPEN)
	{
		if (open_start == 0)
		{
			StartOpen();
		}
		if (GetTickCount64() - open_start >= SWING_TIME_STOP)
		{
			SetState(SWING_STATE_STOP);
		}
	}

	Revival();
}

void CSwing::Revival() {
	CGame* game = CGame::GetInstance();
	if (this->state == SWING_STATE_STOP && !game->InCamera(this) && !game->InCameraByBBox(x_start, y_start, x_start + SWING_BBOX_WIDTH, y_start + SWING_BBOX_HEIGHT))
	{
		this->SetPosition(x_start, y_start);
		this->SetState(SWING_STATE_STAND);
	}
}

void CSwing::Render()
{
	int ani = -1;

	if (state == SWING_STATE_STAND)
	{
		ani = SWING_ANI_STAND;
	}
	else if (state == SWING_STATE_MOVE)
		ani = SWING_ANI_MOVE;
	else if (state == SWING_STATE_OPEN)
		ani = SWING_ANI_OPEN;
	else if (state == SWING_STATE_STOP)
		ani = SWING_ANI_STOP;

	int alpha = 255;
	animation_set->at(ani)->Render(x, y, alpha);

	//RenderBoundingBox();
}

void CSwing::SetState(int state)
{
	CGameObject::SetState(state);

	switch (state)
	{
	case SWING_STATE_STAND:
		Sound::GetInstance()->Stop("SOUND_Effect_19");
		Sound::GetInstance()->Stop("SOUND_Effect_45");
		vx = 0;
		break;
	case SWING_STATE_MOVE:
		Sound::GetInstance()->Play("SOUND_Effect_19", 1);
		vx = SWING_SPEED_X;
		break;
	case SWING_STATE_OPEN:
		Sound::GetInstance()->Stop("SOUND_Effect_19");
		Sound::GetInstance()->Play("SOUND_Effect_45", 0,1);
		vx = 0;
		break;
	}
}
#include "BlackBird.h"
#include "Game.h"
#include "PlayScene.h"
#include "Gimmick.h"


CBlackBird::CBlackBird(int direction, float endPoint)
{
	this->direction = direction;
	this->endPoint = endPoint;
	vx = direction * BLACKBIRD_VX;
	state = BLACKBIRD_STATE_TRANSFORM;
}

void CBlackBird::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{

	if (state == BLACKBIRD_STATE_TRANSFORM)
	{
		timeTransform -= dt;
		if (timeTransform <= 0)
		{
			state = BLACKBIRD_STATE_WAITPLAYER;
		}

		//DebugOut(L"Change state %d\n", timeTransform);
	}
	else if (state == BLACKBIRD_STATE_WAITPLAYER)
	{
		timeGoWaitPlace -= dt;
		if (timeGoWaitPlace >= 0)
		{
			vx = BLACKBIRD_VX * direction;
			vy = -0.008;
			CGameObject::Update(dt);
			x += dx;
			y += dy;
			state = BLACKBIRD_STATE_WAITPLAYER;
		}
		else
		{
			vx = 0;
			sinX += dt;
			vy = BLACKBIRD_VY * 2 * sin(3.14 * sinX / 5 / 180);

			CGameObject::Update(dt);
			x += dx;
			y += dy;
			state = BLACKBIRD_STATE_WAITPLAYER;
		}

		//DebugOut(L"Wait  %d\n", timeGoWaitPlace);
	}
	else if (DropPlayer())
	{
		vx = BLACKBIRD_VX * direction * 1.5;
		vy = BLACKBIRD_VY * 5;

		CGameObject::Update(dt);
		x += dx;
		y += dy;

		state = BLACKBIRD_STATE_DROPPLAYER;
		//DebugOut(L"Fly\n");
	}
	else
	{
		vx = BLACKBIRD_VX * direction;
		sinX += dt;
		vy = BLACKBIRD_VY * 2 * sin(3.14 * sinX / 5 / 180);

		CGameObject::Update(dt);
		x += dx;
		y += dy;

		if (countOnBird > 0)
		{
			CGimmick* player = ((CPlayScene*)(CGame::GetInstance()->GetCurrentScene()))->GetPlayer();
			player->UpdateWithBird(dx, dy);
			if (countOnBird <= 2) 
			{
				player->UpdateWithBird(0, 1);
			}
			vy = 0;
		}
		//DebugOut(L"CountonBird  %d\n", countOnBird);
		countOnBird--;
		state = BLACKBIRD_STATE_CARRYPLAYER;
	}
	Revival();
}

void CBlackBird::Revival() {
	CGame* game = CGame::GetInstance();
	if ((this->state == BLACKBIRD_STATE_DROPPLAYER || BLACKBIRD_STATE_CARRYPLAYER) && !game->InCamera(this) && !game->InCameraByBBox(x_start, y_start, x_start + BLACKBIRD_BBOX_WIDTH, y_start + BLACKBIRD_BBOX_HEIGHT))
	{
		this->SetState(BLACKBIRD_STATE_TRANSFORM);
		this->SetPosition(x_start, y_start);
		timeTransform = 500;
		timeGoWaitPlace = 1500;
	}
}

void CBlackBird::Render()
{

	if (state == BLACKBIRD_STATE_TRANSFORM)
	{
		if (direction > 0)
		{
			animation_set->at(0)->Render(x, y + 27, 255);
		}
		else
		{
			animation_set->at(1)->Render(x, y + 27, 255);
		}
	}
	else
	{
		if (direction > 0)
		{
			animation_set->at(2)->Render(x - 5, y + 15, 255);
		}
		else
		{
			animation_set->at(3)->Render(x - 27, y + 15, 255);
		}
	}

	//RenderBoundingBox();


}


void CBlackBird::GetBoundingBox(float& l, float& t, float& r, float& b)
{
	l = x;
	t = y;
	r = l + BLACKBIRD_BBOX_WIDTH;
	b = t - BLACKBIRD_BBOX_HEIGHT;
}


void CBlackBird::setOnBird(bool x) 
{
	countOnBird = BLACKBIRD_COUNTONBIRD;
	state = BLACKBIRD_STATE_CARRYPLAYER;
}

bool CBlackBird::DropPlayer() 
{
	if (direction > 0) 
	{
		if (x >= endPoint) return true;
		return false;
	}
	else if (direction < 0) 
	{
		if (x <= endPoint) return true;
		return false;
	}
	return false;
}
#include "BigBoatWindow.h"
#include "PlayScene.h"
#include "Game.h"

void CBigBoatWindow::Render()
{
	int ani = BIG_BOAT_WINDOW_ANI;
	int alpha = 255;
	animation_set->at(ani)->Render(x, y, alpha);
	//RenderBoundingBox();
}

void CBigBoatWindow::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
	left = x;
	top = y;
	right = x + BIG_BOAT_WINDOW_BBOX_WIDTH;
	bottom = y - BIG_BOAT_WINDOW_BBOX_HEIGHT;
}
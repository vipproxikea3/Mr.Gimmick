#include "Window.h"

void CWindow::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects) {}

void CWindow::Render()
{
	int ani;

	switch (this->type)
	{
	case 1:
		ani = WINDOW_ANI_TYPE_1;
		break;
	case 2:
		ani = WINDOW_ANI_TYPE_2;
		break;
	case 3:
		ani = WINDOW_ANI_TYPE_3;
		break;
	case 4:
		ani = WINDOW_ANI_TYPE_4;
		break;
	default:
		ani = WINDOW_ANI_TYPE_1;
		break;
	}

	int alpha = 255;
	animation_set->at(ani)->Render(x, y, alpha);
	//RenderBoundingBox();
}

void CWindow::SetState(int state) {}

void CWindow::GetBoundingBox(float& l, float& t, float& r, float& b)
{
	l = x;
	t = y;
	r = l + WINDOW_BBOX_WIDTH;
	b = t - WINDOW_BBOX_HEIGHT;
}
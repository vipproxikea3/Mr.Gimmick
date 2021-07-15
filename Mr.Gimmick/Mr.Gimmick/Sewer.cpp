#include "Sewer.h"
#include "InclinedBrick.h"
#include "Gimmick.h"
#include "Utils.h"

/*int CSewer::Collision(LPGAMEOBJECT object, float dy) {
	if (!this->CheckAABB(object))
		return 0;
	if (object->x + 8 < this->x || object->x + 8 > this->x + 16)
		return 0;
	if (dynamic_cast<CGimmick*>(object) && ((CGimmick*)object)->jumping)
		return 0;

	float lx = this->x;
	float ly = this->y - this->ly;
	float rx = this->x + INCLINEDBRICK_BBOX_WIDTH;
	float ry = this->y - this->ry;

	float ox = object->x + 8.0f;

	float oy = (((ox - lx) * (ry - ly)) / (rx - lx)) + ly;

	if (object->y - 16 + dy < oy + 2.0f) {
		if (((ly > ry && lx + 8.0f > ox) && isT == -1) || ((ly < ry && rx - 8.0f < ox) && isT == 1))
			object->y = oy + 18.0f;
		else
			object->y = oy + 16.0f;
		/*if(object->vy < 0)*/ /*object->vy = 0.0f;
		object->onInclinedBrick = true;

		if (dynamic_cast<CGimmick*>(object)) {
			CGimmick* gimmick = dynamic_cast<CGimmick*>(object);
			gimmick->onGround = true;
			gimmick->falling = false;
		}
	}
	return this->direction;
}*/

void CSewer::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects) {}

void CSewer::Render()
{
	int ani;

	switch (this->type)
	{
	case 1:
		ani = SEWER_TYPE_1;
		break;
	case 2:
		ani = SEWER_TYPE_2;
		break;
	case 3:
		ani = SEWER_TYPE_3;
		break;
	case 4:
		ani = SEWER_TYPE_4;
		break;
	case 5:
		ani = SEWER_TYPE_5;
		break;
	case 6:
		ani = SEWER_TYPE_6;
		break;
	default:
		ani = SEWER_TYPE_1;
		break;
	}

	int alpha = 255;
	animation_set->at(ani)->Render(x, y, alpha);
	//RenderBoundingBox();
}

void CSewer::SetState(int state) {}

void CSewer::GetBoundingBox(float& l, float& t, float& r, float& b)
{
	l = x;
	t = y;
	r = x+SEWER_BBOX_WIDTH;
	b = y-SEWER_BBOX_HEIGHT;
}
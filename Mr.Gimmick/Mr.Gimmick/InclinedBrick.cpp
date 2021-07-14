#include "InclinedBrick.h"
#include "Gimmick.h"
#include "Utils.h"

int CInclinedBrick::Collision(LPGAMEOBJECT object, float dy) {
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

	float ox = object->x + 8;

	float oy = (((ox - lx) * (ry - ly)) / (rx - lx)) + ly;

	if (object->y - 16 + dy < oy + 2.0f) {
		if (((ly > ry && lx + 8.0f > ox) && isT == -1) || ((ly < ry && rx - 8.0f < ox) && isT == 1))
			object->y = oy + 21.0f;
		else
			object->y = oy + 16.0f;
		/*if(object->vy < 0)*/ object->vy = 0.0f;
		object->onInclinedBrick = true;

		if (dynamic_cast<CGimmick*>(object)) {
			CGimmick* gimmick = dynamic_cast<CGimmick*>(object);
			gimmick->onGround = true; 
			gimmick->falling = false;
		}
	}
	return this->direction;
}

void CInclinedBrick::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects) {}

void CInclinedBrick::Render()
{
	RenderBoundingBox();
}

void CInclinedBrick::SetState(int state) {}

void CInclinedBrick::GetBoundingBox(float& l, float& t, float& r, float& b)
{
	l = x;
	t = y;
	r = x + INCLINEDBRICK_BBOX_WIDTH;
	b = y - INCLINEDBRICK_BBOX_HEIGHT;
}
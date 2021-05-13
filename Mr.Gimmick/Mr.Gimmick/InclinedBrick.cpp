#include "InclinedBrick.h"
#include "Gimmick.h"
#include "Utils.h"

void CInclinedBrick::Collision(LPGAMEOBJECT object, float dy) {
	if (!this->CheckAABB(object))
		return;
	if (object->x + 8 < this->x || object->x + 8 > this->x + 16)
		return;

	float lx = this->x;
	float ly = this->y + this->ly;
	float rx = this->x + INCLINEDBRICK_BBOX_WIDTH;
	float ry = this->y + this->ry;

	float ox = object->x + 8.0f;

	float oy = (((ox - lx) * (ry - ly)) / (rx - lx)) + ly;

	if (object->y + 16 + dy > oy - 2.0f) {
		if (((ly < ry && lx + 8.0f > ox) && isT == -1) || ((ly > ry && rx - 8.0f < ox) && isT == 1))
			object->y = oy - 18.0f;
		else
			object->y = oy - 16.0f;
		object->vy = 0.0f;
		if (dynamic_cast<CGimmick*>(object)) {
			CGimmick* gimmick = dynamic_cast<CGimmick*>(object);
			gimmick->onInclinedBrick = true;
		}
	}
}

void CInclinedBrick::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects) {}

void CInclinedBrick::Render()
{
	//RenderBoundingBox();
}

void CInclinedBrick::SetState(int state) {}

void CInclinedBrick::GetBoundingBox(float& l, float& t, float& r, float& b)
{
	l = x;
	t = y;
	r = x + INCLINEDBRICK_BBOX_WIDTH;
	b = y + INCLINEDBRICK_BBOX_HEIGHT;
}
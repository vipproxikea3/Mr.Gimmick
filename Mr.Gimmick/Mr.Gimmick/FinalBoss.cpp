#include "FinalBoss.h"
#include "Utils.h"

CFinalBoss::CFinalBoss()
{
	nx = -1;
	SetState(FINALBOSS_STATE_CAPE);
}

void CFinalBoss::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	CGameObject::Update(dt, coObjects);

	vx += ax * dt;
	vy -= FINALBOSS_GRAVITY * dt;

	LimitPosition();
	DetectPlayer();
	CalculateSpeed();
	CalculateState();
	CalculateStun();
	CalculateBlink();
	DetectStar();
	SpecialCollisionWithPlayer();

	onGround = false;

	vector<LPGAMEOBJECT> newCoObjects;
	for (UINT i = 0; i < coObjects->size(); i++)
	{
		if (dynamic_cast<CBrick*>(coObjects->at(i))) newCoObjects.push_back(coObjects->at(i));
	}

	vector<LPCOLLISIONEVENT> coEvents;
	vector<LPCOLLISIONEVENT> coEventsResult;

	coEvents.clear();

	//if (state != FINALBOSS_STATE_DIE)
		CalcPotentialCollisions(&newCoObjects, coEvents);

	if (coEvents.size() == 0)
	{
		x += dx;
		y += dy;
	}
	else
	{
		float min_tx, min_ty, nx = 0, ny;
		float rdx = 0;
		float rdy = 0;

		float x0 = x;
		float y0 = y;

		FilterCollision(coEvents, coEventsResult, min_tx, min_ty, nx, ny, rdx, rdy);

		float l, t, r, b;
		GetBoundingBox(l, t, r, b);
		for (UINT i = 0; i < coEventsResult.size(); i++)
		{
			LPCOLLISIONEVENT e = coEventsResult[i];
			float ol, ot, or , ob;
			e->obj->GetBoundingBox(ol, ot, or , ob);
			if (e->ny > 0) onGround = true;

			if (dynamic_cast<CBrick*>(e->obj))
			{
				CBrick* brick = dynamic_cast<CBrick*>(e->obj);
				x = x0 + min_tx * dx + nx * 0.1f;
				y = y0 + min_ty * dy + ny * 0.1f;
				if (e->nx != 0)
				{
					vx = 0;
				}
				if (e->ny > 0) {
					vy = 0;
					onGround = true;
					if (state == FINALBOSS_STATE_JUMP_OUT)
						SetState(FINALBOSS_STATE_WALK);
					if (state == FINALBOSS_STATE_JUMP_IN)
						SetState(FINALBOSS_STATE_CHOP);
				}
			}
		}
	}
	// clean up newCoObjects
	for (UINT i = 0; i < newCoObjects.size(); i++) newCoObjects[i] = nullptr;
	// clean up collision events
	for (UINT i = 0; i < coEvents.size(); i++) delete coEvents[i];
}

void CFinalBoss::SetState(int state)
{
	CGameObject::SetState(state);
	switch (state)
	{
	case FINALBOSS_STATE_CAPE:
		cape_appear_start = GetTickCount64();
		break;
	case FINALBOSS_STATE_TRANSFORM:
		transform_start = GetTickCount64();
		break;
	case FINALBOSS_STATE_WALK:
		break;
	case FINALBOSS_STATE_JUMP_IN:
		ax = 0;
		if (nx < 0)
		{
			vx = -FINALBOSS_JUMP_SPEED_X;
		}
		else
		{
			vx = FINALBOSS_JUMP_SPEED_X;
		}
		vy = FINALBOSS_JUMP_SPEED_Y;
		break;
	case FINALBOSS_STATE_JUMP_OUT:
		ax = 0;
		if (nx < 0)
		{
			vx = FINALBOSS_JUMP_SPEED_X;
		}
		else
		{
			vx = -FINALBOSS_JUMP_SPEED_X;
		}
		vy = FINALBOSS_JUMP_SPEED_Y;
		break;
	case FINALBOSS_STATE_CHOP:
		ax = 0;
		vx = 0;
		chop_start = GetTickCount64();
		break;
	case FINALBOSS_STATE_SHOT_BIG_BULLET:
		ax = 0;
		vx = 0;
		shot_start = GetTickCount64();
		if (nx > 0)
			ShootBigBullet(1);
		else
			ShootBigBullet(-1);
		break;
	case FINALBOSS_STATE_SHOT_SMALL_BULLETS:
		ax = 0;
		vx = 0;
		shot_start = GetTickCount64();
		if (nx > 0)
			ShootSmallBullet(1);
		else
			ShootSmallBullet(-1);
		break;
	case FINALBOSS_STATE_STUN:
		vx = 0;
		ax = 0;
		vy = 0;
		stun_start = GetTickCount64();
		break;
	case FINALBOSS_STATE_DIE:
		vx = 0;
		ax = 0;
		vy = 0;
		die_start = GetTickCount64();
		break;
	}
}

void CFinalBoss::GetBoundingBox(float& l, float& t, float& r, float& b)
{
	l = x;
	t = y;
	r = x + FINALBOSS_BBOX_WIDTH;
	b = y - FINALBOSS_BBOX_HEIGHT;
}

void CFinalBoss::CalculateSpeed()
{
	CScene* scene = CGame::GetInstance()->GetCurrentScene();
	CGimmick* player = ((CPlayScene*)scene)->GetPlayer();

	float player_center = player->x + GIMMICK_BBOX_WIDTH / 2;
	float boss_center = x + FINALBOSS_BBOX_WIDTH / 2;

	// ===== WALK:
	// Gioi han toc do:
	if (state == FINALBOSS_STATE_WALK)
	{
		if (vx > FINALBOSS_WALK_SPEED)
			vx = FINALBOSS_WALK_SPEED;
		if (vx < -FINALBOSS_WALK_SPEED)
			vx = -FINALBOSS_WALK_SPEED;
	}

	//duoi theo gimmick:
	// WALK
	if (state == FINALBOSS_STATE_WALK && onGround)
	{
		if (nx < 0)
		{
			if (isEqual(player_center + FINALBOSS_PLAYER_DISTANCE, boss_center, 0.5f)) // so float cang nho thi cang dung dua (khong on dinh) 
			{
				vx = 0;
				ax = 0;
			}
			else if (player_center + FINALBOSS_PLAYER_DISTANCE < boss_center)
				ax = -FINALBOSS_ACCELERATION;
			else if (player_center + FINALBOSS_PLAYER_DISTANCE > boss_center)
				ax = FINALBOSS_ACCELERATION;
		}
		else
		{
			if (isEqual(player_center - FINALBOSS_PLAYER_DISTANCE, boss_center, 0.5f))
			{
				vx = 0;
				ax = 0;
			}
			else if (player_center - FINALBOSS_PLAYER_DISTANCE > boss_center)
				ax = FINALBOSS_ACCELERATION;
			else if (player_center - FINALBOSS_PLAYER_DISTANCE < boss_center)
				ax = -FINALBOSS_ACCELERATION;
		}
	}

	//neu gimmick lai gan qua thi chop, neu o ria map thi nhay / neu crazy thi chi nhay (distance xa hon)
	if (state == FINALBOSS_STATE_WALK && onGround)
	{
		if (nx < 0)
		{
			if (player_center + FINALBOSS_PLAYER_CANJUMP_DISTANCE >= boss_center) 
			{
				if (x + FINALBOSS_BBOX_WIDTH >= FINALBOSS_MAP_END_RIGHT - 32) // o ria map thi nhay
					SetState(FINALBOSS_STATE_JUMP_IN);
				else  
				{
					SetState(FINALBOSS_STATE_JUMP_OUT);
				}
			}
		}
		else
		{
			if (player_center - FINALBOSS_PLAYER_CANJUMP_DISTANCE <= boss_center)
			{
				if (x <= FINALBOSS_MAP_END_LEFT + 32)
					SetState(FINALBOSS_STATE_JUMP_IN);
				else
				{
					SetState(FINALBOSS_STATE_JUMP_OUT);
				}
			}
		}
	}
}

void CFinalBoss::DetectPlayer()
{
	CScene* scene = CGame::GetInstance()->GetCurrentScene();
	CGimmick* player = ((CPlayScene*)scene)->GetPlayer();

	// Duoi theo gimmick:
	if (player->x + GIMMICK_BBOX_WIDTH / 2 < x + FINALBOSS_BBOX_WIDTH / 2)
		nx = -1;
	else
		nx = 1;

}

void CFinalBoss::DetectStar()
{
	if (hide) return;

	CScene* scene = CGame::GetInstance()->GetCurrentScene();
	CStar* star = ((CPlayScene*)scene)->GetStar();

	float star_center = star->x + GIMMICK_BBOX_WIDTH / 2;
	float boss_center = x + FINALBOSS_BBOX_WIDTH / 2;

	//PHASE 1: CAPE
	if (state == FINALBOSS_STATE_CAPE && (star->state == STAR_STATE_WALKING_LEFT || star->state == STAR_STATE_WALKING_RIGHT))
	{
		if (CheckAABB(star))
		{
			if (life_counts > 6) // 6
			{
				is_stunning = true;
				stun_start = GetTickCount64();
			}
			else
			{
				SetState(FINALBOSS_STATE_TRANSFORM);
			}
			star->SetState(STAR_STATE_EXPLOSIVE);
			life_counts -= 1;
			
		}
	}

	//PHASE 2:
	// Nhay toi - jump in ne dan (o ria thi jump out)
	if (state == FINALBOSS_STATE_WALK)
	{
		if (star->state == STAR_STATE_WALKING_LEFT || star->state == STAR_STATE_WALKING_RIGHT) // nhay len ne dan
		{
			if (onGround ) {
				if (nx < 0 && boss_center > star_center && boss_center <= star_center + FINALBOSS_STAR_DETECT_RANGE)
				{
					if (x + FINALBOSS_BBOX_WIDTH >= FINALBOSS_MAP_END_RIGHT - 32) // o ria map thi nhay
						SetState(FINALBOSS_STATE_JUMP_IN);
					else
					{
						SetState(FINALBOSS_STATE_JUMP_OUT);
					}
				}
				else if (nx > 0 && boss_center < star_center && boss_center >= star_center - FINALBOSS_STAR_DETECT_RANGE)
				{
					if (x <= FINALBOSS_MAP_END_LEFT + 32)
						SetState(FINALBOSS_STATE_JUMP_IN);
					else
					{
						SetState(FINALBOSS_STATE_JUMP_OUT);
					}
				}
			}
		}
	}

	// Do dan bang light saber :
	if ((state == FINALBOSS_STATE_WALK || state == FINALBOSS_STATE_CHOP
		|| state == FINALBOSS_STATE_SHOT_BIG_BULLET || state == FINALBOSS_STATE_SHOT_SMALL_BULLETS)
		&& CheckSwordBlockStar()) // Do dan bang kiem
	{
		if (star->state == STAR_STATE_WALKING_LEFT || star->state == STAR_STATE_WALKING_RIGHT)
		{
			star->SetState(STAR_STATE_EXPLOSIVE);
		}
	}
	else if ((state != FINALBOSS_STATE_CAPE && state != FINALBOSS_STATE_TRANSFORM && state != FINALBOSS_STATE_DIE && state != FINALBOSS_STATE_STUN)) // ban trung heart
	{
		if ((star->state == STAR_STATE_WALKING_LEFT || star->state == STAR_STATE_WALKING_RIGHT )&& CheckStarHitHeart())
		{
			star->SetState(STAR_STATE_EXPLOSIVE);

			if (life_counts == 1) //1
			{
				//DIE
				SetState(FINALBOSS_STATE_DIE);
				CreateDieEffect();
			}
			else
			{
				SetState(FINALBOSS_STATE_STUN);
				life_counts -= 1;
			}
		}
	}
}

int CFinalBoss::CheckSideOfStar() // -1 left, 1 right dung de xac dinh huong die
{
	CScene* scene = CGame::GetInstance()->GetCurrentScene();
	CStar* star = ((CPlayScene*)scene)->GetStar();
	if (star->x < x)
		return -1;
	else
		return 1;
}

bool CFinalBoss::onTopOf(CGameObject* object, float equal)
{
	float ol, ot, or , ob;
	object->GetBoundingBox(ol, ot, or , ob);
	float l, t, r, b;
	GetBoundingBox(l, t, r, b);
	if (r >= ol && l <= or && abs(b - ot) < equal)
		return true;
	return false;
}

bool CFinalBoss::onSideOf(CGameObject* object, float equal)
{
	float ol, ot, or , ob;
	object->GetBoundingBox(ol, ot, or , ob);
	float l, t, r, b;
	GetBoundingBox(l, t, r, b);
	if (dynamic_cast<CBrick*>(object))
	{
		if (b <= ot && t >= ob && ((abs(l - or ) < equal && l >= or) || (abs(r - ol) < equal && r <= ol)))
			return true;
	}
	return false;
}

void CFinalBoss::SpecialCollisionWithPlayer()
{
	CScene* scene = CGame::GetInstance()->GetCurrentScene();
	CGimmick* player = ((CPlayScene*)scene)->GetPlayer();
	CStar* star = ((CPlayScene*)scene)->GetStar();

	if (player->state != GIMMICK_STATE_DIE && player->getUntouchable() != 1)
	{
		if (state == FINALBOSS_STATE_CAPE && CheckAABB(player))
		{
			if (x < player->x)
			{
				//player->vx = GIMMICK_DEFLECT_SPEED_X;
				//player->nx = 1.0;
			}
			else
			{
				//player->vx = -GIMMICK_DEFLECT_SPEED_X;
				//player->nx = -1.0;
			}

			player->SetState(GIMMICK_STATE_STUN);
			player->StartUntouchable();
		}
		else if (CheckHitPlayer() && (state == FINALBOSS_STATE_WALK || state == FINALBOSS_STATE_CHOP|| state == FINALBOSS_STATE_SHOT_BIG_BULLET || state == FINALBOSS_STATE_SHOT_SMALL_BULLETS))
		{
			if (x < player->x)
			{
				//player->vx = GIMMICK_DEFLECT_SPEED_X;
				//player->nx = 1.0;
			}
			else
			{
				//player->vx = -GIMMICK_DEFLECT_SPEED_X;
				//player->nx = -1.0;
			}

			player->SetState(GIMMICK_STATE_STUN);
			player->StartUntouchable();
		}
	}
}

bool CFinalBoss::CheckSwordBlockStar()
{
	CScene* scene = CGame::GetInstance()->GetCurrentScene();
	CStar* star = ((CPlayScene*)scene)->GetStar();

	float l, t, r, b;
	GetBoundingBox(l, t, r, b);
	if (state == FINALBOSS_STATE_WALK)
	{
		if (nx < 0)
		{
			l -= 14;
			t += 3;
			r = l + FINALBOSS_LIGHSABER_BBOX_WIDTH;
			b = t - FINALBOSS_LIGHSABER_BBOX_HEIGHT;
		}
		else
		{
			t += 3;
			r += 16;
			l = r - FINALBOSS_LIGHSABER_BBOX_WIDTH;
			b = t - FINALBOSS_LIGHSABER_BBOX_HEIGHT;
		}
	}
	else if (state == FINALBOSS_STATE_SHOT_SMALL_BULLETS)
	{
		if (nx < 0)
		{
			t += 12;
			r = l + FINALBOSS_LIGHSABER_SHOT_BBOX_WIDTH;
			b = t - FINALBOSS_LIGHSABER_SHOT_BBOX_HEIGHT;
		}
		else
		{
			t += 12;
			l = r - FINALBOSS_LIGHSABER_SHOT_BBOX_WIDTH;
			b = t - FINALBOSS_LIGHSABER_SHOT_BBOX_HEIGHT;
		}
	}
	else if (state == FINALBOSS_STATE_CHOP || state == FINALBOSS_STATE_SHOT_BIG_BULLET)
	{
		if (nx < 0)
		{
			t += 12;
			r = l + FINALBOSS_LIGHSABER_BBOX_WIDTH;
			b = t - FINALBOSS_BBOX_HEIGHT + 1;
		}
		else
		{
			l = r - 6;
			t += 12;
			r += 10;
			b = t - FINALBOSS_BBOX_HEIGHT + 1;
		}
	}
	else
	{
		l = t = r = b = 0; //cac state khac thi khong co light saber
	}
	

	float ol, ot, or , ob;
	star->GetBoundingBox(ol, ot, or , ob);

	if (or >= l && ol <= r
		&& ot >= b && ob <= t)
		return true;
	return false;
}

bool CFinalBoss::CheckHitPlayer()
{
	CScene* scene = CGame::GetInstance()->GetCurrentScene();
	CGimmick* player = ((CPlayScene*)scene)->GetPlayer();

	float l, t, r, b;
	GetBoundingBox(l, t, r, b);
	if (state == FINALBOSS_STATE_WALK)
	{
		if (nx < 0)
		{
			l -= 14;
			t += 3;
			r = l + FINALBOSS_LIGHSABER_BBOX_WIDTH;
			b = t - FINALBOSS_LIGHSABER_BBOX_HEIGHT;
		}
		else
		{
			t += 3;
			r += 16;
			l = r - FINALBOSS_LIGHSABER_BBOX_WIDTH;
			b = t - FINALBOSS_LIGHSABER_BBOX_HEIGHT;
		}
	}
	else if (state == FINALBOSS_STATE_SHOT_SMALL_BULLETS)
	{
		if (nx < 0)
		{
			t += 12;
			r = l + FINALBOSS_LIGHSABER_SHOT_BBOX_WIDTH;
			b = t - FINALBOSS_LIGHSABER_SHOT_BBOX_HEIGHT;
		}
		else
		{
			t += 12;
			l = r - FINALBOSS_LIGHSABER_SHOT_BBOX_WIDTH;
			b = t - FINALBOSS_LIGHSABER_SHOT_BBOX_HEIGHT;
		}
	}
	else if (state == FINALBOSS_STATE_CHOP || state == FINALBOSS_STATE_SHOT_BIG_BULLET)
	{
		if (nx < 0)
		{
			t += 12;
			r = l + FINALBOSS_LIGHSABER_BBOX_WIDTH;
			b = t - FINALBOSS_BBOX_HEIGHT + 1;
		}
		else
		{
			l = r - 6;
			t += 12;
			r += 10;
			b = t - FINALBOSS_BBOX_HEIGHT + 1;
		}
	}
	else
	{
		l = t = r = b = 0; //cac state khac thi khong co light saber
	}


	float ol, ot, or , ob;
	player->GetBoundingBox(ol, ot, or , ob);

	if (or >= l && ol <= r
		&& ot >= b && ob <= t)
		return true;
	return false;
}

bool CFinalBoss::CheckStarHitHeart()
{
	CScene* scene = CGame::GetInstance()->GetCurrentScene();
	CStar* star = ((CPlayScene*)scene)->GetStar();

	float l, t, r, b;
	GetBoundingBox(l, t, r, b);

	l = l + (FINALBOSS_BBOX_WIDTH - FINALBOSS_HEART_BBOX_WIDTH) / 2;
	t = t - (FINALBOSS_BBOX_HEIGHT - FINALBOSS_HEART_BBOX_HEIGHT) / 2 + 8;
	r = l + FINALBOSS_HEART_BBOX_WIDTH;
	b = t - FINALBOSS_HEART_BBOX_HEIGHT;


	float ol, ot, or , ob;
	star->GetBoundingBox(ol, ot, or , ob);

	if (or >= l && ol <= r
		&& ot >= b && ob <= t)
		return true;
	return false;
}

void CFinalBoss::Render()
{
	if (!visible) return;
	if (hide) return;

	//render heart:
	//RenderHeartBB();

	//RenderBoundingBox();
	int ani = 0;
	if (state == FINALBOSS_STATE_CAPE) { //==============WALK
		if (!is_stunning)
		{
			if (nx > 0)
				ani = FINALBOSS_ANI_CAPE_RIGHT;
			else
				ani = FINALBOSS_ANI_CAPE_LEFT;
		}
		else
		{
			if (nx > 0)
				ani = FINALBOSS_ANI_CAPE_STUN_RIGHT;
			else
				ani = FINALBOSS_ANI_CAPE_STUN_LEFT;
		}

	}
	else if (state == FINALBOSS_STATE_TRANSFORM)
	{
		if (nx > 0) {
			ani = FINALBOSS_ANI_TRANSFORM_RIGHT;
			animation_set->at(ani)->Render(x, y, alphaRender);
			return;
		}
		else {
			ani = FINALBOSS_ANI_TRANSFORM_LEFT;
			animation_set->at(ani)->Render(x - 14, y, alphaRender);
			return;
		}
	}
	else if (state == FINALBOSS_STATE_WALK) { //==============WALK
		{
			if (ax != 0)
			{
				if (nx > 0)
				{
					ani = FINALBOSS_ANI_WALK_RIGHT;
					//RenderBB(x + FINALBOSS_BBOX_WIDTH + 18 - FINALBOSS_LIGHSABER_BBOX_WIDTH, y + 3, x + FINALBOSS_BBOX_WIDTH + 18, y + 3 - FINALBOSS_LIGHSABER_BBOX_HEIGHT);
				}
				else
				{
					ani = FINALBOSS_ANI_WALK_LEFT;
					animation_set->at(ani)->Render(x - 14, y + 3, alphaRender);
					//RenderBB(x - 14, y + 3, x - 14 + 16, y + 3 - 17 );
					return;
				}
			}
			else
			{
				if (nx > 0)
				{
					ani = FINALBOSS_ANI_IDLE_RIGHT;
					//RenderBB(x + FINALBOSS_BBOX_WIDTH + 18 - FINALBOSS_LIGHSABER_BBOX_WIDTH, y + 3, x + FINALBOSS_BBOX_WIDTH + 18, y + 3 - FINALBOSS_LIGHSABER_BBOX_HEIGHT);
				}
				else
				{
					ani = FINALBOSS_ANI_IDLE_LEFT;
					animation_set->at(ani)->Render(x - 14, y + 3, alphaRender);
					//RenderBB(x - 14, y + 3, x - 14 + 16, y + 3 - 17 );
					return;
				}
			}

		}
	}
	else if (state == FINALBOSS_STATE_JUMP_OUT) {
		if (nx > 0)
			ani = FINALBOSS_ANI_JUMPOUT_RIGHT;
		else
			ani = FINALBOSS_ANI_JUMPOUT_LEFT;
	}
	else if (state == FINALBOSS_STATE_JUMP_IN) {
		if (nx > 0)
			ani = FINALBOSS_ANI_JUMPIN_RIGHT;
		else
			ani = FINALBOSS_ANI_JUMPIN_LEFT;
	}
	else if (state == FINALBOSS_STATE_CHOP || state == FINALBOSS_STATE_SHOT_BIG_BULLET) {
		if (nx > 0)
		{
			ani = FINALBOSS_ANI_CHOP_RIGHT;
			//RenderBB(x + FINALBOSS_BBOX_WIDTH - 6, y + 12, x + FINALBOSS_BBOX_WIDTH + 10, y + 12 - 65);
		}
		else
		{
			ani = FINALBOSS_ANI_CHOP_LEFT;
			//RenderBB(x, y + 12, x + 16, y + 12 - 65);
		}
		animation_set->at(ani)->Render(x, y + 12, alphaRender);
		return;
	}
	else if (state == FINALBOSS_STATE_SHOT_SMALL_BULLETS) {
		if (nx > 0)
		{
			ani = FINALBOSS_ANI_SHOT_RIGHT;
			//RenderBB(x + FINALBOSS_BBOX_WIDTH - 11, y + 12, x + FINALBOSS_BBOX_WIDTH, y + 12 - 20);
		}
		else
		{
			ani = FINALBOSS_ANI_SHOT_LEFT;
			//RenderBB(x, y + 12, x + 11, y + 12 - 20);
		}
		animation_set->at(ani)->Render(x, y + 12, alphaRender);
		return;
	}
	else if (state == FINALBOSS_STATE_STUN)
	{
		if (nx > 0) {
			ani = FINALBOSS_ANI_STUN_RIGHT;
		}
		else {
			ani = FINALBOSS_ANI_STUN_LEFT;
		}
	}
	else if (state == FINALBOSS_STATE_DIE)
	{
		if (nx > 0) {
			ani = FINALBOSS_ANI_DIE_RIGHT;
		}
		else {
			ani = FINALBOSS_ANI_DIE_LEFT;
		}
		animation_set->at(ani)->Render(x, y, alphaRender);
		return;
	}
	animation_set->at(ani)->Render(x, y + 3, alphaRender);

	//RenderBoundingBox();
}

bool CFinalBoss::isEqual(float a, float b, float equal)
{
	if (abs(a - b) <= equal)
		return true;
	return false;
}

void CFinalBoss::LimitPosition()
{
	if (vx != 0)
	{
		if (x + FINALBOSS_BBOX_WIDTH >= FINALBOSS_MAP_END_RIGHT && ax > 0) //fixed loi ko the truy duoi tiep
		{
			vx = 0;
		}
		if (x <= FINALBOSS_MAP_END_LEFT && ax < 0)
		{
			vx = 0;
		}
	}
}

void CFinalBoss::CalculateState()
{
	//Ban dan khi trang thai CAPE
	if (can_shoot && state == FINALBOSS_STATE_CAPE && GetTickCount64() - cape_appear_start >= FINALBOSS_CAPE_STARTSHOOT_TIME)
	{
		// ban dan:
		if (nx > 0)
			ShootBigBullet(1);
		else
			ShootBigBullet(-1);
		can_shoot = false;
	}
	//tinh thoi gian chuyen doi huong
	if (state == FINALBOSS_STATE_CAPE && GetTickCount64() - cape_appear_start >= FINALBOSS_CAPE_APPEAR_TIME)
	{
		if (!is_blinking)
			StartBlink();

		if (!hide && GetTickCount64() - cape_appear_start >= FINALBOSS_CAPE_APPEAR_TIME + FINALBOSS_BLINK_TIME)
		{
			hide = true; // tang hinh
		}

		if (hide && GetTickCount64() - cape_appear_start >= FINALBOSS_CAPE_APPEAR_TIME + FINALBOSS_BLINK_TIME + FINALBOSS_CAPE_HIDE_TIME)
		{
			hide = false;
			cape_appear_start = GetTickCount64();
			//doi ben dung:
			if (x < FINALBOSS_MAP_END_LEFT + (FINALBOSS_MAP_END_RIGHT - FINALBOSS_MAP_END_LEFT) / 2)
				x = FINALBOSS_CAPE_RIGHT_START_X;
			else
				x = FINALBOSS_CAPE_LEFT_START_X;
			can_shoot = true; // reset lan ban
		}
	}

	//Bien hinh :
	if (state == FINALBOSS_STATE_TRANSFORM && GetTickCount64() - transform_start >= FINALBOSS_TRANSFORM_TIME)
	{
		SetState(FINALBOSS_STATE_WALK);
		transform_start = 0;
		is_blinking = false;
		//bat dau auto ban dan:
		auto_bigshot_cooldown_start = GetTickCount64();
		auto_smallshot_cooldown_start = GetTickCount64();
	}

	//Tinh thoi gian auto ban dan to:(Auto Big Shot)
	if (state == FINALBOSS_STATE_WALK && GetTickCount64() - auto_bigshot_cooldown_start >= FINALBOSS_BIGSHOT_COOLDOWN)
	{
		SetState(FINALBOSS_STATE_SHOT_BIG_BULLET);
		auto_bigshot_cooldown_start = GetTickCount64(); // reset thoi gian ban dan to
	}
	//Tinh thoi gian auto ban dan nho:(Auto Small Shot)
	if (state == FINALBOSS_STATE_WALK && GetTickCount64() - auto_smallshot_cooldown_start >= FINALBOSS_SMALLSHOT_COOLDOWN)
	{
		SetState(FINALBOSS_STATE_SHOT_SMALL_BULLETS);
		auto_smallshot_cooldown_start = GetTickCount64(); // reset thoi gian ban dan to
	}

	// Chop:
	if (state == FINALBOSS_STATE_CHOP && GetTickCount64() - chop_start >= FINALBOSS_CHOP_TIME)
	{
		SetState(FINALBOSS_STATE_WALK);
		chop_start = 0;
	}
	// Big Shot va Small shot animation:
	if ((state == FINALBOSS_STATE_SHOT_BIG_BULLET || state== FINALBOSS_STATE_SHOT_SMALL_BULLETS) && GetTickCount64() - shot_start >= FINALBOSS_SHOT_TIME)
	{
		SetState(FINALBOSS_STATE_WALK);
		shot_start = 0;
	}

	// XXX=== DIE ===XXX:
	if (state == FINALBOSS_STATE_DIE && GetTickCount64() - die_start >= FINALBOSS_DIE_TIME)
	{
		die_start = 0;
		visible = false;
	}
	//if (state == FINALBOSS_STATE_DIE && GetTickCount64() - die_start >= FINALBOSS_DIE_TIME + 3000)
	//{
	//	die_start = 0;
	//	CGame::GetInstance()->SwitchScene(888);// Outro
	//}
}

void CFinalBoss::StartBlink()
{
	is_blinking = true;
	blink_start = GetTickCount64();
}

void CFinalBoss::CalculateBlink()
{
	if (!is_blinking)
		alphaRender = 255;
	else {
		if (((GetTickCount64() - blink_start) / 100) % 2 == 1) //nhap nhay lien tuc moi 100ms
			alphaRender = 0; //muc do nhap nhay
		else
			alphaRender = 255;
	}

	if (is_blinking && GetTickCount64() - blink_start >= FINALBOSS_BLINK_TIME) {
		is_blinking = false;
		blink_start = 0;
	}
}

void CFinalBoss::CalculateStun()
{
	// STUN khi state CAPE
	if (is_stunning && GetTickCount64() - stun_start >= FINALBOSS_STUN_TIME) {
		is_stunning = false;
		stun_start = 0;
	}

	// STUN Sau khi coi ao choang:
	if (state == FINALBOSS_STATE_STUN && GetTickCount64() - stun_start >= FINALBOSS_STUN_TIME)
	{
		SetState(FINALBOSS_STATE_WALK);
		stun_start = 0;
	}
}

void CFinalBoss::ShootBigBullet(int direction)
{
	CFinalBossBigBullet* bullet = new CFinalBossBigBullet(direction);
	//Vi tri cho dan
	bullet->SetPosition(x + (FINALBOSS_BBOX_WIDTH - FINALBOSS_BIG_BULLET_BBOX_WIDTH) / 2, y - FINALBOSS_BBOX_HEIGHT + FINALBOSS_BIG_BULLET_BBOX_HEIGHT );

	//Animation set cho quai
	CAnimationSets* animation_sets = CAnimationSets::GetInstance();
	LPANIMATION_SET ani_set = animation_sets->Get(FINALBOSS_BIG_BULLET_ANISET_ID);
	bullet->SetAnimationSet(ani_set);

	//them vao list object cua playscene
	CScene* scene = CGame::GetInstance()->GetCurrentScene();
	((CPlayScene*)scene)->PushBackObj(bullet);
}

void CFinalBoss::ShootSmallBullet(float direction)
{
	for (int i = 1; i <= 7; i++)
	{
		CFinalBossSmallBullet* bullet = new CFinalBossSmallBullet(direction * i * 0.2f, -1);
		//Vi tri cho dan
		if (nx < 0)
			bullet->SetPosition(x, y - FINALBOSS_LIGHSABER_BBOX_HEIGHT);
		else
			bullet->SetPosition(x + FINALBOSS_BBOX_WIDTH, y - FINALBOSS_LIGHSABER_BBOX_HEIGHT);

		//Animation set cho quai
		CAnimationSets* animation_sets = CAnimationSets::GetInstance();
		LPANIMATION_SET ani_set = animation_sets->Get(SMALLBULLET_ANISET_ID);
		bullet->SetAnimationSet(ani_set);

		//them vao list object cua playscene
		CScene* scene = CGame::GetInstance()->GetCurrentScene();
		((CPlayScene*)scene)->PushBackObj(bullet);
	}
	
}

void CFinalBoss::RenderBB(float l, float t, float r, float b)
{
	D3DXVECTOR3 p(x, y, 0);
	RECT rect;

	LPDIRECT3DTEXTURE9 bbox = CTextures::GetInstance()->Get(ID_TEX_BBOX);

	rect.left = 0;
	rect.top = 0;
	rect.right = (int)r - (int)l;
	rect.bottom = (int)t - (int)b;

	CGame::GetInstance()->Draw(l, t, bbox, rect.left, rect.top, rect.right, rect.bottom,200);
}

void CFinalBoss::RenderHeartBB()
{
	float l, t, r, b;
	GetBoundingBox(l, t, r, b);
	l = l + (FINALBOSS_BBOX_WIDTH - FINALBOSS_HEART_BBOX_WIDTH) / 2;
	t = t - (FINALBOSS_BBOX_HEIGHT - FINALBOSS_HEART_BBOX_HEIGHT) / 2 + 8;
	r = l + FINALBOSS_HEART_BBOX_WIDTH;
	b = t - FINALBOSS_HEART_BBOX_HEIGHT;
	RenderBB(l, t, r, b);
}

void CFinalBoss::CreateDieEffect()
{
	if (nx > 0)
	{
		CFinalBossDieEffect* effect = new CFinalBossDieEffect(x, y);
		//Animation set cho quai
		CAnimationSets* animation_sets = CAnimationSets::GetInstance();
		LPANIMATION_SET ani_set = animation_sets->Get(FINALBOSSDIEEFFECT_ANI_SET);
		effect->SetAnimationSet(ani_set);

		//them vao list object cua playscene
		CScene* scene = CGame::GetInstance()->GetCurrentScene();
		((CPlayScene*)scene)->PushBackObj(effect);
	}
	else
	{
		CFinalBossDieEffect* effect = new CFinalBossDieEffect(x+15, y);
		//Animation set cho quai
		CAnimationSets* animation_sets = CAnimationSets::GetInstance();
		LPANIMATION_SET ani_set = animation_sets->Get(FINALBOSSDIEEFFECT_ANI_SET);
		effect->SetAnimationSet(ani_set);

		//them vao list object cua playscene
		CScene* scene = CGame::GetInstance()->GetCurrentScene();
		((CPlayScene*)scene)->PushBackObj(effect);
	}

	
}



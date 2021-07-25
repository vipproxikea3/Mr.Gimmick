#include "Backup.h"
#include "Hud.h"
#include "Game.h"
#include "PlayScene.h"
#include "Gimmick.h"

CBackup* CBackup::__instance = NULL;

CBackup* CBackup::GetInstance()
{
	if (__instance == NULL) __instance = new CBackup();
	return __instance;
}

void CBackup::UpdateRest(int newRest) {
	this->rest = newRest;
	CHud* hud = ((CPlayScene*)CGame::GetInstance()->GetCurrentScene())->GetHud();
	hud->UpdateRest(this->rest);
}

void CBackup::UpdateLifeStack(int newLifeStack) {
	this->lifeStack = newLifeStack;
	if (this->lifeStack > 4)
		this->lifeStack = 4;
	if (this->lifeStack <= 0) {
		CGimmick* player = ((CPlayScene*)CGame::GetInstance()->GetCurrentScene())->GetPlayer();
		player->SetState(GIMMICK_STATE_DIE);
	}
	CHud* hud = ((CPlayScene*)CGame::GetInstance()->GetCurrentScene())->GetHud();
	switch (this->lifeStack)
	{
	case 1:
		hud->UpdateLight(3, 0, 0, 0);
		break;
	case 2:
		hud->UpdateLight(3, 3, 0, 0);
		break;
	case 3:
		hud->UpdateLight(3, 3, 3, 0);
		break;
	case 4:
		hud->UpdateLight(3, 3, 3, 3);
		break;
	default:
		hud->UpdateLight(0, 0, 0, 0);
		break;
	}
	hud->UpdateRest(this->rest);
}

void CBackup::UpdateScore(int score) {
	this->score = score;
	CHud* hud = ((CPlayScene*)CGame::GetInstance()->GetCurrentScene())->GetHud();
	hud->UpdateScore(this->score);
}

void CBackup::UpdateItem(int item) {
	this->item_3 = this->item_2;
	this->item_2 = this->item_1;
	this->item_1 = item;

	int tmp_item_1 = 0;
	int tmp_item_2 = 0;
	int tmp_item_3 = 0;

	switch (this->item_1)
	{
	case BACKUP_ITEM_TYPE_NULL:
		tmp_item_1 = 0;
		break;
	case BACKUP_ITEM_TYPE_MEDICINE:
		tmp_item_1 = 4;
		break;
	case BACKUP_ITEM_TYPE_BOOM:
		tmp_item_1 = 5;
		break;
	case BACKUP_ITEM_TYPE_CIRCLE:
		tmp_item_1 = 6;
		break;
	default:
		break;
	}

	switch (this->item_2)
	{
	case BACKUP_ITEM_TYPE_NULL:
		tmp_item_2 = 0;
		break;
	case BACKUP_ITEM_TYPE_MEDICINE:
		tmp_item_2 = 1;
		break;
	case BACKUP_ITEM_TYPE_BOOM:
		tmp_item_2 = 2;
		break;
	case BACKUP_ITEM_TYPE_CIRCLE:
		tmp_item_2 = 3;
		break;
	default:
		break;
	}

	switch (this->item_3)
	{
	case BACKUP_ITEM_TYPE_NULL:
		tmp_item_3 = 0;
		break;
	case BACKUP_ITEM_TYPE_MEDICINE:
		tmp_item_3 = 1;
		break;
	case BACKUP_ITEM_TYPE_BOOM:
		tmp_item_3 = 2;
		break;
	case BACKUP_ITEM_TYPE_CIRCLE:
		tmp_item_3 = 3;
		break;
	default:
		break;
	}

	CHud* hud = ((CPlayScene*)CGame::GetInstance()->GetCurrentScene())->GetHud();
	hud->UpdateItem(tmp_item_1, tmp_item_2, tmp_item_3);
}

void CBackup::ChangeItem() {
	int tmp_item = 0;
	if (this->item_3 == BACKUP_ITEM_TYPE_NULL && this->item_2 == BACKUP_ITEM_TYPE_NULL && this->item_1 == BACKUP_ITEM_TYPE_NULL)
		return;
	if (this->item_3 == BACKUP_ITEM_TYPE_NULL && this->item_2 == BACKUP_ITEM_TYPE_NULL)
		return;
	if (this->item_3 == BACKUP_ITEM_TYPE_NULL) {
		tmp_item = this->item_2;
		this->item_2 = this->item_1;
		this->item_1 = tmp_item;
	} else {
		tmp_item = this->item_3;
		this->item_3 = this->item_2;
		this->item_2 = this->item_1;
		this->item_1 = tmp_item;
	}

	int tmp_item_1 = 0;
	int tmp_item_2 = 0;
	int tmp_item_3 = 0;

	switch (this->item_1)
	{
	case BACKUP_ITEM_TYPE_NULL:
		tmp_item_1 = 0;
		break;
	case BACKUP_ITEM_TYPE_MEDICINE:
		tmp_item_1 = 4;
		break;
	case BACKUP_ITEM_TYPE_BOOM:
		tmp_item_1 = 5;
		break;
	case BACKUP_ITEM_TYPE_CIRCLE:
		tmp_item_1 = 6;
		break;
	default:
		break;
	}

	switch (this->item_2)
	{
	case BACKUP_ITEM_TYPE_NULL:
		tmp_item_2 = 0;
		break;
	case BACKUP_ITEM_TYPE_MEDICINE:
		tmp_item_2 = 1;
		break;
	case BACKUP_ITEM_TYPE_BOOM:
		tmp_item_2 = 2;
		break;
	case BACKUP_ITEM_TYPE_CIRCLE:
		tmp_item_2 = 3;
		break;
	default:
		break;
	}

	switch (this->item_3)
	{
	case BACKUP_ITEM_TYPE_NULL:
		tmp_item_3 = 0;
		break;
	case BACKUP_ITEM_TYPE_MEDICINE:
		tmp_item_3 = 1;
		break;
	case BACKUP_ITEM_TYPE_BOOM:
		tmp_item_3 = 2;
		break;
	case BACKUP_ITEM_TYPE_CIRCLE:
		tmp_item_3 = 3;
		break;
	default:
		break;
	}

	CHud* hud = ((CPlayScene*)CGame::GetInstance()->GetCurrentScene())->GetHud();
	hud->UpdateItem(tmp_item_1, tmp_item_2, tmp_item_3);
}
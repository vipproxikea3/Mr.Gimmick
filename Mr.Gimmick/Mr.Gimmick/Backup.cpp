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
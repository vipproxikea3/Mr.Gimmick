#pragma once
#include <d3d9.h>
#include <d3dx9.h>

#define BACKUP_ITEM_TYPE_NULL		0
#define BACKUP_ITEM_TYPE_MEDICINE	1
#define BACKUP_ITEM_TYPE_BOOM		2
#define BACKUP_ITEM_TYPE_CIRCLE		3

class CBackup
{
	static CBackup* __instance;
public:

	int item_1 = BACKUP_ITEM_TYPE_NULL;
	int item_2 = BACKUP_ITEM_TYPE_NULL;
	int item_3 = BACKUP_ITEM_TYPE_NULL;

	int end_scene_1 = 0;
	int end_scene_2 = 0;

	int rest = 5;
	int lifeStack = 4;
	int score = 0;

	void UpdateRest(int newRest);
	void UpdateLifeStack(int lifeStack);
	void UpdateScore(int score);
	void UpdateItem(int item);
	void ChangeItem();

	static CBackup* GetInstance();

	~CBackup();
};


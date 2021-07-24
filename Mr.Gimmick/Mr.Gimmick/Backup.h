#pragma once
#include <d3d9.h>
#include <d3dx9.h>

class CBackup
{
	static CBackup* __instance;
public:

	int end_scene_1 = 0;
	int end_scene_2 = 0;

	int rest = 5;
	int lifeStack = 4;
	int score = 0;

	void UpdateRest(int newRest);
	void UpdateLifeStack(int lifeStack);
	void UpdateScore(int score);

	static CBackup* GetInstance();

	~CBackup();
};


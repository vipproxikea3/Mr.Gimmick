#pragma once
#include <d3d9.h>
#include <d3dx9.h>

class CBackup
{
	static CBackup* __instance;
public:

	int rest = 5;
	int lifeStack = 4;

	void UpdateRest(int newRest);
	void UpdateLifeStack(int lifeStack);

	static CBackup* GetInstance();

	~CBackup();
};


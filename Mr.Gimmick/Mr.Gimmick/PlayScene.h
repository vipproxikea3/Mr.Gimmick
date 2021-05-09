#pragma once
#include "Game.h"
#include "Textures.h"
#include "Map.h"
#include "Scene.h"
#include "GameObject.h"
#include "Brick.h"
#include "Gimmick.h"
#include "CogwheelSmall.h"
#include "Cogwheel.h"
#include "Chain.h"
#include "BlueFire.h"


class CPlayScene : public CScene
{
protected:
	CGimmick* player;					// A play scene has to have player, right?
	Map* map;

	vector<LPGAMEOBJECT> objects;

	void _ParseSection_TEXTURES(string line);
	void _ParseSection_MAP(string line);
	void _ParseSection_SPRITES(string line);
	void _ParseSection_ANIMATIONS(string line);
	void _ParseSection_ANIMATION_SETS(string line);
	void _ParseSection_OBJECTS(string line);


public:
	CPlayScene(int id, LPCWSTR filePath);

	virtual void Load();
	virtual void Update(DWORD dt);
	virtual void Render();
	virtual void Unload();

	void SetCamPos();

	CGimmick* GetPlayer() { return player; }

	//friend class CPlayScenceKeyHandler;
};

class CPlaySceneKeyHandler : public CSceneKeyHandler
{
public:
	virtual void KeyState(BYTE* states);
	virtual void OnKeyDown(int KeyCode);
	virtual void OnKeyUp(int KeyCode) {};
	CPlaySceneKeyHandler(CScene* s) :CSceneKeyHandler(s) {};
};


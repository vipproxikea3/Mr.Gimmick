#pragma once
#include "Game.h"
#include "Quadtree.h"
#include "Textures.h"
#include "Map.h"
#include "Zone.h"
#include "Scene.h"
#include "GameObject.h"
#include "Brick.h"
#include "InclinedBrick.h"
#include "Gimmick.h"
#include "CogwheelSmall.h"
#include "Cogwheel.h"
#include "Chain.h"
#include "BlueFire.h"
#include "Conveyor.h"
#include "Tube.h"
#include "Window.h"
#include "Boom.h"
#include "Swing.h"
#include "Worm.h"
#include "Medicine.h"
#include "GimmickDieEffect.h"
#include "BrickPink.h"
#include "BlackEnemy.h"
#include "Hud.h"
#include "Star.h"
#include "Door.h"
#include "BlackBoss.h"
#include "Sewer.h"
#include "GreenBoss.h"
#include "Portal.h"

class CPlayScene : public CScene
{
protected:
	CGimmick* player = nullptr;					// A play scene has to have player, right?
	CStar* star = nullptr;
	
	CHud* hud = new CHud();
	//Map* map = nullptr;

	Map** map = nullptr;
	//unordered_map<int, Map*> map;
	int maptt = -1; // stt cua sprite map
	int spritemap = 4;// so luong sprite map
	int countfps = 0;//speed waterfall
	int fps = 1;

	vector<CZone*> zones;
	float ll = -9999.0f;
	float lt = 9999.0f;
	float lr = 9999.0f;
	float lb = -9999.0f;

	Quadtree* quadtree;

	vector<LPGAMEOBJECT> objects;

	void _ParseSection_TEXTURES(string line);
	void _ParseSection_MAP(string line);
	void _ParseSection_ZONES(string line);
	void _ParseSection_SPRITES(string line);
	void _ParseSection_ANIMATIONS(string line);
	void _ParseSection_ANIMATION_SETS(string line);
	void _ParseSection_OBJECTS(string line);
	void _ParseSection_SOUNDS(string line);


public:
	CPlayScene(int id, LPCWSTR filePath);

	virtual void Load();
	virtual void Update(DWORD dt);
	virtual void Render();
	virtual void Unload();

	void UpdateZone();
	void SetCamPos();

	void PushBackObj(CGameObject* obj);

	CGimmick* GetPlayer() { return player; };
	CStar* GetStar() { return star; };
};

class CPlaySceneKeyHandler : public CSceneKeyHandler
{
public:
	virtual void KeyState(BYTE* states);
	virtual void OnKeyDown(int KeyCode);
	virtual void OnKeyUp(int KeyCode);
	CPlaySceneKeyHandler(CScene* s) :CSceneKeyHandler(s) {};
};


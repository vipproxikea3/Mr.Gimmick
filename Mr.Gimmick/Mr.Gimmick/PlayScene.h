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
#include "Gun.h"
#include "Bullet.h"
#include "EnemyBoom.h"
#include "MiniBoom.h"
#include "SwordBoss.h"
#include "Sword.h"
#include "Boat.h"
#include "WaterDie.h"
#include "Boomboat.h"
#include "BigBoatWindow.h"
#include "ElectricBlackEnemy.h"
#include "Cannon.h"
#include "BoomCannon.h"
#include "Turle.h"
#include "BlackBird.h"
#include "StandBlackEnemy.h"
#include "Bridge.h"
#include "Bird.h"
#include "EnemyTail.h"
#include "SpecialBrick.h"
#include "Cat.h"
#include "FinalBoss.h"
#include "FinalBossSmallBullet.h"
#include "FinalBossDieEffect.h"
#include "CloudEnemy.h"

class CPlayScene : public CScene
{
protected:
	CGimmick* player = nullptr;					// A play scene has to have player, right?
	CStar* star = nullptr;

	CHud* hud = new CHud();
	//Map* map = nullptr;

	
	//unordered_map<int, Map*> map;

	//Map** map = nullptr;
	int maptt = 0; // stt cua sprite map
	int spritemap = 4;// so luong sprite map
	int countfps = 0;//speed waterfall
	int fps = 1;

	int attackBird = -1;

	vector<Map*> maps;

	vector<CZone*> zones;
	float ll = -9999.0f;
	float lt = 9999.0f;
	float lr = 9999.0f;
	float lb = -9999.0f;
	float revival_x = 0;
	float revival_y = 0;

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

	void GetRevivalPosition(float& x, float& y) {
		x = this->revival_x;
		y = this->revival_y;
	}

	void PushBackObj(CGameObject* obj);

	CGimmick* GetPlayer() { return player; };
	CStar* GetStar() { return star; };
	CHud* GetHud() { return hud; };
	void AttackBird() { attackBird = 3; }
};

class CPlaySceneKeyHandler : public CSceneKeyHandler
{
public:
	virtual void KeyState(BYTE* states);
	virtual void OnKeyDown(int KeyCode);
	virtual void OnKeyUp(int KeyCode);
	CPlaySceneKeyHandler(CScene* s) :CSceneKeyHandler(s) {};
};


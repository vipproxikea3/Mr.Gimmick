#pragma once
#include "Game.h"
#include "Textures.h"
#include "Map.h"
#include "Scene.h"
#include "GameObject.h"
#include "define.h"

class COutroScene : public CScene
{
protected:

	ULONGLONG outtro_start = 0;

	float gimmick_x; //Position so voi Intro
	float gimmick_y;

	bool end = false;

	bool isRenderingGimmick;
	bool isRenderingBall = false;

	int ball_ani = 1;

	vector<LPGAMEOBJECT> objects;

	void _ParseSection_TEXTURES(string line);
	void _ParseSection_SPRITES(string line);
	void _ParseSection_ANIMATIONS(string line);
	void _ParseSection_ANIMATION_SETS(string line);
	void _ParseSection_OBJECTS(string line);
	void _ParseSection_SOUNDS(string line);

public:
	int avoiInitialCount = 0; // bien de fix khi khoi tao scene chay update 2 lan
	COutroScene(int id, LPCWSTR filePath);

	virtual void Load();
	virtual void Update(DWORD dt);
	virtual void Render();
	virtual void Unload();

	void SetCamPos();
	void RenderMainOuttro();
	void RenderGimmick();
	void RenderBall();
	void getIntroPos(float& intro_x, float& intro_y);
	void Timing();
};

class COutroSceneKeyHandler : public CSceneKeyHandler
{
public:
	virtual void KeyState(BYTE* states);
	virtual void OnKeyDown(int KeyCode);
	virtual void OnKeyUp(int KeyCode) {};
	COutroSceneKeyHandler(CScene* s) :CSceneKeyHandler(s) {};
};


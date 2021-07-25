#pragma once
#include "Game.h"
#include "Textures.h"
#include "Scene.h"
#include "GameObject.h"
#include "define.h"

class CWorldMap : public CScene
{
protected:				// A play scene has to have player, right?


	ULONGLONG mainintro_start = 0;
	vector<LPGAMEOBJECT> objects;

	void _ParseSection_TEXTURES(string line);
	void _ParseSection_SPRITES(string line);
	void _ParseSection_ANIMATIONS(string line);
	void _ParseSection_ANIMATION_SETS(string line);
	void _ParseSection_OBJECTS(string line);
	void _ParseSection_SOUNDS(string line);

public:

	CWorldMap(int id, LPCWSTR filePath);

	virtual void Load();
	virtual void Update(DWORD dt);
	virtual void Render();
	virtual void Unload();

	void SetCamPos();
	void RenderMainScreen();
	void getIntroPos(float& intro_x, float& intro_y);
	void EndScene();

	//friend class CPlayScenceKeyHandler;
};

class CWorldMapKeyHandler : public CSceneKeyHandler
{
public:
	virtual void KeyState(BYTE* states);
	virtual void OnKeyDown(int KeyCode);
	virtual void OnKeyUp(int KeyCode) {};
	CWorldMapKeyHandler(CScene* s) :CSceneKeyHandler(s) {};
};


class CCloud : public CGameObject
{

public:
	int leftLimit = -50;
	float start = 300;
	CCloud(float start = 300)
	{
		vx = -0.025f;
		this->start = start;
	}
	~CCloud() {}
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
	{
		CGameObject::Update(dt);
		x += dx;
		if (x <= leftLimit && vx < 0)
		{
			x = start;
		}

	}
	virtual void Render()
	{
		animation_set->at(0)->Render(x, y, 255);
		//RenderBoundingBox();
	}
	//void SetState(int state);
	virtual void GetBoundingBox(float& l, float& t, float& r, float& b)
	{

			l = x;
			t = y;
			r = l + 16;
			b = t - 16;
	}
};


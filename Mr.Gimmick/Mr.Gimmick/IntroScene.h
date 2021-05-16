#pragma once
#include "Game.h"
#include "Textures.h"
#include "Map.h"
#include "Scene.h"
#include "GameObject.h"
#include "define.h"

class CIntroScene: public CScene
{
protected:				// A play scene has to have player, right?
	Map* map = nullptr;
	
	DWORD mainintro_start = 0;

	float gimmick_x; //Position so voi Intro
	float gimmick_y;

	bool isRenderingCake ;
	bool isRenderingGirlBlow ;
	bool isRenderingLghtingDisapear ;
	bool isRenderingGimmick;
	bool isRenderingGate = false;

	bool gimmick_fall_stopped = false;
	bool gimmick_bounce_stopped = false;
	bool gimmick_getup_stopped = false;

	bool bounce_up = true;
	float bounce_range = INTRO_BOUNCE_RANGE; //height of the bounce
	int bounces_count = 0;
	
	//gimmick:
	int gimmick_state; //state nay chi de quan ly render thoi, position phai tu sap xep


	vector<LPGAMEOBJECT> objects;

	void _ParseSection_TEXTURES(string line);
	void _ParseSection_MAP(string line);
	void _ParseSection_SPRITES(string line);
	void _ParseSection_ANIMATIONS(string line);
	void _ParseSection_ANIMATION_SETS(string line);
	void _ParseSection_OBJECTS(string line);


public:
	CIntroScene(int id, LPCWSTR filePath);

	virtual void Load();
	virtual void Update(DWORD dt);
	virtual void Render();
	virtual void Unload();

	void SetCamPos();
	void RenderMainIntro();
	void RenderCake();
	void RenderGirlBlow();
	void RenderLightingDissapear();
	void RenderGimmick();
	void RenderGate();
	void getIntroPos(float &intro_x, float &intro_y);
	void Timing();

	//friend class CPlayScenceKeyHandler;
};

class CIntroSceneKeyHandler : public CSceneKeyHandler
{
public:
	virtual void KeyState(BYTE* states);
	virtual void OnKeyDown(int KeyCode);
	virtual void OnKeyUp(int KeyCode) {};
	CIntroSceneKeyHandler (CScene* s) :CSceneKeyHandler(s) {};
};


#include <iostream>
#include <fstream>

#include "PlayScene.h"
#include "Utils.h"
#include "Textures.h"
#include "Sprites.h"
#include "IntroScene.h"
#include "define.h"
#include "Sound.h"

using namespace std;

CIntroScene::CIntroScene(int id, LPCWSTR filePath) :
	CScene(id, filePath)
{
	key_handler = new CIntroSceneKeyHandler(this);
	this->map = nullptr;

	isRenderingCake = false;
	isRenderingGirlBlow = false;
	isRenderingLghtingDisapear = false;
	isRenderingGimmick = false;
	
	gimmick_state = INTRO_GIMMICK_STATE_IDLE;
	gimmick_x = INTRO_GIMMICK_START_X;
	gimmick_y = INTRO_GIMMICK_START_Y;
}

/*
	Load scene resources from scene file (textures, sprites, animations and objects)
	See scene1.txt, scene2.txt for detail format specification
*/

#define SCENE_SECTION_UNKNOWN -1
#define SCENE_SECTION_TEXTURES 1
#define SCENE_SECTION_MAP 2
#define SCENE_SECTION_SPRITES 3
#define SCENE_SECTION_ANIMATIONS 4
#define SCENE_SECTION_ANIMATION_SETS	5
#define SCENE_SECTION_OBJECTS	6
#define SCENE_SECTION_SOUND		8

#define OBJECT_TYPE_BRICK			1
#define OBJECT_TYPE_GIMMICK			2
#define OBJECT_TYPE_COGWHEELSMALL	3
#define OBJECT_TYPE_COGWHEEL		4
#define OBJECT_TYPE_CHAIN			5
#define OBJECT_TYPE_BLUEFIRE		6
#define OBJECT_TYPE_INCLINEDBRICK	7
#define OBJECT_TYPE_CONVEYOR		8

#define MAX_SCENE_LINE 1024


void CIntroScene::_ParseSection_TEXTURES(string line)
{
	vector<string> tokens = split(line);

	if (tokens.size() < 5) return; // skip invalid lines

	int texID = atoi(tokens[0].c_str());
	wstring path = ToWSTR(tokens[1]);
	int R = atoi(tokens[2].c_str());
	int G = atoi(tokens[3].c_str());
	int B = atoi(tokens[4].c_str());

	CTextures::GetInstance()->Add(texID, path.c_str(), D3DCOLOR_XRGB(R, G, B));
}

void CIntroScene::_ParseSection_MAP(string line)
{
	vector<string> tokens = split(line);

	if (tokens.size() < 8) return; // skip invalid lines

	float PosX = atof(tokens[0].c_str());
	float PosY = atof(tokens[1].c_str());
	int TotalRowsOfMap = atoi(tokens[2].c_str());
	int TotalColumnsOfMap = atoi(tokens[3].c_str());
	int TotalRowsOfTileSet = atoi(tokens[4].c_str());
	int TotalColumnsOfTileSet = atoi(tokens[5].c_str());
	int TileSetID = atoi(tokens[6].c_str());
	wstring mapMatrixPath = ToWSTR(tokens[7]);

	this->map = new Map(TotalRowsOfMap, TotalColumnsOfMap, TotalRowsOfTileSet, TotalColumnsOfTileSet, TileSetID, mapMatrixPath);
	DebugOut(L"[INFO] Load map OK\n");
}

void CIntroScene::_ParseSection_SPRITES(string line)
{
	vector<string> tokens = split(line);

	if (tokens.size() < 6) return; // skip invalid lines

	int ID = atoi(tokens[0].c_str());
	int l = atoi(tokens[1].c_str());
	int t = atoi(tokens[2].c_str());
	int r = atoi(tokens[3].c_str());
	int b = atoi(tokens[4].c_str());
	int texID = atoi(tokens[5].c_str());

	LPDIRECT3DTEXTURE9 tex = CTextures::GetInstance()->Get(texID);
	if (tex == NULL)
	{
		DebugOut(L"[ERROR] Texture ID %d not found!\n", texID);
		return;
	}

	CSprites::GetInstance()->Add(ID, l, t, r, b, tex);
}

void CIntroScene::_ParseSection_ANIMATIONS(string line)
{
	vector<string> tokens = split(line);

	if (tokens.size() < 3) return; // skip invalid lines - an animation must at least has 1 frame and 1 frame time

	//DebugOut(L"--> %s\n",ToWSTR(line).c_str());

	LPANIMATION ani = new CAnimation();

	int ani_id = atoi(tokens[0].c_str());
	for (int i = 1; i < tokens.size(); i += 2)	// why i+=2 ?  sprite_id | frame_time  
	{
		int sprite_id = atoi(tokens[i].c_str());
		int frame_time = atoi(tokens[i + 1].c_str());
		ani->Add(sprite_id, frame_time);
	}

	CAnimations::GetInstance()->Add(ani_id, ani);
}

void CIntroScene::_ParseSection_ANIMATION_SETS(string line)
{
	vector<string> tokens = split(line);

	if (tokens.size() < 2) return; // skip invalid lines - an animation set must at least id and one animation id

	int ani_set_id = atoi(tokens[0].c_str());

	LPANIMATION_SET s = new CAnimationSet();

	CAnimations* animations = CAnimations::GetInstance();

	for (int i = 1; i < tokens.size(); i++)
	{
		int ani_id = atoi(tokens[i].c_str());

		LPANIMATION ani = animations->Get(ani_id);
		s->push_back(ani);
	}

	CAnimationSets::GetInstance()->Add(ani_set_id, s);
}

void CIntroScene::_ParseSection_SOUNDS(string line)
{
	Sound* sound = Sound::GetInstance();
	vector<string> tokens = split(line);

	if (tokens.size() < 2) return; // phai co ten va duong dan

	string filepath = tokens[0].c_str();
	string sound_name = tokens[1].c_str();
	sound->LoadSound(filepath, sound_name);
}

/*
	Parse a line in section [OBJECTS]
*/
void CIntroScene::_ParseSection_OBJECTS(string line)
{
	vector<string> tokens = split(line);

	//DebugOut(L"--> %s\n",ToWSTR(line).c_str());

	if (tokens.size() < 3) return; // skip invalid lines - an object set must have at least id, x, y

	int object_type = atoi(tokens[0].c_str());
	float x = atof(tokens[1].c_str());
	float y = atof(tokens[2].c_str());

	int ani_set_id = atoi(tokens[3].c_str());

	CAnimationSets* animation_sets = CAnimationSets::GetInstance();

	CGameObject* obj = NULL;

	switch (object_type)
	{
	case OBJECT_TYPE_BRICK: obj = new CBrick(atof(tokens[4].c_str()), atof(tokens[5].c_str())); break;
	case OBJECT_TYPE_COGWHEELSMALL: obj = new CCogwheelSmall(); break;
	case OBJECT_TYPE_COGWHEEL:
		obj = new CCogwheel(atoi(tokens[4].c_str()));
		break;
	case OBJECT_TYPE_CHAIN:
		obj = new CChain(atoi(tokens[4].c_str()));
		break;
	case OBJECT_TYPE_BLUEFIRE: obj = new CBlueFire(); break;
	case OBJECT_TYPE_INCLINEDBRICK:
		obj = new CInclinedBrick(atof(tokens[4].c_str()), atof(tokens[5].c_str()), atoi(tokens[6].c_str()));
		break;
	case OBJECT_TYPE_CONVEYOR:
		obj = new CConveyor(atoi(tokens[4].c_str()));
		break;
	default:
		DebugOut(L"[ERR] Invalid object type: %d\n", object_type);
		return;
	}

	// General object setup
	obj->SetPosition(x, y);

	LPANIMATION_SET ani_set = animation_sets->Get(ani_set_id);

	obj->SetAnimationSet(ani_set);
	objects.push_back(obj);
}

void CIntroScene::Load()
{
	DebugOut(L"[INFO] Start loading scene resources from : %s \n", sceneFilePath);

	ifstream f;
	f.open(sceneFilePath);

	// current resource section flag
	int section = SCENE_SECTION_UNKNOWN;

	char str[MAX_SCENE_LINE];
	while (f.getline(str, MAX_SCENE_LINE))
	{
		string line(str);

		if (line[0] == '#') continue;	// skip comment lines	

		if (line == "[TEXTURES]") { section = SCENE_SECTION_TEXTURES; continue; }
		if (line == "[MAP]") { section = SCENE_SECTION_MAP; continue; }
		if (line == "[SOUNDS]") { section = SCENE_SECTION_SOUND; continue; }
		if (line == "[SPRITES]") {
			section = SCENE_SECTION_SPRITES; continue;
		}
		if (line == "[ANIMATIONS]") {
			section = SCENE_SECTION_ANIMATIONS; continue;
		}
		if (line == "[ANIMATION_SETS]") {
			section = SCENE_SECTION_ANIMATION_SETS; continue;
		}
		if (line == "[OBJECTS]") {
			section = SCENE_SECTION_OBJECTS; continue;
		}
		if (line[0] == '[') { section = SCENE_SECTION_UNKNOWN; continue; }

		//
		// data section
		//
		switch (section)
		{
		case SCENE_SECTION_TEXTURES: _ParseSection_TEXTURES(line); break;
		case SCENE_SECTION_MAP: _ParseSection_MAP(line); break;
		case SCENE_SECTION_SOUND: _ParseSection_SOUNDS(line); break;
		case SCENE_SECTION_SPRITES: _ParseSection_SPRITES(line); break;
		case SCENE_SECTION_ANIMATIONS: _ParseSection_ANIMATIONS(line); break;
		case SCENE_SECTION_ANIMATION_SETS: _ParseSection_ANIMATION_SETS(line); break;
		case SCENE_SECTION_OBJECTS: _ParseSection_OBJECTS(line); break;
		}
	}

	f.close();

	CTextures::GetInstance()->Add(ID_TEX_BBOX, L"textures\\bbox.png", D3DCOLOR_XRGB(255, 255, 255));

	DebugOut(L"[INFO] Done loading scene resources %s\n", sceneFilePath);
	//Nhac nen
	Sound::GetInstance()->Play("SOUND_Stage1_Background", 1);
}

void CIntroScene::Update(DWORD dt)
{
	// We know that Mario is the first object in the list hence we won't add him into the colliable object list
	// TO-DO: This is a "dirty" way, need a more organized way 

	vector<LPGAMEOBJECT> coObjects;
	for (size_t i = 1; i < objects.size(); i++)
	{
		if (dynamic_cast<CGimmick*>(objects[i]))
			continue;
		coObjects.push_back(objects[i]);
	}

	for (size_t i = 0; i < objects.size(); i++)
	{
		if (dynamic_cast<CGimmick*>(objects[i]))
			continue;
		objects[i]->Update(dt, &coObjects);
	}

	SetCamPos();
	Timing();// Script for Main Intro
}

void CIntroScene::SetCamPos() {
	float y = CGame::GetInstance()->GetScreenHeight();
	CGame::GetInstance()->SetCamPos(0, y);
}

void CIntroScene::RenderMainIntro()
{
	float intro_x, intro_y;
	getIntroPos(intro_x, intro_y);

	CAnimations::GetInstance()->Get(INTRO_ANI_ID)->Render(intro_x, intro_y);
	//start count main intro time
	if(mainintro_start == 0)
		mainintro_start = GetTickCount64();
}

void CIntroScene::RenderCake()
{
	if (!isRenderingCake) return;

	float intro_x, intro_y;
	getIntroPos(intro_x, intro_y);

	float cake_x, cake_y;
	cake_x = intro_x + 56;
	cake_y = intro_y - 48;

	CAnimations::GetInstance()->Get(INTRO_CAKE_ANI_ID)->Render(cake_x, cake_y);
}

void CIntroScene::RenderGirlBlow()
{
	if (!isRenderingGirlBlow) return;

	float intro_x, intro_y;
	getIntroPos(intro_x, intro_y);

	float girl_x, girl_y;
	girl_x = intro_x + 56;
	girl_y = intro_y - 24;

	CSprites::GetInstance()->Get(INTRO_GIRL_BLOW_SPRITE_ID)->Draw(girl_x, girl_y);
}

void CIntroScene::RenderLightingDissapear()
{
	if (!isRenderingLghtingDisapear) return;

	float intro_x, intro_y;
	getIntroPos(intro_x, intro_y);

	float ani_x, ani_y;
	ani_x = intro_x + 48;
	ani_y = intro_y - 48;

	CAnimations::GetInstance()->Get(INTRO_LIGHTNING_DISSAPEAR_ANI_ID)->Render(ani_x, ani_y);
}

void CIntroScene::RenderGimmick()
{
	if (!isRenderingGimmick) return;

	float intro_x, intro_y;
	getIntroPos(intro_x, intro_y);
	
	switch (gimmick_state)
	{
	case INTRO_GIMMICK_STATE_IDLE:
		CAnimations::GetInstance()->Get(INTRO_GIMMICK_ANI_IDLE)->Render(intro_x + gimmick_x, intro_y - gimmick_y);
		break;
	case INTRO_GIMMICK_STATE_FALL:
		CAnimations::GetInstance()->Get(INTRO_GIMMICK_ANI_FALL)->Render(intro_x + gimmick_x, intro_y - gimmick_y);
		break;
	case INTRO_GIMMICK_STATE_BOUNCE:
		CAnimations::GetInstance()->Get(INTRO_GIMMICK_ANI_BOUNCE)->Render(intro_x + gimmick_x, intro_y - gimmick_y);
		break;
	case INTRO_GIMMICK_STATE_GETUP:
		CAnimations::GetInstance()->Get(INTRO_GIMMICK_ANI_GETUP)->Render(intro_x + gimmick_x, intro_y - gimmick_y);
		break;
	case INTRO_GIMMICK_STATE_IDLE_LOOKUP:
		CAnimations::GetInstance()->Get(INTRO_GIMMICK_ANI_IDLE_LOOKUP)->Render(intro_x + gimmick_x, intro_y - gimmick_y);
		break;
	case INTRO_GIMMICK_STATE_JUMP:
		CAnimations::GetInstance()->Get(INTRO_GIMMICK_ANI_JUMP)->Render(intro_x + gimmick_x, intro_y - gimmick_y);
		break;
	}
}

void CIntroScene::RenderGate()
{
	if (!isRenderingGate) return;

	float intro_x, intro_y;
	getIntroPos(intro_x, intro_y);

	CAnimations::GetInstance()->Get(INTRO_GATE_ANI_ID)->Render(intro_x, intro_y);
}

void CIntroScene::RenderMainScreen()
{
	float camx, camy;
	CGame::GetInstance()->GetCamPos(camx, camy);

	CAnimations::GetInstance()->Get(INTRO_MAINSCREEN_ANI_ID)->Render(camx, camy);
}

void CIntroScene::getIntroPos(float &intro_x, float &intro_y)
{
	float screen_width = CGame::GetInstance()->GetScreenWidth();
	float screen_height = CGame::GetInstance()->GetScreenHeight();

	intro_x = (screen_width - INTRO_WIDTH) / 2;
	intro_y = (screen_height + INTRO_HEIGHT) / 2;
}

void CIntroScene::Timing()
{
	//Cake:
	if (GetTickCount64() - mainintro_start >= FRAME_1_TIME
		&& GetTickCount64() - mainintro_start < FRAME_1_TIME + FRAME_CAKE_TIME
		&& !isRenderingCake)
		isRenderingCake = true;
	else
		isRenderingCake = false;

	//Girl Blow Candle:
	if (GetTickCount64() - mainintro_start >= FRAME_1_TIME + FRAME_CAKE_TIME/2 //start
		&& GetTickCount64() - mainintro_start < FRAME_1_TIME + FRAME_CAKE_TIME + FRAME_GIRL_BLOW_TIME //end
		&& !isRenderingGirlBlow)
		isRenderingGirlBlow = true;
	if (GetTickCount64() - mainintro_start > FRAME_1_TIME + FRAME_CAKE_TIME + FRAME_GIRL_BLOW_TIME)
		isRenderingGirlBlow = false;

	//Lightning Dissapear:
	if (GetTickCount64() - mainintro_start >= FRAME_12_DISSAPEAR_START_TIME //start
		&& GetTickCount64() - mainintro_start < FRAME_12_DISSAPEAR_START_TIME + FRAME_12_DISSAPEAR_TIME //end
		&& !isRenderingLghtingDisapear)
		isRenderingLghtingDisapear = true;
	if (GetTickCount64() - mainintro_start > FRAME_12_DISSAPEAR_START_TIME + FRAME_12_DISSAPEAR_TIME)
		isRenderingLghtingDisapear = false;
	//Gate Open and Close
	if (GetTickCount64() - mainintro_start >= FRAME_14_GATE_OPEN_START_TIME //start
		&& GetTickCount64() - mainintro_start < FRAME_14_GATE_OPEN_START_TIME + FRAME_14_GATE_TIME //end
		&& !isRenderingGate)
		isRenderingGate = true;
	if (GetTickCount64() - mainintro_start >= FRAME_14_GATE_OPEN_START_TIME + FRAME_14_GATE_TIME)
		isRenderingGate = false;
	//Gimmick fall and get up:
	//MANAGE GIMMICK APPEAR TIME FRAME 12:
	if (GetTickCount64() - mainintro_start >= FRAME_12_GIMMICK_START_TIME //start
		&& GetTickCount64() - mainintro_start < FRAME_12_GIMMICK_START_TIME + FRAME_12_GIMMICK_TIME //end
		&& !isRenderingGimmick)
	{
		isRenderingGimmick = true;
		gimmick_x = INTRO_GIMMICK_START_X;
		gimmick_y = INTRO_GIMMICK_START_Y;
		gimmick_state = INTRO_GIMMICK_STATE_IDLE;
	}
	if (GetTickCount64() - mainintro_start > FRAME_12_GIMMICK_START_TIME + FRAME_12_GIMMICK_TIME)
		isRenderingGimmick = false;
	//FALL:
	if (GetTickCount64() - mainintro_start >= FRAME_12_GIMMICK_FALL_START_TIME && !gimmick_fall_stopped)
	{
		if (gimmick_y < INTRO_GIMMICK_STOP_FALL_Y - INTRO_GIMMICK_HEIGHT)
		{
			gimmick_state = INTRO_GIMMICK_STATE_FALL;
			gimmick_y += 1.2f;
		}
		else
			gimmick_fall_stopped = true;
	}
	//BOUNCE:
	if (gimmick_fall_stopped && !gimmick_bounce_stopped)
	{
		gimmick_state = INTRO_GIMMICK_STATE_BOUNCE;
		if (bounces_count >= 2)
		{
			gimmick_bounce_stopped = true;
			bounce_up = true;//reset bounce direction
			return;
		}
		if (gimmick_y >= INTRO_GIMMICK_STOP_FALL_Y - INTRO_GIMMICK_HEIGHT && !bounce_up)
		{
			bounce_up = true;
			bounces_count++;
		}
		if (gimmick_y <= INTRO_GIMMICK_STOP_FALL_Y - INTRO_GIMMICK_HEIGHT - bounce_range && bounce_up)
		{
			bounce_up = false;
			bounce_range = INTRO_SMALL_BOUNCE_RANGE; //giam do nay cho lan nay tiep theo
		}

		if (bounce_up) //nay len nhanh nay xuong cham
		{
			gimmick_y -= 1.5f;
			gimmick_x -= 0.1f;
		}
		else
		{
			gimmick_y += 0.5;
			gimmick_x -= 0.1f;
		}
	}
	//GET UP:
	if (gimmick_fall_stopped && gimmick_bounce_stopped && !gimmick_getup_stopped)
	{
		gimmick_state = INTRO_GIMMICK_STATE_GETUP;
	}

	//MANAGE GIMMICK APPEAR TIME FRAME 14:
	if (GetTickCount64() - mainintro_start >= FRAME_14_GIMMICK_START_TIME //start
		&& GetTickCount64() - mainintro_start < FRAME_14_GIMMICK_START_TIME + FRAME_14_GIMMICK_TIME //end
		&& !isRenderingGimmick)
	{
		isRenderingGimmick = true;
	}
	if (GetTickCount64() - mainintro_start > FRAME_14_GIMMICK_START_TIME + FRAME_14_GIMMICK_TIME)
		isRenderingGimmick = false;
	//LOOK UP
	if (GetTickCount64() - mainintro_start >= FRAME_14_GIMMICK_START_TIME && GetTickCount64() - mainintro_start <= FRAME_14_GIMMICK_START_TIME + FRAME_14_GIMMICK_LOOKUP_TIME) //1s sau nhay
	{
		gimmick_state = INTRO_GIMMICK_STATE_IDLE_LOOKUP;
	}
	//JUMP:
	if (GetTickCount64() - mainintro_start > FRAME_14_GIMMICK_START_TIME + FRAME_14_GIMMICK_LOOKUP_TIME)
	{
		gimmick_state = INTRO_GIMMICK_STATE_JUMP;
		
		if (gimmick_y <= INTRO_GIMMICK_STOP_FALL_Y - INTRO_GIMMICK_HEIGHT -INTRO_JUMP_RANGE_Y)
			bounce_up = false;

		gimmick_x -= 0.5f;
		if (bounce_up)
			gimmick_y -= 1.5f;
		else
			gimmick_y += 1.5f;
	}

	//END INTRO:
	if (GetTickCount64() - mainintro_start >= INTRO_TIME - 2000 ) // so default de khi khoi chay ko vao day
	{
		DebugOut(L"end intro \n");
		avoiInitialCount += 1;
		
		if(avoiInitialCount == 3 && phase == 1) phase = 2;
	}
}

void CIntroScene::EndScene()
{
	CGame::GetInstance()->SwitchScene(1);
}

void CIntroScene::Render()
{
	if (map)
		map->Render();

	for (int i = 0; i < objects.size(); i++)
		if (!dynamic_cast<CGimmick*>(objects[i]))
			objects[i]->Render();
	//Render Intro:
	if (phase == 1)
	{
		RenderMainIntro();
		RenderCake();
		RenderGirlBlow();
		RenderLightingDissapear();
		RenderGate();//before gimmick
		RenderGimmick();
	}
	else
	{
		RenderMainScreen();
	}
}

/*
	Unload current scene
*/
void CIntroScene::Unload()
{
	for (int i = 0; i < objects.size(); i++)
		delete objects[i];

	objects.clear();

	DebugOut(L"[INFO] Scene %s unloaded! \n", sceneFilePath);
}

void CIntroSceneKeyHandler::OnKeyDown(int KeyCode)
{
	//DebugOut(L"[INFO] KeyDown: %d\n", KeyCode);
	CGame* game = CGame::GetInstance();
	CIntroScene* intro_scene = (CIntroScene*)game->GetCurrentScene();
	switch (KeyCode)
	{
	case DIK_S:
		if (intro_scene->phase == 1)
		{
			intro_scene->phase = 2;
		}
		else
			game->SwitchScene(1);
		break;
	}
}

void CIntroSceneKeyHandler::KeyState(BYTE* states)
{
	CGame* game = CGame::GetInstance();

	if (game->IsKeyDown(DIK_RIGHT))
	{

	}
}
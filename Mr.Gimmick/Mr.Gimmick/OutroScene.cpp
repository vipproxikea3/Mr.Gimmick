#include "OutroScene.h"
#include <iostream>
#include <fstream>

#include "Utils.h"
#include "Textures.h"
#include "Sprites.h"
#include "IntroScene.h"
#include "define.h"
#include "Sound.h"
#include "Gimmick.h"

using namespace std;

COutroScene::COutroScene(int id, LPCWSTR filePath) :
	CScene(id, filePath)
{
	key_handler = new COutroSceneKeyHandler(this);


	gimmick_x = OUTRO_GIMMICK_START_X;
	gimmick_y = OUTRO_GIMMICK_START_Y;
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


void COutroScene::_ParseSection_TEXTURES(string line)
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

void COutroScene::_ParseSection_SPRITES(string line)
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

void COutroScene::_ParseSection_ANIMATIONS(string line)
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

void COutroScene::_ParseSection_ANIMATION_SETS(string line)
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

void COutroScene::_ParseSection_SOUNDS(string line)
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
void COutroScene::_ParseSection_OBJECTS(string line)
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
	/*case OBJECT_TYPE_BRICK: obj = new CBrick(atof(tokens[4].c_str()), atof(tokens[5].c_str())); break;
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
		break;*/
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

void COutroScene::Load()
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
	//Sound::GetInstance()->Play("SOUND_Stage1_Background", 1);
	Sound::GetInstance()->Play("SOUND_Ending_2", 1);
}

void COutroScene::Update(DWORD dt)
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
	isRenderingBall = false;
	isRenderingGimmick = false;
	SetCamPos();
	Timing();// Script for Main Intro
}

void COutroScene::SetCamPos() {
	float y = CGame::GetInstance()->GetScreenHeight();
	CGame::GetInstance()->SetCamPos(0, y);
}

void COutroScene::RenderMainOuttro()
{
	float outro_x, outro_y;
	getIntroPos(outro_x, outro_y);

	CAnimations::GetInstance()->Get(OUTRO_ANI_MAIN_ID)->Render(outro_x, outro_y);
	//start count main intro time
	if (outtro_start == 0)
	{
		outtro_start = GetTickCount64();
		DebugOut(L"start \n");
	}
}

void COutroScene::RenderGimmick()
{
	if (!isRenderingGimmick) return;

	float outro_x, outro_y;
	getIntroPos(outro_x, outro_y);

	CAnimations::GetInstance()->Get(OUTRO_ANI_GIMMICK_WALK_RIGHT)->Render(outro_x + gimmick_x, outro_y - gimmick_y);

}

void COutroScene::RenderBall()
{
	if (!isRenderingBall) return;

	float outro_x, outro_y;
	getIntroPos(outro_x, outro_y);

	if(ball_ani == 1)
		CAnimations::GetInstance()->Get(OUTRO_ANI_BALLBREAK_1)->Render(outro_x + OUTRO_BALL_START_X, outro_y - OUTRO_BALL_START_Y);
	else if(ball_ani == 2)
		CAnimations::GetInstance()->Get(OUTRO_ANI_BALLBREAK_2)->Render(outro_x + OUTRO_BALL_START_X, outro_y - OUTRO_BALL_START_Y);
	else
		CAnimations::GetInstance()->Get(OUTRO_ANI_BALLBREAK_3)->Render(outro_x + OUTRO_BALL_START_X, outro_y - OUTRO_BALL_START_Y);

}

void COutroScene::getIntroPos(float& intro_x, float& intro_y)
{
	float screen_width = CGame::GetInstance()->GetScreenWidth();
	float screen_height = CGame::GetInstance()->GetScreenHeight();

	intro_x = (screen_width - INTRO_WIDTH) / 2;
	intro_y = (screen_height + INTRO_HEIGHT) / 2;
}

void COutroScene::Timing()
{
	//if (avoiInitialCount >= 2) // fix loi moi vo update 2 lan bang bien avoid initialcount
	//{
	//	
	//}

	//  ================= MAIN OUTRO:
	//Di bo
	if (GetTickCount64() - outtro_start < OUTRO_FRAME_1_TIME && GetTickCount64() - outtro_start > 0)
	{
		if (!isRenderingGimmick)
			isRenderingGimmick = true;
		gimmick_x += 1;
	}
	else if (GetTickCount64() - outtro_start >= OUTRO_FRAME_3_START && GetTickCount64() - outtro_start <= OUTRO_FRAME_3_START + OUTRO_FRAME_1_TIME) // so default de khi khoi chay ko vao day
	{
		if (!isRenderingGimmick)
			isRenderingGimmick = true;
		gimmick_x += 1;
	}
	else
		gimmick_x = OUTRO_GIMMICK_START_X;

	// ================ BALL
	// 1st Appear:
	if (GetTickCount64() - outtro_start >= OUTRO_FRAME_1_TIME && GetTickCount64() - outtro_start <= OUTRO_FRAME_1_TIME + 2000) // so default de khi khoi chay ko vao day
	{
		ball_ani = 1;
		if (!isRenderingBall)
			isRenderingBall = true;

	}
	// 2st Appear
	if (GetTickCount64() - outtro_start >= OUTRO_FRAME_10_START  && GetTickCount64() - outtro_start <= OUTRO_FRAME_10_START + 2000) // so default de khi khoi chay ko vao day
	{
		ball_ani = 2;
		if (!isRenderingBall)
			isRenderingBall = true;

	}
	//3rd Appear
	if (GetTickCount64() - outtro_start >= OUTRO_FRAME_12_START  && GetTickCount64() - outtro_start <= OUTRO_FRAME_12_START + 3000) // so default de khi khoi chay ko vao day
	{
		ball_ani = 3;
		if (!isRenderingBall)
			isRenderingBall = true;

	}

	// ============== END OUTRO:
	if (GetTickCount64() - outtro_start >= OUTRO_TIME ) // so default de khi khoi chay ko vao day
	{
		if (avoiInitialCount >= 2)
		{
			DebugOut(L"end intro \n");
			end = true;
			//STOP MUSIC RIGHT HERE :
		}
		avoiInitialCount += 1;

	}
}

void COutroScene::Render()
{
	for (int i = 0; i < objects.size(); i++)
		if (!dynamic_cast<CGimmick*>(objects[i]))
			objects[i]->Render();
	//Render Intro:
	if (!end)
	{
		RenderMainOuttro();
		RenderGimmick();
		RenderBall();
	}
}

/*
	Unload current scene
*/
void COutroScene::Unload()
{
	for (int i = 0; i < objects.size(); i++)
		delete objects[i];

	objects.clear();

	DebugOut(L"[INFO] Scene %s unloaded! \n", sceneFilePath);
}

void COutroSceneKeyHandler::OnKeyDown(int KeyCode)
{
	//DebugOut(L"[INFO] KeyDown: %d\n", KeyCode);
	CGame* game = CGame::GetInstance();
	COutroScene* intro_scene = (COutroScene*)game->GetCurrentScene();
	switch (KeyCode)
	{
	case DIK_S:
		game->SwitchScene(INTRO_SCENE_ID);
		break;
	}
}

void COutroSceneKeyHandler::KeyState(BYTE* states)
{
	CGame* game = CGame::GetInstance();

	if (game->IsKeyDown(DIK_RIGHT))
	{

	}
}

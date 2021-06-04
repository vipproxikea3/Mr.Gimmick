#include <iostream>
#include <fstream>

#include "PlayScene.h"
#include "Utils.h"
#include "Textures.h"
#include "Sprites.h"
#include "Map.h"
#include "Sound.h"

using namespace std;

CPlayScene::CPlayScene(int id, LPCWSTR filePath) :
	CScene(id, filePath)
{
	key_handler = new CPlaySceneKeyHandler(this);
	this->player = nullptr;
	this->map = nullptr;
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
#define SCENE_SECTION_ZONES	7
#define SCENE_SECTION_SOUND		8

#define OBJECT_TYPE_BRICK			1
#define OBJECT_TYPE_GIMMICK			2
#define OBJECT_TYPE_COGWHEELSMALL	3
#define OBJECT_TYPE_COGWHEEL		4
#define OBJECT_TYPE_CHAIN			5
#define OBJECT_TYPE_BLUEFIRE		6
#define OBJECT_TYPE_INCLINEDBRICK	7
#define OBJECT_TYPE_CONVEYOR		8
#define OBJECT_TYPE_TUBE			9
#define OBJECT_TYPE_WINDOW			10
#define OBJECT_TYPE_BOOM			11
#define OBJECT_TYPE_WORM			12
#define OBJECT_TYPE_SWING			15
#define OBJECT_TYPE_MEDICINE		16

#define MAX_SCENE_LINE 1024


void CPlayScene::_ParseSection_TEXTURES(string line)
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

void CPlayScene::_ParseSection_MAP(string line)
{
	vector<string> tokens = split(line);

	if (tokens.size() < 6) return; // skip invalid lines

	int TotalRowsOfMap = atoi(tokens[0].c_str());
	int TotalColumnsOfMap = atoi(tokens[1].c_str());
	int TotalRowsOfTileSet = atoi(tokens[2].c_str());
	int TotalColumnsOfTileSet = atoi(tokens[3].c_str());
	int TileSetID = atoi(tokens[4].c_str());
	wstring mapMatrixPath = ToWSTR(tokens[5]);

	if (maptt == -1)
		this->map = new Map * [spritemap];
	maptt++;
	this->map[maptt] = new Map(TotalRowsOfMap, TotalColumnsOfMap, TotalRowsOfTileSet, TotalColumnsOfTileSet, TileSetID, mapMatrixPath);
	DebugOut(L"[INFO] Load map OK\n");
}

void CPlayScene::_ParseSection_ZONES(string line)
{
	vector<string> tokens = split(line);

	if (tokens.size() < 4) return; // skip invalid lines

	float l = (float)atof(tokens[0].c_str());
	float t = (float)atof(tokens[1].c_str());
	float r = (float)atof(tokens[2].c_str());
	float b = (float)atof(tokens[3].c_str());

	CZone* zone = new CZone(l, t, r, b);
	zones.push_back(zone);
}

void CPlayScene::_ParseSection_SPRITES(string line)
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

void CPlayScene::_ParseSection_ANIMATIONS(string line)
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

void CPlayScene::_ParseSection_ANIMATION_SETS(string line)
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

void CPlayScene::_ParseSection_SOUNDS(string line)
{
	Sound *sound = Sound::GetInstance();
	vector<string> tokens = split(line);

	if (tokens.size() < 2) return; // phai co ten va duong dan

	string filepath = tokens[0].c_str();
	string sound_name = tokens[1].c_str();
	sound->LoadSound(filepath, sound_name);
}

/*
	Parse a line in section [OBJECTS]
*/
void CPlayScene::_ParseSection_OBJECTS(string line)
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
	case OBJECT_TYPE_GIMMICK:
		if (player != NULL)
		{
			DebugOut(L"[ERROR] GIMMICK object was created before!\n");
			return;
		}
		obj = new CGimmick();
		player = (CGimmick*)obj;

		DebugOut(L"[INFO] Player object created!\n");
		break;
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
	case OBJECT_TYPE_TUBE:
		obj = new CTube(atoi(tokens[4].c_str()));
		break;
	case OBJECT_TYPE_WINDOW:
		obj = new CWindow(atoi(tokens[4].c_str()));
		break;
	case OBJECT_TYPE_BOOM:
		obj = new CBoom();
		break;
	case OBJECT_TYPE_SWING:
		obj = new CSwing();
		break;
	case OBJECT_TYPE_WORM:
		obj = new CWorm(atoi(tokens[4].c_str()));
		break;
	case OBJECT_TYPE_MEDICINE:
		obj = new CMedicine(atoi(tokens[4].c_str()));
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

void CPlayScene::Load()
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
		if (line == "[ZONES]") { section = SCENE_SECTION_ZONES; continue; }
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
		case SCENE_SECTION_ZONES: _ParseSection_ZONES(line); break;
		case SCENE_SECTION_SPRITES: _ParseSection_SPRITES(line); break;
		case SCENE_SECTION_ANIMATIONS: _ParseSection_ANIMATIONS(line); break;
		case SCENE_SECTION_ANIMATION_SETS: _ParseSection_ANIMATION_SETS(line); break;
		case SCENE_SECTION_OBJECTS: _ParseSection_OBJECTS(line); break;
		}
	}

	f.close();

	CTextures::GetInstance()->Add(ID_TEX_BBOX, L"textures\\bbox.png", D3DCOLOR_XRGB(255, 255, 255));

	DebugOut(L"[INFO] Done loading scene resources %s\n", sceneFilePath);

	// Create quadtree
	quadtree = new Quadtree(1, 0.0f, 768.0f, 2048.0f, 0.0f);
	Sound::GetInstance()->Play("SOUND_Stage1_Background", 1);
}

void CPlayScene::Update(DWORD dt)
{
	// Cho tất cả các object có nhiệm vụ làm coObject vào Quadtree
	for (size_t i = 0; i < objects.size(); i++) {
		if (dynamic_cast<CGimmick*>(objects[i]))
			continue;
		if (dynamic_cast<CBlueFire*>(objects[i]))
			continue;
		if (dynamic_cast<CChain*>(objects[i]))
			continue;
		if (dynamic_cast<CCogwheel*>(objects[i]))
			continue;
		if (dynamic_cast<CCogwheelSmall*>(objects[i]))
			continue;
		if (dynamic_cast<CTube*>(objects[i]))
			continue;
		if (dynamic_cast<CWindow*>(objects[i]))
			continue;
		quadtree->Insert(objects[i]);
	}
	// Duyệt các object cần update (có code xử lý trong hàm update của object đó)
	for (size_t i = 0; i < objects.size(); i++)
	{
		if (!CGame::GetInstance()->InCamera(objects[i]))
			continue;
		if (dynamic_cast<CGimmick*>(objects[i])
			|| dynamic_cast<CBoom*>(objects[i])
			|| dynamic_cast<CSwing*>(objects[i]))
		{
			vector<LPGAMEOBJECT> coObjects;
			quadtree->Retrieve(&coObjects, objects[i]);
			objects[i]->Update(dt, &coObjects);
		}
	}
	// Làm trống quadtree
	quadtree->Clear();


	/*vector<LPGAMEOBJECT> coObjects;
	for (size_t i = 1; i < objects.size(); i++)
	{
		if (dynamic_cast<CGimmick*>(objects[i]))
			continue;
		coObjects.push_back(objects[i]);
	}

	for (size_t i = 0; i < objects.size(); i++) {
		if (dynamic_cast<CGimmick*>(objects[i]) || dynamic_cast<CBoom*>(objects[i]))
			objects[i]->Update(dt, &coObjects);
	}*/

	// skip the rest if scene was already unloaded (Mario::Update might trigger PlayScene::Unload)
	if (player == NULL) return;

	SetCamPos();
}

void CPlayScene::UpdateZone() {
	float x, y;
	player->GetPosition(x, y);
	for (unsigned int i = 0; i < zones.size(); i++)
	{
		boolean inZone = true;
		if (x < zones[i]->l) inZone = false;
		if (x > zones[i]->r) inZone = false;
		if (y > zones[i]->t) inZone = false;
		if (y < zones[i]->b) inZone = false;

		if (inZone) {
			ll = zones[i]->l;
			lt = zones[i]->t;
			lr = zones[i]->r;
			lb = zones[i]->b;
		}
	}
}

void CPlayScene::SetCamPos() {
	UpdateZone();

	// Update camera to follow mario
	float cx, cy;
	player->GetPosition(cx, cy);

	CGame* game = CGame::GetInstance();
	cx -= game->GetScreenWidth() / 2;
	cy += game->GetScreenHeight() / 2;


	if (cx < ll) cx = ll;
	if (cx + game->GetScreenWidth() > lr) cx = lr - game->GetScreenWidth();

	if (cy > lt) cy = lt;
	if (cy - game->GetScreenHeight() < lb) cy = lb + game->GetScreenHeight();

	CGame::GetInstance()->SetCamPos(cx, cy);
}

void CPlayScene::Render()
{
	// Render Waterfall
	if (countfps == fps)
	{
		if (this->maptt == this->spritemap - 1)
			this->maptt = 0;
		else
			this->maptt++;
	}
	if (this->map)
		this->map[maptt]->Render();
	if (countfps >= fps)
	{
		countfps = 0;
	}
	else
		countfps++;

	// Render objects
	for (int i = 0; i < objects.size(); i++)
		if (!dynamic_cast<CGimmick*>(objects[i]) && CGame::GetInstance()->InCamera(objects[i]))
			objects[i]->Render();
	// Render player
	if (player) player->Render();

	// Render top layer
	for (int i = 0; i < objects.size(); i++)
		if ((dynamic_cast<CTube*>(objects[i]) || dynamic_cast<CWindow*>(objects[i])) && CGame::GetInstance()->InCamera(objects[i]))
			objects[i]->Render();
}

/*
	Unload current scene
*/
void CPlayScene::Unload()
{
	for (int i = 0; i < objects.size(); i++)
		delete objects[i];

	objects.clear();
	player = NULL;
	for (unsigned int i = 0; i < zones.size(); i++)
		delete zones[i];
	zones.clear();

	if (quadtree) {
		delete quadtree;
		quadtree = nullptr;
	}

	DebugOut(L"[INFO] Scene %s unloaded! \n", sceneFilePath);
}

void CPlaySceneKeyHandler::OnKeyDown(int KeyCode)
{
	//DebugOut(L"[INFO] KeyDown: %d\n", KeyCode);
	Sound* sound = Sound::GetInstance();

	CGimmick* gimmick = ((CPlayScene*)scene)->GetPlayer();
	switch (KeyCode)
	{
	case DIK_SPACE:
		gimmick->SetState(GIMMICK_STATE_JUMP);
		sound->Play("SOUND_Effect_1", 0, 1);
		break;
	}
}

void CPlaySceneKeyHandler::KeyState(BYTE* states)
{
	CGame* game = CGame::GetInstance();
	CGimmick* gimmick = ((CPlayScene*)scene)->GetPlayer();

	// disable control key when Mario die 
	if (gimmick->GetState() == GIMMICK_STATE_DIE) return;
	if (game->IsKeyDown(DIK_RIGHT))
		gimmick->SetState(GIMMICK_STATE_WALKING_RIGHT);
	else if (game->IsKeyDown(DIK_LEFT))
		gimmick->SetState(GIMMICK_STATE_WALKING_LEFT);
	else
		gimmick->SetState(GIMMICK_STATE_IDLE);
}
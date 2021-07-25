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
	this->star = nullptr;
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
#define OBJECT_TYPE_BLACKENEMY		13
#define OBJECT_TYPE_SWING			15
#define OBJECT_TYPE_PINK_BRICK		16
#define OBJECT_TYPE_MEDICINE		17
#define OBJECT_TYPE_STAR			18
#define OBJECT_TYPE_DOOR			19
#define OBJECT_TYPE_BLACK_BOSS		22
#define OBJECT_TYPE_SEWER			99
#define OBJECT_TYPE_ENEMYBOOM		40
#define OBJECT_TYPE_PORTAL			42
#define OBJECT_TYPE_BRIDGE			43
#define OBJECT_TYPE_ENEMYTAIL		44
#define OBJECT_TYPE_SPECIALBRICK	45
#define OBJECT_TYPE_ELECTRIC_BLACKENEMY		23
#define OBJECT_TYPE_THUNDER			24

#define OBJECT_TYPE_GREEN_BOSS		50
#define OBJECT_TYPE_SWORD_BOSS		51
#define OBJECT_TYPE_SWORD			52
#define OBJECT_TYPE_FINALBOSS		53
#define OBJECT_TYPE_FINALBOSS_BIG_BULLET		54
#define OBJECT_TYPE_FINALBOSS_SMALL_BULLET		55
#define OBJECT_TYPE_FINALBOSS_DIE_EFFECT		56
#define OBJECT_TYPE_GUN				20
#define OBJECT_TYPE_BIRD			177
#define OBJECT_TYPE_CAT				666
#define OBJECT_TYPE_BOAT			700
#define OBJECT_TYPE_WATER_DIE		750
#define OBJECT_TYPE_BOOM_BOAT		777
#define OBJECT_TYPE_BIG_BOAT_WINDOW	778
#define OBJECT_TYPE_CANNON			80
#define OBJECT_TYPE_TURLTE			91
#define OBJECT_TYPE_BLACKBIRD		35
#define OBJECT_TYPE_STANDBLACKENEMY		34
#define OBJECT_TYPE_CLOUDENEMY		37

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

	/*if (maptt == -1)
		this->map = new Map * [spritemap];
	maptt++;
	this->map[maptt] = new Map(TotalRowsOfMap, TotalColumnsOfMap, TotalRowsOfTileSet, TotalColumnsOfTileSet, TileSetID, mapMatrixPath);*/

	Map* map = new Map(TotalRowsOfMap, TotalColumnsOfMap, TotalRowsOfTileSet, TotalColumnsOfTileSet, TileSetID, mapMatrixPath);
	maps.push_back(map);

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
	float revival_x = (float)atof(tokens[4].c_str());
	float revival_y = (float)atof(tokens[5].c_str());

	CZone* zone = new CZone(l, t, r, b, revival_x, revival_y);
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
	case OBJECT_TYPE_CAT:
		obj = new CCat(atoi(tokens[4].c_str()), atof(tokens[5].c_str()), atoi(tokens[6].c_str()));
		break;
	case OBJECT_TYPE_BIRD:
		obj = new CBird(atoi(tokens[4].c_str()));
		break;
	case OBJECT_TYPE_BRICK: obj = new CBrick(atof(tokens[4].c_str()), atof(tokens[5].c_str())); break;
	case OBJECT_TYPE_WATER_DIE: obj = new CWaterDie(atof(tokens[4].c_str()), atof(tokens[5].c_str())); break;
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
	case OBJECT_TYPE_SEWER: 
		obj = new CSewer(atof(tokens[4].c_str()));
		break;
	case OBJECT_TYPE_BOAT:
		obj = new CBoat();
		break;
	case OBJECT_TYPE_BIG_BOAT_WINDOW:
		obj = new CBigBoatWindow();
		break;
	case OBJECT_TYPE_BOOM_BOAT:
		obj = new CBomboat();
		break;
	case OBJECT_TYPE_COGWHEEL:
		obj = new CCogwheel(atoi(tokens[4].c_str()));
		break;
	case OBJECT_TYPE_CHAIN:
		obj = new CChain(atoi(tokens[4].c_str()));
		break;
	case OBJECT_TYPE_BLUEFIRE: obj = new CBlueFire(); break;
	case OBJECT_TYPE_PINK_BRICK:
		obj = new CBrickPink(atof(tokens[4].c_str()), atof(tokens[5].c_str()), atoi(tokens[6].c_str()), atof(tokens[7].c_str()), atof(tokens[8].c_str()));
		break;
	case OBJECT_TYPE_INCLINEDBRICK:
		obj = new CInclinedBrick(atof(tokens[4].c_str()), atof(tokens[5].c_str()), atoi(tokens[6].c_str()));
		break;
	case OBJECT_TYPE_CONVEYOR:
		obj = new CConveyor(atoi(tokens[4].c_str()), atoi(tokens[5].c_str()));
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
	case OBJECT_TYPE_BLACKENEMY:
		if(tokens.size() > 5)
			obj = new CBlackEnemy(atoi(tokens[4].c_str()), atoi(tokens[5].c_str()) );
		else
			obj = new CBlackEnemy(atoi(tokens[4].c_str()));
		break;
	case OBJECT_TYPE_STAR:
		if (star != NULL)
		{
			DebugOut(L"[ERROR] STAR object was created before!\n");
			return;
		}
		obj = new CStar();
		this->star = (CStar*)obj;

		DebugOut(L"[INFO] Srar object created!\n");
		break;
	case OBJECT_TYPE_DOOR:
		obj = new CDoor();
		break;
	case OBJECT_TYPE_BLACK_BOSS:
		obj = new CBlackBoss();
		break;
	case OBJECT_TYPE_PORTAL:
		obj = new CPortal(atof(tokens[4].c_str()), atof(tokens[5].c_str()));
		break;
	case OBJECT_TYPE_ENEMYBOOM:
		obj = new CEnemyBoom(atof(tokens[4].c_str()), atof(tokens[5].c_str()));
		break;
	case OBJECT_TYPE_GREEN_BOSS:
		obj = new CGreenBoss();
		break;
	case OBJECT_TYPE_GUN:
		obj = new CGun();
		break;
	case OBJECT_TYPE_ELECTRIC_BLACKENEMY:
		obj = new CElectricBlackEnemy();
		break;
	case OBJECT_TYPE_CANNON:
		obj = new CCannon();
		break;
	case OBJECT_TYPE_TURLTE:
		obj = new CTurle(atof(tokens[4].c_str()), atof(tokens[5].c_str()));
		break;
	case OBJECT_TYPE_BLACKBIRD:
		obj = new CBlackBird(atoi(tokens[4].c_str()), atof(tokens[5].c_str()));
		break;
	case OBJECT_TYPE_STANDBLACKENEMY:
		obj = new CStandBlackEnemy();
		break;
	case OBJECT_TYPE_SWORD_BOSS:
		obj = new CSwordBoss();
		break; 
	case OBJECT_TYPE_FINALBOSS:
		obj = new CFinalBoss();
		break;
	case OBJECT_TYPE_BRIDGE:
		obj = new CBridge(atoi(tokens[4].c_str()));
		break;
	case OBJECT_TYPE_ENEMYTAIL:
		obj = new CEnemyTail(atof(tokens[4].c_str()), atof(tokens[5].c_str()));
		break;
	case OBJECT_TYPE_SPECIALBRICK: obj = new CSpecialBrick(atof(tokens[4].c_str()), atof(tokens[5].c_str()), atoi(tokens[6].c_str())); break;
	case OBJECT_TYPE_CLOUDENEMY:
		obj = new CCloudEnemy(atof(tokens[4].c_str()), atof(tokens[5].c_str()));
		break;
	default:
		DebugOut(L"[ERR] Invalid object type: %d\n", object_type);
		return;
	}

	// General object setup
	obj->SetPosition(x, y);
	obj->SetStartPosition(x, y);

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

	// create hud
	hud = new CHud();
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
		if (dynamic_cast<CGimmickDieEffect*>(objects[i]))
			continue;
		if (dynamic_cast<CBomboat*>(objects[i]))
			continue;
		if (dynamic_cast<CBridge*>(objects[i]))
			continue;
		if (dynamic_cast<CPortal*>(objects[i]))
			continue;
		if (dynamic_cast<CBird*>(objects[i]))
			continue;
		if (dynamic_cast<CMedicine*>(objects[i]))
			continue;
		if (dynamic_cast<CCat*>(objects[i]))
			continue;
		if (dynamic_cast<CFinalBossDieEffect*>(objects[i]))
			continue;
		quadtree->Insert(objects[i]);
	}

	// Update player
	vector<LPGAMEOBJECT> coObjects;
	quadtree->Retrieve(&coObjects, player);
	player->Update(dt, &coObjects);

	// Set position star follow player
	if (star->GetState() == STAR_STATE_HIDE || star->GetState() == STAR_STATE_PENDING || star->GetState() == STAR_STATE_CREATED || star->GetState() == STAR_STATE_READY) {
		star->SetPosition(player->x, player->y + 16);
	}

	// Update star
	vector<LPGAMEOBJECT> tmp_coObjects;
	quadtree->Retrieve(&tmp_coObjects, star);
	star->Update(dt, &tmp_coObjects);

	// Duyệt các object cần update (có code xử lý trong hàm update của object đó)
	for (size_t i = 0; i < objects.size(); i++)
	{
		if (dynamic_cast<CSwing*>(objects[i])
			|| dynamic_cast<CBoat*>(objects[i])
			|| dynamic_cast<CBlackBird*>(objects[i])
			|| dynamic_cast<CSwordBoss*>(objects[i]))
		{
			vector<LPGAMEOBJECT> coObjects;
			quadtree->Retrieve(&coObjects, objects[i]);
			objects[i]->Update(dt, &coObjects);
		}
		else {
			if (!CGame::GetInstance()->InLargeCamera(objects[i]))
				continue;
			if (dynamic_cast<CBoom*>(objects[i])
				|| dynamic_cast<CBlueFire*>(objects[i])
				|| dynamic_cast<CGimmickDieEffect*>(objects[i])
				|| dynamic_cast<CWorm*>(objects[i])
				|| dynamic_cast<CBlackEnemy*>(objects[i])
				|| dynamic_cast<CBlackBoss*>(objects[i])
				|| dynamic_cast<CBrick*>(objects[i])
				|| dynamic_cast<CBrickPink*>(objects[i])
				|| dynamic_cast<CDoor*>(objects[i])
				|| dynamic_cast<CElectricBlackEnemy*>(objects[i])
				|| dynamic_cast<CGreenBoss*>(objects[i])
				|| dynamic_cast<CPortal*>(objects[i])
				|| dynamic_cast<CGun*>(objects[i])
				|| dynamic_cast<CBullet*>(objects[i])
				|| dynamic_cast<CWaterDie*>(objects[i])
				|| dynamic_cast<CEnemyBoom*>(objects[i])
				|| dynamic_cast<CMiniBoom*>(objects[i])
				|| dynamic_cast<CBomboat*>(objects[i])
				|| dynamic_cast<CCannon*>(objects[i])
				|| dynamic_cast<CBoomCannon*>(objects[i])
				|| dynamic_cast<CTurle*>(objects[i])
				|| dynamic_cast<CSwordBoss*>(objects[i])
				|| dynamic_cast<CSword*>(objects[i])
				|| dynamic_cast<CFinalBoss*>(objects[i])
				|| dynamic_cast<CFinalBossBigBullet*>(objects[i])
				|| dynamic_cast<CFinalBossSmallBullet*>(objects[i])
				|| dynamic_cast<CFinalBossDieEffect*>(objects[i])
				|| dynamic_cast<CBird*>(objects[i])
				|| dynamic_cast<CStandBlackEnemy*>(objects[i])
				|| dynamic_cast<CSword*>(objects[i])
				|| dynamic_cast<CEnemyTail*>(objects[i])
				|| dynamic_cast<CCloudEnemy*>(objects[i])
				|| dynamic_cast<CMedicine*>(objects[i])
				|| dynamic_cast<CCat*>(objects[i]))
			{
				if (dynamic_cast<CCloudEnemy*>(objects[i]) && attackBird > 0)
				{
					((CCloudEnemy*)(objects[i]))->SetState(CLOUD_STATE_ATTACK);
				}
				vector<LPGAMEOBJECT> coObjects;
				quadtree->Retrieve(&coObjects, objects[i]);
				objects[i]->Update(dt, &coObjects);
			}
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


	for (int i = objects.size() - 1; i >= 0; i--) {
		if (dynamic_cast<CBullet*>(objects[i]))
		{
			CBullet* bullet = (CBullet*)(objects[i]);
			if (bullet->isDelete == true)
			{
				objects.erase(objects.begin() + i);
				delete bullet;
			}
		}
		else if (dynamic_cast<CFinalBossSmallBullet*>(objects[i]))
		{
			CFinalBossSmallBullet* bullet = (CFinalBossSmallBullet*)(objects[i]);
			if (!bullet->visible)
			{
				objects.erase(objects.begin() + i);
				delete bullet;
			}
		}
		else if (dynamic_cast<CFinalBossBigBullet*>(objects[i]))
		{
			CFinalBossBigBullet* bullet = (CFinalBossBigBullet*)(objects[i]);
			if (!bullet->visible)
			{
				objects.erase(objects.begin() + i);
				delete bullet;
			}
		}
		else if (dynamic_cast<CBoomCannon*>(objects[i]))
		{
			CBoomCannon* boom_cannon = (CBoomCannon*)(objects[i]);
			if (boom_cannon->isDelete == true)
			{
				objects.erase(objects.begin() + i);
				delete boom_cannon;
			}
		}
		else if (dynamic_cast<CGun*>(objects[i]))
		{
			CGun* gun = (CGun*)(objects[i]);
			if (gun->visible == false)
			{
				objects.erase(objects.begin() + i);
				delete gun;
			}
		}
	}
		
	// skip the rest if scene was already unloaded (Mario::Update might trigger PlayScene::Unload)
	if (player == NULL) return;

	attackBird--;

	SetCamPos();

	hud->Update(dt);
	// Play soundtrack sence
	CScene* Scene = CGame::GetInstance()->GetCurrentScene();
	if (player->state == GIMMICK_STATE_DIE)
	{
		Sound::GetInstance()->Stop("SOUND_Boss_1");
		Sound::GetInstance()->Stop("SOUND_Scene_1");
		Sound::GetInstance()->Stop("SOUND_Sence_2");
		Sound::GetInstance()->Stop("SOUND_Scene_High_1");
		Sound::GetInstance()->Stop("SOUND_Sence_7");
	}
	else if (Scene->Getid() == 1)
	{
		if (player->x >= 0 && player->x <= 1024 && player->y <= 850 && player->y >= 576)
		{
			//Sound::GetInstance()->StopAll();
			Sound::GetInstance()->Stop("SOUND_Boss_1");
			Sound::GetInstance()->Stop("SOUND_Scene_1");
			Sound::GetInstance()->Play("SOUND_Scene_High_1", 1);
		}
		else if(player->x >= 1024 && player->x <= 1280 && player->y <= 800 && player->y >= 576)
		{
			//Sound::GetInstance()->StopAll();
			Sound::GetInstance()->Stop("SOUND_Scene_High_1");
			Sound::GetInstance()->Stop("SOUND_Scene_1");
			Sound::GetInstance()->Play("SOUND_Boss_1", 1);
		}
		else
		{
			//Sound::GetInstance()->StopAll();
			Sound::GetInstance()->Stop("SOUND_Scene_High_1");
			Sound::GetInstance()->Stop("SOUND_Boss_1");
			Sound::GetInstance()->Play("SOUND_Scene_1", 1);
		}
	}
	else if (Scene->Getid() == 2)
	{
		if (player->x >= 0 && player->x <= 480 && player->y <= 980 && player->y >= 432)
		{
			//Sound::GetInstance()->StopAll();
			Sound::GetInstance()->Stop("SOUND_Boss_1");
			Sound::GetInstance()->Stop("SOUND_Sence_2");
			Sound::GetInstance()->Play("SOUND_Scene_High_1", 1);
		}
		else if (player->x >= 1792 && player->x <= 2048 && player->y <= 590 && player->y >= 384)
		{
			//Sound::GetInstance()->StopAll();
			Sound::GetInstance()->Stop("SOUND_Scene_High_1");
			Sound::GetInstance()->Stop("SOUND_Sence_2");
			Sound::GetInstance()->Play("SOUND_Boss_1", 1);
		}
		else
		{
			//Sound::GetInstance()->StopAll();
			Sound::GetInstance()->Stop("SOUND_Scene_High_1");
			Sound::GetInstance()->Stop("SOUND_Boss_1");
			Sound::GetInstance()->Play("SOUND_Sence_2", 1);
		}
	}
	else if (Scene->Getid() == 7)
	{
		if (player->x >= 1792 && player->x <= 2048 && player->y <= 768 && player->y >= 576)
		{
			//Sound::GetInstance()->StopAll();
			Sound::GetInstance()->Stop("SOUND_Sence_7");
			Sound::GetInstance()->Play("SOUND_Boss_1", 1);
		}
		else
		{
			//Sound::GetInstance()->StopAll();
			Sound::GetInstance()->Stop("SOUND_Boss_1");
			Sound::GetInstance()->Play("SOUND_Sence_7", 1);
		}
	}
	// CheckEndScene
	CheckSwitchScene();
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
			revival_x = zones[i]->revival_x;
			revival_y = zones[i]->revival_y;
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

	hud->SetPosition(cx, cy - game->GetScreenHeight());
}

void CPlayScene::CheckSwitchScene() {
	if (end_scene == 1 && GetTickCount64() - end_scene_start >= ENDSCENE_TIME) {
		end_scene = 0;
		end_scene_start = NULL;
		CGame::GetInstance()->SwitchScene(1000);
	} else if (end_game == 1 && GetTickCount64() - end_game_start >= ENDSCENE_TIME) {
		end_game = 0;
		end_game_start = NULL;
		CGame::GetInstance()->SwitchScene(888);
	}
}

void CPlayScene::PushBackObj(CGameObject* obj) {
	objects.push_back(obj);
}

void CPlayScene::Render()
{
	// Render Waterfall
	CScene* Scene2 = CGame::GetInstance()->GetCurrentScene();
	if (Scene2->Getid() == 2)
		fps = 3;
	if (countfps == fps)
	{
		if (this->maptt == maps.size() - 1)
			this->maptt = 0;
		else
			this->maptt++;
	}
	maps[maptt]->Render();
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
	{
		if (dynamic_cast<CSewer*>(objects[i]) || (dynamic_cast<CTube*>(objects[i]) || dynamic_cast<CWindow*>(objects[i]) || dynamic_cast<CBridge*>(objects[i]) || dynamic_cast<CBird*>(objects[i])) && CGame::GetInstance()->InCamera(objects[i]))
			objects[i]->Render();
		if (dynamic_cast<CBomboat*>(objects[i])) // render Boomboat falling
		{
			CBomboat* boomboat = dynamic_cast<CBomboat*>(objects[i]);
			if(boomboat->vy<0)
				objects[i]->Render();
		}
	}

	hud->Render();
}

/*
	Unload current scene
*/
void CPlayScene::Unload()
{
	for (int i = 0; i < objects.size(); i++)
		delete objects[i];

	objects.clear();

	player = nullptr;

	star = nullptr;

	hud = nullptr;

	/*if (hud)
	{
		delete hud;
		hud = nullptr;
	}*/

	for (unsigned int i = 0; i < maps.size(); i++)
		delete maps[i];
	maps.clear();

	for (unsigned int i = 0; i < zones.size(); i++)
		delete zones[i];
	zones.clear();

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
	CStar* star = ((CPlayScene*)scene)->GetStar();

	/*if (gimmick->GetState() == GIMMICK_STATE_DIE || gimmick->stunning == true)
		return;*/

	switch (KeyCode)
	{
	//case DIK_SPACE:
	//	sound->Play("SOUND_Effect_24", 0, 1); // jump
	//	break;
	case DIK_S:
		if (gimmick->GetState() != GIMMICK_STATE_DIE && gimmick->stunning == false && gimmick->inSewer == false) {
			if (star != nullptr) {
				star->Ready();
			}
		}
		break;
	case DIK_DOWN:
		CBackup::GetInstance()->ChangeItem();
		break;
	// =============== SCENE 1:
	case DIK_1: // Cho can sao de nhay len
		gimmick->SetState(GIMMICK_STATE_IDLE);
		gimmick->x = 990;
		gimmick->y = 560;
		break;
	case DIK_V: // boss man 1
		gimmick->SetState(GIMMICK_STATE_IDLE);
		gimmick->x = 1300;
		gimmick->y = 630;
		break;
	// =============== SCENE 2:
	case DIK_2: // di thuyen qua bo
		gimmick->SetState(GIMMICK_STATE_IDLE);
		gimmick->x = 1760;
		gimmick->y = 272;
		break;
	case DIK_3: //truoc khau sung
		gimmick->SetState(GIMMICK_STATE_IDLE);
		gimmick->x = 752;
		gimmick->y = 528;
		break;
	case DIK_4: // truoc blackbird
		gimmick->SetState(GIMMICK_STATE_IDLE);
		gimmick->SetPosition(176, 688);
		break;
	case DIK_5: // truoc blackbird 2
		gimmick->SetState(GIMMICK_STATE_IDLE);
		gimmick->x = 1504;
		gimmick->y = 672;
		break;
	case DIK_B: //green boss
		gimmick->SetState(GIMMICK_STATE_IDLE);
		gimmick->x = 1150;
		gimmick->y = 448;
		break;
	case DIK_N: //Sword boss
		gimmick->SetState(GIMMICK_STATE_IDLE);
		gimmick->x = 1744;
		gimmick->y = 480;
		break;
	// ============== SCENE 7:
	case DIK_6: // truoc man 7 (1) tren ong nuoc
		gimmick->SetState(GIMMICK_STATE_IDLE);
		gimmick->x = 1936;
		gimmick->y = 304;
		break;
	case DIK_7: // truoc man 7 (2) bai co
		gimmick->SetState(GIMMICK_STATE_IDLE);
		gimmick->x = 944;
		gimmick->y = 448;
		break;
	case DIK_M: // Final boss
		gimmick->SetState(GIMMICK_STATE_IDLE);
		gimmick->x = 1712;
		gimmick->y = 640;
		break;
	case DIK_U:
		CGame::GetInstance()->SwitchScene(1000);
		break;
	}
}

void CPlaySceneKeyHandler::KeyState(BYTE* states)
{
	CGame* game = CGame::GetInstance();
	CGimmick* gimmick = ((CPlayScene*)scene)->GetPlayer();

	if (gimmick->GetState() == GIMMICK_STATE_DIE)
		return;

	// disable control key when Mario die 
	if (gimmick->GetState() == GIMMICK_STATE_DIE) return;
	if (game->IsKeyDown(DIK_SPACE) && gimmick->stunning == false && !gimmick->inSewer) {
		if (!gimmick->falling || gimmick->onInclinedBrick || gimmick->onEnemy || gimmick->onStar || gimmick->jumping) // onEnemy de fix loi ko nhay dc tren quai
			gimmick->SetState(GIMMICK_STATE_JUMP);
	}
	if (game->IsKeyDown(DIK_RIGHT) && gimmick->stunning == false && !gimmick->inSewer)
		gimmick->SetState(GIMMICK_STATE_WALKING_RIGHT);
	else if (game->IsKeyDown(DIK_LEFT) && gimmick->stunning == false && !gimmick->inSewer)
		gimmick->SetState(GIMMICK_STATE_WALKING_LEFT);
	else
		gimmick->SetState(GIMMICK_STATE_IDLE);
}

void CPlaySceneKeyHandler::OnKeyUp(int KeyCode)
{
	CGimmick* gimmick = ((CPlayScene*)scene)->GetPlayer();
	/*if (gimmick->GetState() == GIMMICK_STATE_DIE || gimmick->stunning == true || gimmick->inSewer == true)
		return;*/
	CStar* star = ((CPlayScene*)scene)->GetStar();

	switch (KeyCode)
	{
	case DIK_S:
		if (gimmick->GetState() != GIMMICK_STATE_DIE && gimmick->stunning == false && gimmick->inSewer == false) {
			if (star != nullptr) {
				star->Shot();
			}
		}
		break;
	case DIK_SPACE:
		if (gimmick->GetState() != GIMMICK_STATE_DIE && gimmick->stunning == false && gimmick->inSewer == false) {
			gimmick->falling = true;
			gimmick->jumping = false;
		}
		break;
	}
}
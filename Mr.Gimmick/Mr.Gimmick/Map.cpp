#include "Map.h"
#include <fstream>
#include "Textures.h"
#include "Game.h"

Map::Map(float posX, float posY, int TotalRowsOfMap, int TotalColumnsOfMap, int TotalRowsOfTileSet, int TotalColumnsOfTileSet, int TileSetID, wstring mapMatrixPath) {
	this->posx = posX;
	this->posy = posY;
	this->TotalRowsOfTileSet = TotalRowsOfTileSet;
	this->TotalColumnsOfTileSet = TotalColumnsOfTileSet;
	this->TotalRowsOfMap = TotalRowsOfMap;
	this->TotalColumnsOfMap = TotalColumnsOfMap;
	TileSetImg = CTextures::GetInstance()->Get(TileSetID);
	LoadMapMatrix(mapMatrixPath);
}

void Map::LoadMapMatrix(wstring path) {
	fstream inputfile;
	inputfile.open(path, ios::in);
	int tmp;

	for (int i = 0; i < TotalRowsOfMap * TotalColumnsOfMap; i++) {
		inputfile >> tmp;
		MapData.push_back(tmp);
	}

	inputfile.close();
}

void Map::DrawTile(int idOfTile, float x, float y) {
	float xOfTile, yOfTile;
	int colOfTile, rowOfTile;

	rowOfTile = (idOfTile - 1) / TotalColumnsOfTileSet;
	colOfTile = (idOfTile - 1) % TotalColumnsOfTileSet;

	xOfTile = colOfTile * TILE_WIDTH;
	yOfTile = rowOfTile * TILE_HEIGHT;

	CGame::GetInstance()->Draw(posx + x, posy + y, TileSetImg, xOfTile, yOfTile, xOfTile + TILE_WIDTH, yOfTile + TILE_HEIGHT);
}

float Map::getMapWidth() {
	return TotalColumnsOfMap * TILE_WIDTH;
}

float Map::getMapHeight() {
	return TotalRowsOfMap * TILE_HEIGHT;
}

void Map::Render() {
	float xOfTile, yOfTile;
	int colOfTile, rowOfTile;

	int rowBegin, rowEnd, colBegin, colEnd;
	CGame* game = CGame::GetInstance();
	float cx;
	float cy;
	float screenWidth, screenHeight;
	game->GetCamPos(cx, cy);
	screenWidth = game->GetScreenWidth();
	screenHeight = game->GetScreenHeight();

	bool isDraw;

	for (int i = 0; i < TotalRowsOfMap * TotalColumnsOfMap; i++) {
		isDraw = true;

		rowOfTile = i / TotalColumnsOfMap;
		colOfTile = i % TotalColumnsOfMap;

		colBegin = floor(cx / TILE_WIDTH);
		colEnd = ceil((cx + screenWidth) / TILE_WIDTH);

		rowBegin = floor(cy / TILE_HEIGHT);
		rowEnd = ceil((cy + screenHeight) / TILE_HEIGHT);

		xOfTile = colOfTile * TILE_WIDTH;
		yOfTile = rowOfTile * TILE_HEIGHT;

		if (colOfTile < colBegin || colOfTile > colEnd) isDraw = false;
		if (rowOfTile < rowBegin || rowOfTile > rowEnd) isDraw = false;

		if (isDraw)
			DrawTile(MapData[i], xOfTile, yOfTile);
	}
}

Map::~Map()
{

}
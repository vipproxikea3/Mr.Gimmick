#pragma once
#include <d3dx9.h>
#include <vector>
#include <string>

#define TILE_WIDTH 16
#define TILE_HEIGHT 16

using namespace std;

class Map
{
	float posx, posy;
	int TotalRowsOfTileSet, TotalColumnsOfTileSet;
	LPDIRECT3DTEXTURE9 TileSetImg;
	int TotalRowsOfMap, TotalColumnsOfMap;
	vector<int> MapData;

public:
	Map(float posx, float posy, int TotalRowsOfMap, int TotalColumnsOfMap, int TotalRowsOfTileSet, int TotalColumnsOfTileSet, int TileSetID, wstring mapMatrixPath);
	~Map();
	void LoadMapMatrix(wstring mapMatrixPath);
	void DrawTile(int idOfTile, float x, float y);
	float getMapWidth();
	float getMapHeight();
	void Render();
};
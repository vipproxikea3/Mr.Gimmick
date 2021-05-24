#pragma once
#include <d3dx9.h>
#include <vector>
#include <string>

#define TILE_WIDTH 16.0f
#define TILE_HEIGHT 16.0f

using namespace std;

class Map
{
	int TotalRowsOfTileSet, TotalColumnsOfTileSet;
	LPDIRECT3DTEXTURE9 TileSetImg;
	int TotalRowsOfMap, TotalColumnsOfMap;
	vector<int> MapData;

public:
	Map(int TotalRowsOfMap, int TotalColumnsOfMap, int TotalRowsOfTileSet, int TotalColumnsOfTileSet, int TileSetID, wstring mapMatrixPath);
	~Map();
	void LoadMapMatrix(wstring mapMatrixPath);
	void DrawTile(int idOfTile, float x, float y);
	float getMapWidth();
	float getMapHeight();
	void Render();
};
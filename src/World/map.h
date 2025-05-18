#ifndef MAP_H
#define MAP_H

#include "raylib.h"

#define MAP_WIDTH 24
#define MAP_HEIGHT 24
#define TILE_SIZE 64.0f

// Map tile types
#define TILE_EMPTY 0
#define TILE_WALL 1
#define TILE_DOOR 2
#define TILE_SECRET_WALL 3
#define TILE_OBSTACLE 4

// Direction constants
#define NORTH 0
#define EAST 1
#define SOUTH 2
#define WEST 3

typedef struct {
    int x, y;           // Position in grid
    int type;           // Type of tile
    int textureID;      // Texture ID for walls
    bool isAnimated;    // Whether this tile should be animated (e.g., doors)
    float animState;    // Animation state (0.0 to 1.0)
} MapTile;

typedef struct Map {
    int grid[MAP_WIDTH][MAP_HEIGHT];
    Texture2D wallTextures[8]; // Different wall textures
    RenderTexture2D mapTexture; // GPU texture representation of the map
    bool isMapTextureInitialized;
} Map;

void InitMap(Map* map);
void UnloadMap(Map* map);
void UpdateMap(Map* map, float deltaTime);
int GetMapTile(Map map, int x, int y);
bool IsWall(Map map, float x, float y);
bool IsDoor(Map map, int x, int y);
void SetMapTile(Map* map, int x, int y, int value);
void UpdateMapGPUTexture(Map* map);

#endif // MAP_H
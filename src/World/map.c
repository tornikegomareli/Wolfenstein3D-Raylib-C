#include "map.h"
#include <stdio.h>

// A more detailed test map with different wall types
const int TEST_MAP[MAP_HEIGHT][MAP_WIDTH] = {
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,1},
    {1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,1},
    {1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,1},
    {1,0,0,0,1,0,0,1,1,1,1,1,1,1,1,0,0,0,1,1,0,0,0,1},
    {1,0,0,0,1,0,0,1,0,0,0,0,0,0,1,0,0,0,0,1,0,0,0,1},
    {1,0,0,0,1,0,0,1,0,0,0,0,0,0,1,0,0,0,0,1,0,0,0,1},
    {1,0,0,0,1,0,0,1,0,0,0,0,0,0,1,0,0,0,0,1,0,0,0,1},
    {1,0,0,0,1,0,0,1,0,0,0,0,0,0,1,1,2,1,1,1,0,0,0,1},
    {1,0,0,0,1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,1,0,0,1,0,0,0,0,0,0,1,1,1,1,1,1,0,0,0,1},
    {1,0,0,0,1,0,0,1,0,0,0,0,0,0,1,0,0,0,0,1,0,0,0,1},
    {1,0,0,0,1,0,0,1,0,0,0,0,0,0,1,0,0,0,0,1,0,0,0,1},
    {1,0,0,0,1,0,0,1,1,1,1,1,1,1,1,0,0,0,0,1,0,0,0,1},
    {1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,1},
    {1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,1},
    {1,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
};

void InitMap(Map* map) {
    // Initialize map texture flag
    map->isMapTextureInitialized = false;
    
    // Copy test map to map grid
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            map->grid[x][y] = TEST_MAP[y][x];
        }
    }
    
    // Create different wall textures with different colors
    for (int i = 0; i < 8; i++) {
        Color color1, color2;
        
        switch (i) {
            case 0: color1 = RED; color2 = MAROON; break;
            case 1: color1 = GREEN; color2 = DARKGREEN; break;
            case 2: color1 = BLUE; color2 = DARKBLUE; break;
            case 3: color1 = YELLOW; color2 = GOLD; break;
            case 4: color1 = PURPLE; color2 = DARKPURPLE; break;
            case 5: color1 = ORANGE; color2 = BROWN; break;
            case 6: color1 = LIME; color2 = GREEN; break;
            case 7: color1 = SKYBLUE; color2 = DARKBLUE; break;
        }
        
        Image img;
        
        // Create different patterns for different walls - use variations of checkerboard
        if (i % 3 == 0) {
            img = GenImageChecked(64, 64, 8, 8, color1, color2);
        } else if (i % 3 == 1) {
            img = GenImageChecked(64, 64, 16, 16, color1, color2);
        } else {
            img = GenImageChecked(64, 64, 32, 32, color1, color2);
        }
        
        map->wallTextures[i] = LoadTextureFromImage(img);
        UnloadImage(img);
    }
    
    // Initialize GPU map texture
    UpdateMapGPUTexture(map);
}

void UnloadMap(Map* map) {
    // Unload all wall textures
    for (int i = 0; i < 8; i++) {
        UnloadTexture(map->wallTextures[i]);
    }
    
    // Unload map texture if initialized
    if (map->isMapTextureInitialized) {
        UnloadRenderTexture(map->mapTexture);
    }
}

void UpdateMap(Map* map, float deltaTime) {
    // This would handle door animations, etc.
    // For now, we don't have any animations
    
    // Avoid unused parameter warnings
    (void)map;
    (void)deltaTime;
    
    // TODO: Animate doors and update map texture if needed
}

int GetMapTile(Map map, int x, int y) {
    // Boundary check
    if (x < 0 || x >= MAP_WIDTH || y < 0 || y >= MAP_HEIGHT) {
        return TILE_WALL; // Treat out of bounds as walls
    }
    
    return map.grid[x][y];
}

bool IsWall(Map map, float x, float y) {
    // Convert world coordinates to map coordinates
    int mapX = (int)(x / TILE_SIZE);
    int mapY = (int)(y / TILE_SIZE);
    
    // Check if position is inside a wall
    int tileType = GetMapTile(map, mapX, mapY);
    return tileType == TILE_WALL || tileType == TILE_SECRET_WALL || tileType == TILE_OBSTACLE;
}

bool IsDoor(Map map, int x, int y) {
    return GetMapTile(map, x, y) == TILE_DOOR;
}

void SetMapTile(Map* map, int x, int y, int value) {
    // Boundary check
    if (x < 0 || x >= MAP_WIDTH || y < 0 || y >= MAP_HEIGHT) {
        return;
    }
    
    map->grid[x][y] = value;
    
    // Update the GPU texture when map changes
    UpdateMapGPUTexture(map);
}

void UpdateMapGPUTexture(Map* map) {
    // Create or update the GPU texture for the map
    if (!map->isMapTextureInitialized) {
        map->mapTexture = LoadRenderTexture(MAP_WIDTH, MAP_HEIGHT);
        map->isMapTextureInitialized = true;
    }
    
    // Begin drawing to the texture
    BeginTextureMode(map->mapTexture);
    
    // Clear with black
    ClearBackground(BLACK);
    
    // Draw each tile as a colored pixel
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            Color color;
            
            switch (map->grid[x][y]) {
                case TILE_EMPTY:
                    color = BLACK;
                    break;
                case TILE_WALL:
                    color = WHITE;
                    break;
                case TILE_DOOR:
                    color = RED;
                    break;
                case TILE_SECRET_WALL:
                    color = GREEN;
                    break;
                case TILE_OBSTACLE:
                    color = BLUE;
                    break;
                default:
                    color = PURPLE;
                    break;
            }
            
            DrawPixel(x, y, color);
        }
    }
    
    EndTextureMode();
}
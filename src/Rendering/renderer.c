#include "renderer.h"
#include "raylib.h"
#include "raymath.h"
#include "../World/map.h"
#include "../World/player.h"
#include <stdio.h>

// Internal variables
static Shader wallShader = { 0 };
static bool shadersLoaded = false;

void InitRenderer(void) {
    // This will be implemented with shader-based rendering later
    // For now, we just use the simple raycasting approach
}

void RenderWorld(Player player, Map map) {
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();
    
    // Draw ceiling (top half of screen)
    DrawRectangle(0, 0, screenWidth, screenHeight/2, SKYBLUE);
    
    // Draw floor (bottom half of screen)
    DrawRectangle(0, screenHeight/2, screenWidth, screenHeight/2, DARKGRAY);
    
    // Simple raycasting loop
    for (int x = 0; x < screenWidth; x++) {
        // Calculate ray position and direction
        float cameraX = 2.0f * x / (float)screenWidth - 1.0f; // x-coordinate in camera space
        Vector2 rayDir = {
            player.direction.x + player.plane.x * cameraX,
            player.direction.y + player.plane.y * cameraX
        };
        
        // Map position
        int mapX = (int)(player.position.x / TILE_SIZE);
        int mapY = (int)(player.position.y / TILE_SIZE);
        
        // Length of ray from current position to next x or y-side
        Vector2 deltaDist = {
            fabsf(rayDir.x) < 0.0001f ? 1e30f : fabsf(1.0f / rayDir.x),
            fabsf(rayDir.y) < 0.0001f ? 1e30f : fabsf(1.0f / rayDir.y)
        };
        
        // Direction to step in x or y direction (either +1 or -1)
        int stepX, stepY;
        
        // Length of ray from one side to next in map
        float sideDistX, sideDistY;
        
        // Calculate step and initial sideDist
        if (rayDir.x < 0) {
            stepX = -1;
            sideDistX = ((player.position.x / TILE_SIZE) - mapX) * deltaDist.x;
        } else {
            stepX = 1;
            sideDistX = (mapX + 1.0f - (player.position.x / TILE_SIZE)) * deltaDist.x;
        }
        
        if (rayDir.y < 0) {
            stepY = -1;
            sideDistY = ((player.position.y / TILE_SIZE) - mapY) * deltaDist.y;
        } else {
            stepY = 1;
            sideDistY = (mapY + 1.0f - (player.position.y / TILE_SIZE)) * deltaDist.y;
        }
        
        // DDA algorithm
        int hit = 0; // Was a wall hit?
        int side = 0; // Was a NS or EW wall hit?
        
        while (hit == 0) {
            // Jump to next map square, either in x-direction, or in y-direction
            if (sideDistX < sideDistY) {
                sideDistX += deltaDist.x;
                mapX += stepX;
                side = 0;
            } else {
                sideDistY += deltaDist.y;
                mapY += stepY;
                side = 1;
            }
            
            // Check if ray has hit a wall
            if (GetMapTile(map, mapX, mapY) > 0) {
                hit = 1;
            }
        }
        
        // Calculate distance projected on camera direction
        float perpWallDist;
        if (side == 0) {
            perpWallDist = (mapX - player.position.x / TILE_SIZE + (1 - stepX) / 2) / rayDir.x;
        } else {
            perpWallDist = (mapY - player.position.y / TILE_SIZE + (1 - stepY) / 2) / rayDir.y;
        }
        
        perpWallDist *= TILE_SIZE; // Scale by tile size
        
        // Calculate height of line to draw on screen
        int lineHeight = (int)(screenHeight / perpWallDist);
        
        // Calculate lowest and highest pixel to fill in current stripe
        int drawStart = -lineHeight / 2 + screenHeight / 2;
        if (drawStart < 0) drawStart = 0;
        
        int drawEnd = lineHeight / 2 + screenHeight / 2;
        if (drawEnd >= screenHeight) drawEnd = screenHeight - 1;
        
        // Choose wall color based on map value
        // Get texture index for later use with texturing
        // int texIndex = GetMapTile(map, mapX, mapY) % 8;
        
        // Choose wall color based on map value
        Color color;
        switch(GetMapTile(map, mapX, mapY)) {
            case TILE_WALL:      color = WHITE; break;
            case TILE_DOOR:      color = RED; break;
            case TILE_SECRET_WALL: color = GREEN; break;
            case TILE_OBSTACLE:  color = BLUE; break;
            default:             color = PURPLE; break;
        }
        
        // Make color darker for y-sides
        if (side == 1) {
            color.r = color.r / 2;
            color.g = color.g / 2;
            color.b = color.b / 2;
        }
        
        // Draw the pixels of the stripe as a vertical line
        DrawLine(x, drawStart, x, drawEnd, color);
    }
    
    // Draw minimap
    RenderMinimap(player, map);
}

void RenderMinimap(Player player, Map map) {
    // Define minimap size and position
    int mapSize = 150;
    int mapPosX = GetScreenWidth() - mapSize - 10;
    int mapPosY = 10;
    int cellSize = mapSize / MAP_WIDTH;
    
    // Draw minimap background
    DrawRectangle(mapPosX, mapPosY, mapSize, mapSize, ColorAlpha(BLACK, 0.7f));
    
    // Draw map cells
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            int cellType = GetMapTile(map, x, y);
            Color cellColor;
            
            switch (cellType) {
                case TILE_EMPTY:
                    cellColor = BLACK;
                    break;
                case TILE_WALL:
                    cellColor = WHITE;
                    break;
                case TILE_DOOR:
                    cellColor = RED;
                    break;
                case TILE_SECRET_WALL:
                    cellColor = GREEN;
                    break;
                case TILE_OBSTACLE:
                    cellColor = BLUE;
                    break;
                default:
                    cellColor = PURPLE;
                    break;
            }
            
            // Draw map cell
            DrawRectangle(mapPosX + x * cellSize, mapPosY + y * cellSize, cellSize, cellSize, cellColor);
        }
    }
    
    // Draw player position on minimap
    int playerMapX = mapPosX + (int)((player.position.x / TILE_SIZE) * cellSize);
    int playerMapY = mapPosY + (int)((player.position.y / TILE_SIZE) * cellSize);
    
    // Draw player as a circle
    DrawCircle(playerMapX, playerMapY, cellSize / 2, YELLOW);
    
    // Draw player direction
    DrawLine(
        playerMapX, 
        playerMapY, 
        playerMapX + (int)(player.direction.x * cellSize * 2),
        playerMapY + (int)(player.direction.y * cellSize * 2),
        RED
    );
    
    // Draw minimap border
    DrawRectangleLines(mapPosX, mapPosY, mapSize, mapSize, RAYWHITE);
}

void UnloadRenderer(void) {
    // Unload shader resources
    if (shadersLoaded) {
        UnloadShader(wallShader);
        shadersLoaded = false;
    }
}
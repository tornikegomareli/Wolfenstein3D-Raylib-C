#ifndef GAME_H
#define GAME_H

#include "raylib.h"
#include "resources.h"
#include "../World/player.h"
#include "../World/map.h"
#include "../Rendering/renderer.h"

typedef struct GameState {
    Player player;
    Map map;
    GameTextures textures;
    bool isRunning;
    bool mouseLookEnabled;
    bool showDebugInfo;
    Vector2 previousMousePosition;
    float mouseSensitivity;
    int screenshotCounter; // Counter for tracking screenshot numbers
} GameState;

// Game state management functions
void InitGame(GameState* state);
void UpdateGame(GameState* state);
void ProcessMapInteractions(GameState* state);
void RenderGame(GameState* state);
void UnloadGame(GameState* state);

#endif // GAME_H
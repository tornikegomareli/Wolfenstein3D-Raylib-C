#include "game.h"
#include "resources.h"
#include "../Rendering/renderer.h"
#include "../World/player.h"
#include "../World/map.h"
#include <stdio.h>

void InitGame(GameState* state) {
    // Initialize game state
    state->isRunning = true;
    state->mouseLookEnabled = true;
    state->previousMousePosition = (Vector2){ 0, 0 };
    state->mouseSensitivity = 0.1f;
    
    // Initialize resources
    LoadGameResources(&state->textures);
    
    // Initialize renderer
    InitRenderer();
    
    // Initialize map
    InitMap(&state->map);
    
    // Initialize player
    InitPlayer(&state->player, state->map);
    
    // Initialize debug info
    state->showDebugInfo = true;
}

void UpdateGame(GameState* state) {
    float deltaTime = GetFrameTime();
    
    // Toggle debug info with F1
    if (IsKeyPressed(KEY_F1)) {
        state->showDebugInfo = !state->showDebugInfo;
    }
    
    // Process mouse look if enabled
    if (state->mouseLookEnabled && IsCursorHidden()) {
        // Get mouse delta
        Vector2 mousePosition = GetMousePosition();
        Vector2 mouseDelta = { 
            mousePosition.x - state->previousMousePosition.x,
            mousePosition.y - state->previousMousePosition.y 
        };
        
        // Apply rotation based on mouse movement
        if ((mouseDelta.x != 0 || mouseDelta.y != 0) && IsWindowFocused()) {
            float rotationAmount = -mouseDelta.x * state->mouseSensitivity * deltaTime;
            RotatePlayer(&state->player, rotationAmount);
            
            // Reset mouse position to center of screen to allow continuous rotation
            int screenWidth = GetScreenWidth();
            int screenHeight = GetScreenHeight();
            SetMousePosition(screenWidth / 2, screenHeight / 2);
            mousePosition = (Vector2){ screenWidth / 2, screenHeight / 2 };
        }
        
        state->previousMousePosition = mousePosition;
    }
    
    // Update player
    UpdatePlayer(&state->player, state->map, deltaTime);
    
    // Update map (animations, etc.)
    UpdateMap(&state->map, deltaTime);
    
    // Test key bindings for door manipulation (for testing)
    ProcessMapInteractions(state);
}

void ProcessMapInteractions(GameState* state) {
    // Get player's current map position
    int playerX = (int)(state->player.position.x / TILE_SIZE);
    int playerY = (int)(state->player.position.y / TILE_SIZE);
    
    // Calculate the tile in front of the player
    int frontX = playerX + (int)(state->player.direction.x * 1.5f);
    int frontY = playerY + (int)(state->player.direction.y * 1.5f);
    
    // For testing: Space key to open/close doors in front of the player
    if (IsKeyPressed(KEY_SPACE)) {
        int tileType = GetMapTile(state->map, frontX, frontY);
        
        if (tileType == TILE_WALL) {
            // For testing: Turn walls into doors
            SetMapTile(&state->map, frontX, frontY, TILE_DOOR);
        } 
        else if (tileType == TILE_DOOR) {
            // Open doors (replace with empty space)
            SetMapTile(&state->map, frontX, frontY, TILE_EMPTY);
        }
    }
}

void RenderGame(GameState* state) {
    // Render the 3D world
    RenderWorld(state->player, state->map);
    
    // Draw debug information
    if (state->showDebugInfo) {
        // Get screen dimensions
        // int screenWidth = GetScreenWidth(); // Not used yet
        int screenHeight = GetScreenHeight();
        
        // Draw FPS
        DrawFPS(10, 10);
        
        // Player position and angle - with more vertical spacing
        char positionText[64];
        sprintf(positionText, "Position: (%.1f, %.1f)", state->player.position.x, state->player.position.y);
        DrawText(positionText, 10, 40, 20, RAYWHITE);
        
        char angleText[64];
        sprintf(angleText, "Angle: %.2f degrees", state->player.angle * RAD2DEG);
        DrawText(angleText, 10, 70, 20, RAYWHITE);
        
        // Controls help
        DrawText("Controls:", 10, screenHeight - 150, 20, YELLOW);
        DrawText("WASD: Move", 10, screenHeight - 120, 20, RAYWHITE);
        DrawText("Mouse/Arrows: Look", 10, screenHeight - 100, 20, RAYWHITE);
        DrawText("Space: Open door", 10, screenHeight - 80, 20, RAYWHITE);
        DrawText("ESC: Toggle mouse", 10, screenHeight - 60, 20, RAYWHITE);
        DrawText("F: Fullscreen", 10, screenHeight - 40, 20, RAYWHITE);
        DrawText("F1: Toggle debug", 10, screenHeight - 20, 20, RAYWHITE);
    }
}

void UnloadGame(GameState* state) {
    // Unload resources
    UnloadMap(&state->map);
    UnloadGameResources(&state->textures);
    UnloadRenderer();
}
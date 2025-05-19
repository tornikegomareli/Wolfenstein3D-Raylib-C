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
    state->screenshotCounter = 1; // Start screenshot numbering from 1

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

    // Toggle render mode with F2
    if (IsKeyPressed(KEY_F2)) {
        ToggleRenderMode();
    }

    // Take screenshot with P key
    if (IsKeyPressed(KEY_P)) {
        // Create a filename with the counter
        char screenshotFilename[64];
        sprintf(screenshotFilename, "screenshot_%03d.png", state->screenshotCounter++);

        // Take the screenshot using raylib's built-in function
        TakeScreenshot(screenshotFilename);

        // Provide feedback (will appear in console)
        TraceLog(LOG_INFO, "Screenshot saved: %s", screenshotFilename);
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
        int screenWidth = GetScreenWidth();
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

        // Map information
        char mapText[64];
        int playerMapX = (int)(state->player.position.x / TILE_SIZE);
        int playerMapY = (int)(state->player.position.y / TILE_SIZE);
        sprintf(mapText, "Map position: (%d, %d)", playerMapX, playerMapY);
        DrawText(mapText, 10, 100, 20, RAYWHITE);

        // Add wall information
        char wallText[128];
        int frontX = playerMapX + (int)(state->player.direction.x * 1.5f);
        int frontY = playerMapY + (int)(state->player.direction.y * 1.5f);
        int tileType = GetMapTile(state->map, frontX, frontY);
        sprintf(wallText, "Looking at: (%d,%d) Type: %d", frontX, frontY, tileType);
        DrawText(wallText, 10, 130, 20, GREEN);

        // Controls help
        DrawText("Controls:", 10, screenHeight - 190, 20, YELLOW);
        DrawText("WASD: Move", 10, screenHeight - 160, 20, RAYWHITE);
        DrawText("Mouse/Arrows: Look", 10, screenHeight - 140, 20, RAYWHITE);
        DrawText("Space: Open door", 10, screenHeight - 120, 20, RAYWHITE);
        DrawText("ESC: Toggle mouse", 10, screenHeight - 100, 20, RAYWHITE);
        DrawText("F: Fullscreen", 10, screenHeight - 80, 20, RAYWHITE);
        DrawText("P: Take screenshot", 10, screenHeight - 60, 20, RAYWHITE);
        DrawText("F1: Toggle debug", 10, screenHeight - 40, 20, RAYWHITE);
        DrawText("F2: Toggle Render Mode", 10, screenHeight - 20, 20, YELLOW);

        // Render status info at top right
        char statusInfo[64];
        sprintf(statusInfo, "Render Mode: %s", GetRenderModeName());
        DrawText(statusInfo, screenWidth - MeasureText(statusInfo, 45) - 10, 10, 20, YELLOW);
    }
}

void UnloadGame(GameState* state) {
    // Unload resources
    UnloadMap(&state->map);
    UnloadGameResources(&state->textures);
    UnloadRenderer();
}

#include "raylib.h"
#include "game.h"
#include "resources.h"
#include <stdio.h>

#define SCREEN_WIDTH 1280 
#define SCREEN_HEIGHT 720
#define GAME_TITLE "Wolf3D-GPU"

// Function prototypes
void ProcessInput(GameState* gameState);
void HandleWindowResize(void);

int main(void) {
    // Set up window configuration
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);
    
    // Initialize window and game
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, GAME_TITLE);
    SetTargetFPS(60);
    
    // Hide cursor for mouse look
    HideCursor();
    
    // Setup mouse for relative mode
    if (IsWindowFocused()) {
        DisableCursor();
    }
    
    // Initialize game state
    GameState gameState;
    InitGame(&gameState);
    
    // Main game loop
    while (!WindowShouldClose()) {
        // Handle input
        ProcessInput(&gameState);
        
        // Handle window resize if needed
        HandleWindowResize();
        
        // Update game state
        UpdateGame(&gameState);
        
        // Draw game
        BeginDrawing();
            ClearBackground(BLACK);
            RenderGame(&gameState);
            
            // Only show controls text if debug info is disabled
            if (!gameState.showDebugInfo) {
                DrawText("WASD: Move, Mouse/Arrows: Look, ESC: Exit", 10, 40, 20, RAYWHITE);
            }
        EndDrawing();
    }
    
    // Clean up
    UnloadGame(&gameState);
    CloseWindow();
    
    return 0;
}

void ProcessInput(GameState* gameState) {
    // Avoid unused parameter warnings
    (void)gameState;
    
    // ESC key handling
    if (IsKeyPressed(KEY_ESCAPE)) {
        // Toggle cursor visibility
        if (IsCursorHidden()) {
            EnableCursor();
            ShowCursor();
        } else {
            DisableCursor();
            HideCursor();
        }
    }
    
    // Handle fullscreen toggle
    if (IsKeyPressed(KEY_F)) {
        ToggleFullscreen();
    }
    
    // Mouse look is handled in UpdatePlayer function
}

void HandleWindowResize(void) {
    // If window was resized, adjust viewport
    if (IsWindowResized()) {
        // You might want to adjust the rendering based on new window size
        // For now we'll let raylib handle it automatically
    }
}
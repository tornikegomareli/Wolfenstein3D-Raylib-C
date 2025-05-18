#include "resources.h"
#include "raylib.h"

void LoadGameResources(GameTextures* textures) {
    // For now, generate placeholder textures
    // In a real implementation, you would load textures from files
    
    // Create checkerboard patterns for different wall textures
    for (int i = 0; i < 8; i++) {
        Color color1 = BLACK;
        Color color2 = WHITE;
        
        switch (i % 4) {
            case 0: color1 = RED; color2 = MAROON; break;
            case 1: color1 = GREEN; color2 = DARKGREEN; break;
            case 2: color1 = BLUE; color2 = DARKBLUE; break;
            case 3: color1 = YELLOW; color2 = GOLD; break;
        }
        
        Image img = GenImageChecked(64, 64, 8, 8, color1, color2);
        textures->walls[i] = LoadTextureFromImage(img);
        UnloadImage(img);
    }
    
    // Floor texture
    Image floorImg = GenImageChecked(64, 64, 16, 16, DARKGRAY, GRAY);
    textures->floor = LoadTextureFromImage(floorImg);
    UnloadImage(floorImg);
    
    // Ceiling texture
    Image ceilingImg = GenImageChecked(64, 64, 32, 32, SKYBLUE, BLUE);
    textures->ceiling = LoadTextureFromImage(ceilingImg);
    UnloadImage(ceilingImg);
    
    // Placeholder sprite textures
    for (int i = 0; i < 8; i++) {
        Image spriteImg = GenImageColor(64, 64, ColorAlpha(PURPLE, 0.0f));
        
        // Draw a simple shape in the center
        ImageDrawRectangle(&spriteImg, 16, 16, 32, 32, ColorAlpha(PURPLE, 1.0f));
        
        textures->sprites[i] = LoadTextureFromImage(spriteImg);
        UnloadImage(spriteImg);
    }
}

void UnloadGameResources(GameTextures* textures) {
    // Unload all textures
    for (int i = 0; i < 8; i++) {
        UnloadTexture(textures->walls[i]);
        UnloadTexture(textures->sprites[i]);
    }
    
    UnloadTexture(textures->floor);
    UnloadTexture(textures->ceiling);
}
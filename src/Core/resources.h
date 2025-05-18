#ifndef RESOURCES_H
#define RESOURCES_H

#include "raylib.h"

// Textures
typedef struct {
    Texture2D walls[8];  // Wall textures
    Texture2D floor;     // Floor texture
    Texture2D ceiling;   // Ceiling texture
    Texture2D sprites[8]; // Sprite textures
} GameTextures;

// Resource management functions
void LoadGameResources(GameTextures* textures);
void UnloadGameResources(GameTextures* textures);

#endif // RESOURCES_H
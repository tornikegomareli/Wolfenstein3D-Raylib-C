#ifndef RENDERER_H
#define RENDERER_H

#include "../World/player.h"
#include "../World/map.h"
#include "../Core/resources.h" // Add for texture access

// Shader configuration constants
#define MAX_LIGHTS 4

// Render modes
typedef enum {
    RENDER_MODE_CPU,  // CPU-based raycasting
    RENDER_MODE_GPU   // GPU-based shader rendering
} RenderMode;

// Renderer state
extern RenderMode currentRenderMode;

void InitRenderer(void);
void RenderWorld(Player player, Map map);
void RenderMinimap(Player player, Map map);
void UpdateShaders(Player player); // For updating shader parameters
void UnloadRenderer(void);
void ToggleRenderMode(void); // Switch between CPU and GPU rendering
const char* GetRenderModeName(void); // Get current render mode name for UI

#endif // RENDERER_H
#include "renderer.h"
#include "raylib.h"
#include "raymath.h"
#include "../World/map.h"
#include "../World/player.h"
#include <stdio.h>
#include <stdlib.h>

// Global render mode
RenderMode currentRenderMode = RENDER_MODE_CPU;

// Function prototypes for internal functions
static void InitGPURendering(void);
static void RenderWorldCPU(Player player, Map map);
static void RenderWorldGPU(Player player, Map map);

// Internal variables
static RenderTexture2D screenTexture = { 0 }; // For post-processing

// GPU rendering resources
static Shader wallShader = { 0 };
static Shader floorCeilingShader = { 0 };
static Model wallModel = { 0 };
static Model floorModel = { 0 };
static Model ceilingModel = { 0 };
static Mesh wallMesh = { 0 };
static Mesh floorMesh = { 0 };
static Mesh ceilingMesh = { 0 };
static bool shadersLoaded = false;
static bool modelsLoaded = false;

// Shader uniform locations (cached for performance)
static int wallHeightLoc = -1;
static int fogDensityLoc = -1;
static int darkFactorLoc = -1;
static int applyTexOffsetLoc = -1;
static int texOffsetLoc = -1;
static int isCeilingLoc = -1;
static int texScaleLoc = -1;
static int fcFogDensityLoc = -1;
static int fcDarknessLoc = -1;
static int cameraPositionLoc = -1;

void InitRenderer(void) {
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();
    
    // Create a render texture for potential post-processing
    screenTexture = LoadRenderTexture(screenWidth, screenHeight);
    
    // Initialize GPU rendering resources (even if we start with CPU rendering)
    InitGPURendering();
}

// Initialize GPU rendering resources
void InitGPURendering(void) {
    // Check if shader files exist before loading
    if (FileExists("resources/shaders/wall.vert") && FileExists("resources/shaders/wall.frag")) {
        wallShader = LoadShader("resources/shaders/wall.vert", "resources/shaders/wall.frag");
        shadersLoaded = (wallShader.id > 0);
    } else {
        TraceLog(LOG_WARNING, "Wall shader files not found. Using fallback CPU rendering");
        shadersLoaded = false;
    }
    
    if (FileExists("resources/shaders/floor_ceiling.vert") && FileExists("resources/shaders/floor_ceiling.frag")) {
        floorCeilingShader = LoadShader("resources/shaders/floor_ceiling.vert", "resources/shaders/floor_ceiling.frag");
    } else {
        TraceLog(LOG_WARNING, "Floor/ceiling shader files not found");
    }
    
    // Set shader locations for uniforms if shaders were loaded
    if (shadersLoaded) {
        // Wall shader uniforms
        wallShader.locs[SHADER_LOC_MATRIX_MVP] = GetShaderLocation(wallShader, "mvp");
        wallShader.locs[SHADER_LOC_VECTOR_VIEW] = GetShaderLocation(wallShader, "viewPos");
        wallShader.locs[SHADER_LOC_COLOR_DIFFUSE] = GetShaderLocation(wallShader, "colDiffuse");
        wallShader.locs[SHADER_LOC_MAP_DIFFUSE] = GetShaderLocation(wallShader, "texture0");
        wallHeightLoc = GetShaderLocation(wallShader, "wallHeight");
        fogDensityLoc = GetShaderLocation(wallShader, "fogDensity");
        darkFactorLoc = GetShaderLocation(wallShader, "darknessFactor");
        applyTexOffsetLoc = GetShaderLocation(wallShader, "applyTextureOffset");
        texOffsetLoc = GetShaderLocation(wallShader, "textureOffset");
        
        // Floor/ceiling shader uniforms
        floorCeilingShader.locs[SHADER_LOC_MATRIX_MVP] = GetShaderLocation(floorCeilingShader, "mvp");
        floorCeilingShader.locs[SHADER_LOC_VECTOR_VIEW] = GetShaderLocation(floorCeilingShader, "cameraPosition");
        floorCeilingShader.locs[SHADER_LOC_COLOR_DIFFUSE] = GetShaderLocation(floorCeilingShader, "colDiffuse");
        floorCeilingShader.locs[SHADER_LOC_MAP_DIFFUSE] = GetShaderLocation(floorCeilingShader, "texture0"); 
        floorCeilingShader.locs[SHADER_LOC_MAP_EMISSION] = GetShaderLocation(floorCeilingShader, "texture1");
        isCeilingLoc = GetShaderLocation(floorCeilingShader, "isCeiling");
        texScaleLoc = GetShaderLocation(floorCeilingShader, "textureScale");
        fcFogDensityLoc = GetShaderLocation(floorCeilingShader, "fogDensity");
        fcDarknessLoc = GetShaderLocation(floorCeilingShader, "floorCeilingDarkness");
        cameraPositionLoc = GetShaderLocation(floorCeilingShader, "cameraPosition");
        
        // Set default uniform values
        if (wallHeightLoc != -1) SetShaderValue(wallShader, wallHeightLoc, (float[1]){ 1.0f }, SHADER_UNIFORM_FLOAT);
        if (fogDensityLoc != -1) SetShaderValue(wallShader, fogDensityLoc, (float[1]){ 0.05f }, SHADER_UNIFORM_FLOAT);
        if (darkFactorLoc != -1) SetShaderValue(wallShader, darkFactorLoc, (float[1]){ 0.3f }, SHADER_UNIFORM_FLOAT);
        if (applyTexOffsetLoc != -1) SetShaderValue(wallShader, applyTexOffsetLoc, (int[1]){ 0 }, SHADER_UNIFORM_INT);
        if (texOffsetLoc != -1) SetShaderValue(wallShader, texOffsetLoc, (float[1]){ 0.0f }, SHADER_UNIFORM_FLOAT);
        
        if (isCeilingLoc != -1) SetShaderValue(floorCeilingShader, isCeilingLoc, (int[1]){ 0 }, SHADER_UNIFORM_INT);
        if (texScaleLoc != -1) SetShaderValue(floorCeilingShader, texScaleLoc, (float[1]){ 0.1f }, SHADER_UNIFORM_FLOAT);
        if (fcFogDensityLoc != -1) SetShaderValue(floorCeilingShader, fcFogDensityLoc, (float[1]){ 0.05f }, SHADER_UNIFORM_FLOAT);
        if (fcDarknessLoc != -1) SetShaderValue(floorCeilingShader, fcDarknessLoc, (float[1]){ 0.1f }, SHADER_UNIFORM_FLOAT);
    }
    
    // Create basic wall mesh (will be transformed by vertex shader)
    wallMesh = GenMeshCube(1.0f, 1.0f, 1.0f);
    wallModel = LoadModelFromMesh(wallMesh);
    SetMaterialTexture(&wallModel.materials[0], MATERIAL_MAP_DIFFUSE, (Texture2D){ 0 });
    
    // Create floor and ceiling planes
    floorMesh = GenMeshPlane(100.0f, 100.0f, 10, 10);
    floorModel = LoadModelFromMesh(floorMesh);
    SetMaterialTexture(&floorModel.materials[0], MATERIAL_MAP_DIFFUSE, (Texture2D){ 0 });
    
    ceilingMesh = GenMeshPlane(100.0f, 100.0f, 10, 10);
    ceilingModel = LoadModelFromMesh(ceilingMesh);
    SetMaterialTexture(&ceilingModel.materials[0], MATERIAL_MAP_EMISSION, (Texture2D){ 0 });
    
    // Apply shaders if loaded
    if (shadersLoaded) {
        wallModel.materials[0].shader = wallShader;
        floorModel.materials[0].shader = floorCeilingShader;
        ceilingModel.materials[0].shader = floorCeilingShader;
    }
    
    modelsLoaded = true;
}

void RenderWorld(Player player, Map map) {
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();
    
    // Decide which rendering method to use
    if (currentRenderMode == RENDER_MODE_GPU && shadersLoaded && modelsLoaded) {
        RenderWorldGPU(player, map);
    } else {
        RenderWorldCPU(player, map);
    }
}

// CPU-based raycasting rendering
void RenderWorldCPU(Player player, Map map) {
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
        
        // Use exact player position as ray origin to match minimap
        Vector2 rayPos = player.position;
        
        Vector2 rayDir = {
            player.direction.x + player.plane.x * cameraX,
            player.direction.y + player.plane.y * cameraX
        };
        
        // Map position - use rayPos instead of player.position directly
        int mapX = (int)(rayPos.x / TILE_SIZE);
        int mapY = (int)(rayPos.y / TILE_SIZE);
        
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
            sideDistX = ((rayPos.x / TILE_SIZE) - mapX) * deltaDist.x;
        } else {
            stepX = 1;
            sideDistX = (mapX + 1.0f - (rayPos.x / TILE_SIZE)) * deltaDist.x;
        }
        
        if (rayDir.y < 0) {
            stepY = -1;
            sideDistY = ((rayPos.y / TILE_SIZE) - mapY) * deltaDist.y;
        } else {
            stepY = 1;
            sideDistY = (mapY + 1.0f - (rayPos.y / TILE_SIZE)) * deltaDist.y;
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
            perpWallDist = (mapX - rayPos.x / TILE_SIZE + (1 - stepX) / 2) / rayDir.x;
        } else {
            perpWallDist = (mapY - rayPos.y / TILE_SIZE + (1 - stepY) / 2) / rayDir.y;
        }
        
        perpWallDist *= TILE_SIZE; // Scale by tile size
        
        // Apply a distance reduction factor to make walls appear closer
        perpWallDist *= 0.4f; // Further reduce distance perception for more dramatic closeness
        
        // Calculate height of line to draw on screen
        // Increase the perceived wall height by multiplying by a factor (makes walls appear closer)
        float distanceFactor = 2.0f; // Increased from 1.5 to 2.0 for taller walls (closer appearance)
        int lineHeight = (int)(screenHeight / perpWallDist * distanceFactor);
        
        // Calculate lowest and highest pixel to fill in current stripe
        int drawStart = -lineHeight / 2 + screenHeight / 2;
        if (drawStart < 0) drawStart = 0;
        
        int drawEnd = lineHeight / 2 + screenHeight / 2;
        if (drawEnd >= screenHeight) drawEnd = screenHeight - 1;
        
        // Choose wall color based on map value
        Color color;
        switch(GetMapTile(map, mapX, mapY)) {
            case TILE_WALL:      color = WHITE; break;
            case TILE_DOOR:      color = RED; break;
            case TILE_SECRET_WALL: color = GREEN; break;
            case TILE_OBSTACLE:  color = BLUE; break;
            default:             color = PURPLE; break;
        }
        
        // Make color darker for y-sides but keep them visible enough
        if (side == 1) {
            color.r = (color.r * 0.7f); // Less darkening (0.7 instead of 0.5)
            color.g = (color.g * 0.7f);
            color.b = (color.b * 0.7f);
        }
        
        // Enhance wall colors to make them more visible
        float colorEnhancement = 1.2f; // Slight enhancement to make walls pop more
        color.r = Clamp(color.r * colorEnhancement, 0, 255);
        color.g = Clamp(color.g * colorEnhancement, 0, 255);
        color.b = Clamp(color.b * colorEnhancement, 0, 255);
        
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

// GPU-based rendering with shaders
void RenderWorldGPU(Player player, Map map) {
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();
    
    // Update shader parameters before rendering
    UpdateShaders(player);
    
    // Set up 3D camera for the scene
    Camera3D camera = { 0 };
    camera.position = (Vector3){ player.position.x, 0.5f, player.position.y }; // Y is up in 3D space
    
    // Calculate camera target based on player direction
    camera.target = (Vector3){ 
        camera.position.x + player.direction.x,
        camera.position.y,
        camera.position.z + player.direction.y
    };
    
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
    camera.fovy = 60.0f;
    camera.projection = CAMERA_PERSPECTIVE;
    
    // Begin drawing to our render texture
    BeginTextureMode(screenTexture);
        ClearBackground(BLACK);
        
        // Begin 3D mode with our camera
        BeginMode3D(camera);
            
            // 1. Render floor
            // Set floor shader parameters
            if (isCeilingLoc != -1) {
                SetShaderValue(floorCeilingShader, isCeilingLoc, (int[1]){ 0 }, SHADER_UNIFORM_INT);
            }
            
            if (cameraPositionLoc != -1) {
                SetShaderValue(floorCeilingShader, cameraPositionLoc, 
                              (float[3]){ camera.position.x, camera.position.y, camera.position.z }, SHADER_UNIFORM_VEC3);
            }
            
            // Position the floor model below the camera
            Matrix floorTransform = MatrixTranslate(camera.position.x, 0.0f, camera.position.z);
            DrawMesh(floorMesh, floorModel.materials[0], floorTransform);
            
            // 2. Render ceiling
            // Set ceiling shader parameters
            if (isCeilingLoc != -1) {
                SetShaderValue(floorCeilingShader, isCeilingLoc, (int[1]){ 1 }, SHADER_UNIFORM_INT);
            }
            
            // Position the ceiling model above the camera
            Matrix ceilingTransform = MatrixTranslate(camera.position.x, 1.0f, camera.position.z);
            DrawMesh(ceilingMesh, ceilingModel.materials[0], ceilingTransform);
        
            // 3. Render walls
            // Iterate through visible map cells and render walls
            int playerMapX = (int)(player.position.x / TILE_SIZE);
            int playerMapY = (int)(player.position.y / TILE_SIZE);
            
            // Render walls in a radius around the player
            int renderRadius = 10; // Adjust based on performance needs
            
            for (int y = playerMapY - renderRadius; y <= playerMapY + renderRadius; y++) {
                for (int x = playerMapX - renderRadius; x <= playerMapX + renderRadius; x++) {
                    // Skip if out of bounds
                    if (x < 0 || y < 0 || x >= MAP_WIDTH || y >= MAP_HEIGHT) continue;
                    
                    int tileType = GetMapTile(map, x, y);
                    
                    // Skip empty spaces
                    if (tileType == TILE_EMPTY) continue;
                    
                    // Set wall color based on type
                    Color wallColor = WHITE;
                    switch(tileType) {
                        case TILE_WALL:       wallColor = WHITE; break;
                        case TILE_DOOR:       wallColor = RED; break;
                        case TILE_SECRET_WALL: wallColor = GREEN; break;
                        case TILE_OBSTACLE:   wallColor = BLUE; break;
                        default:              wallColor = PURPLE; break;
                    }
                    
                    // Use different wall textures based on the wall type
                    int texIndex = (tileType == TILE_WALL) ? (x + y) % 8 : tileType % 8;
                    
                    // Create wall transformation matrix
                    Matrix wallTransform = MatrixIdentity();
                    
                    // Position the wall at the correct grid position
                    wallTransform = MatrixMultiply(
                        wallTransform, 
                        MatrixTranslate(x * TILE_SIZE + TILE_SIZE/2, 0.5f, y * TILE_SIZE + TILE_SIZE/2)
                    );
                    
                    // Scale the wall to tile size
                    wallTransform = MatrixMultiply(
                        wallTransform,
                        MatrixScale(TILE_SIZE/2, 0.5f, TILE_SIZE/2)
                    );
                    
                    // Render the wall with texture and color
                    wallModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = map.wallTextures[texIndex];
                    wallModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].color = wallColor;
                    DrawMesh(wallMesh, wallModel.materials[0], wallTransform);
                }
            }
            
        EndMode3D();
        
    EndTextureMode();
    
    // Draw the final render texture to screen
    DrawTextureRec(
        screenTexture.texture, 
        (Rectangle){ 0, 0, (float)screenTexture.texture.width, (float)-screenTexture.texture.height },
        (Vector2){ 0, 0 }, 
        WHITE
    );
    
    // Draw minimap
    RenderMinimap(player, map);
}

void UpdateShaders(Player player) {
    if (!shadersLoaded) return;
    
    // Wall shader parameters
    if (wallHeightLoc != -1) {
        float wallHeight = 1.0f;
        SetShaderValue(wallShader, wallHeightLoc, &wallHeight, SHADER_UNIFORM_FLOAT);
    }
    
    if (fogDensityLoc != -1) {
        float fogDensity = 0.05f;
        SetShaderValue(wallShader, fogDensityLoc, &fogDensity, SHADER_UNIFORM_FLOAT);
    }
    
    if (darkFactorLoc != -1) {
        float darknessFactor = 0.3f;
        SetShaderValue(wallShader, darkFactorLoc, &darknessFactor, SHADER_UNIFORM_FLOAT);
    }
    
    // Floor/ceiling shader parameters
    if (cameraPositionLoc != -1) {
        float cameraPos[3] = { player.position.x, 0.5f, player.position.y };
        SetShaderValue(floorCeilingShader, cameraPositionLoc, cameraPos, SHADER_UNIFORM_VEC3);
    }
    
    if (texScaleLoc != -1) {
        float textureScale = 0.1f;
        SetShaderValue(floorCeilingShader, texScaleLoc, &textureScale, SHADER_UNIFORM_FLOAT);
    }
    
    if (fcFogDensityLoc != -1) {
        float fogDensity = 0.05f;
        SetShaderValue(floorCeilingShader, fcFogDensityLoc, &fogDensity, SHADER_UNIFORM_FLOAT);
    }
    
    if (fcDarknessLoc != -1) {
        float darknessFactor = 0.1f;
        SetShaderValue(floorCeilingShader, fcDarknessLoc, &darknessFactor, SHADER_UNIFORM_FLOAT);
    }
}

void UnloadRenderer(void) {
    // Unload GPU resources
    if (shadersLoaded) {
        UnloadShader(wallShader);
        UnloadShader(floorCeilingShader);
    }
    
    // Unload models and meshes
    if (modelsLoaded) {
        UnloadModel(wallModel);
        UnloadModel(floorModel);
        UnloadModel(ceilingModel);
    }
    
    // Unload render texture
    if (screenTexture.id > 0) {
        UnloadRenderTexture(screenTexture);
    }
}

void ToggleRenderMode(void) {
    // Switch between CPU and GPU rendering
    if (currentRenderMode == RENDER_MODE_CPU) {
        currentRenderMode = RENDER_MODE_GPU;
        TraceLog(LOG_INFO, "Switched to GPU rendering mode");
    } else {
        currentRenderMode = RENDER_MODE_CPU;
        TraceLog(LOG_INFO, "Switched to CPU rendering mode");
    }
}

const char* GetRenderModeName(void) {
    return (currentRenderMode == RENDER_MODE_CPU) ? "CPU" : "GPU";
}
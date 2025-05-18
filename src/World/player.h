#ifndef PLAYER_H
#define PLAYER_H

#include "raylib.h"
#include "map.h"

#define PLAYER_MOVE_SPEED 3.0f
#define PLAYER_ROTATE_SPEED 2.0f
#define PLAYER_COLLISION_RADIUS 0.2f

typedef struct Player {
    Vector2 position;  // Position in the world (x, y)
    float angle;       // View angle in radians
    Vector2 direction; // Direction vector (based on angle)
    Vector2 plane;     // Camera plane vector (for field of view)
    float moveSpeed;   // Movement speed
    float rotateSpeed; // Rotation speed
    float collisionRadius; // Collision radius
} Player;

void InitPlayer(Player* player, Map map);
void UpdatePlayer(Player* player, Map map, float deltaTime);
void MovePlayer(Player* player, Map map, float moveAmount, float strafeAmount);
void RotatePlayer(Player* player, float angle);
bool IsWallWithRadius(Map map, float x, float y, float radius);

#endif // PLAYER_H
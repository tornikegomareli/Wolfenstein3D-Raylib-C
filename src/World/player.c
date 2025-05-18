#include "player.h"
#include "math.h"

void InitPlayer(Player* player, Map map) {
    // Start player in a good starting position (in an empty area)
    player->position = (Vector2){ 2.5f * TILE_SIZE, 2.5f * TILE_SIZE };
    player->angle = 0.0f;
    
    // Initial direction vector - player starts facing east (1,0)
    player->direction = (Vector2){ 1.0f, 0.0f };
    
    // Camera plane is perpendicular to direction vector
    // The length of this vector determines the field of view
    player->plane = (Vector2){ 0.0f, 0.66f };
    
    // Movement speed and collision radius
    player->moveSpeed = PLAYER_MOVE_SPEED;
    player->rotateSpeed = PLAYER_ROTATE_SPEED;
    player->collisionRadius = PLAYER_COLLISION_RADIUS * TILE_SIZE;
    
    // Find a valid starting position if the player is in a wall
    int attempts = 0;
    while (IsWall(map, player->position.x, player->position.y) && attempts < 100) {
        player->position.x += TILE_SIZE;
        if (IsWall(map, player->position.x, player->position.y)) {
            player->position.x -= TILE_SIZE;
            player->position.y += TILE_SIZE;
        }
        attempts++;
    }
}

void UpdatePlayer(Player* player, Map map, float deltaTime) {
    float moveAmount = 0.0f;
    float strafeAmount = 0.0f;
    float rotateAmount = 0.0f;
    
    // Process input for movement
    if (IsKeyDown(KEY_W)) moveAmount += player->moveSpeed * deltaTime;
    if (IsKeyDown(KEY_S)) moveAmount -= player->moveSpeed * deltaTime;
    if (IsKeyDown(KEY_A)) strafeAmount -= player->moveSpeed * deltaTime;
    if (IsKeyDown(KEY_D)) strafeAmount += player->moveSpeed * deltaTime;
    
    // Process input for rotation with keyboard
    if (IsKeyDown(KEY_LEFT)) rotateAmount -= player->rotateSpeed * deltaTime;
    if (IsKeyDown(KEY_RIGHT)) rotateAmount += player->rotateSpeed * deltaTime;
    
    // Apply movement and rotation
    MovePlayer(player, map, moveAmount, strafeAmount);
    RotatePlayer(player, rotateAmount);
}

void MovePlayer(Player* player, Map map, float moveAmount, float strafeAmount) {
    // Early exit if no movement
    if (moveAmount == 0 && strafeAmount == 0) return;
    
    Vector2 newPos = player->position;
    
    // Forward/backward movement
    if (moveAmount != 0) {
        float moveDist = moveAmount * TILE_SIZE;
        float newX = player->position.x + player->direction.x * moveDist;
        float newY = player->position.y + player->direction.y * moveDist;
        
        // Apply collision detection with a sliding effect
        if (!IsWallWithRadius(map, newX, player->position.y, player->collisionRadius)) {
            newPos.x = newX;
        }
        
        if (!IsWallWithRadius(map, player->position.x, newY, player->collisionRadius)) {
            newPos.y = newY;
        }
    }
    
    // Strafe movement (perpendicular to direction)
    if (strafeAmount != 0) {
        float strafeDist = strafeAmount * TILE_SIZE;
        float perpX = -player->direction.y;
        float perpY = player->direction.x;
        
        float newX = newPos.x + perpX * strafeDist;
        float newY = newPos.y + perpY * strafeDist;
        
        // Apply collision detection with a sliding effect
        if (!IsWallWithRadius(map, newX, newPos.y, player->collisionRadius)) {
            newPos.x = newX;
        }
        
        if (!IsWallWithRadius(map, newPos.x, newY, player->collisionRadius)) {
            newPos.y = newY;
        }
    }
    
    player->position = newPos;
}

void RotatePlayer(Player* player, float angle) {
    if (angle == 0) return;
    
    // Rotate direction vector and camera plane
    float cosAngle = cosf(angle);
    float sinAngle = sinf(angle);
    
    // Rotate direction vector
    float oldDirX = player->direction.x;
    player->direction.x = player->direction.x * cosAngle - player->direction.y * sinAngle;
    player->direction.y = oldDirX * sinAngle + player->direction.y * cosAngle;
    
    // Normalize direction vector
    float dirLen = sqrtf(player->direction.x * player->direction.x + player->direction.y * player->direction.y);
    player->direction.x /= dirLen;
    player->direction.y /= dirLen;
    
    // Rotate camera plane
    float oldPlaneX = player->plane.x;
    player->plane.x = player->plane.x * cosAngle - player->plane.y * sinAngle;
    player->plane.y = oldPlaneX * sinAngle + player->plane.y * cosAngle;
    
    // Update player angle
    player->angle += angle;
    
    // Normalize angle to 0-2π range
    while (player->angle < 0) player->angle += 2 * PI;
    while (player->angle >= 2 * PI) player->angle -= 2 * PI;
}

// Additional helper function for collision detection with a radius
bool IsWallWithRadius(Map map, float x, float y, float radius) {
    // Check the center point
    if (IsWall(map, x, y)) return true;
    
    // Check cardinal directions at radius distance
    if (IsWall(map, x + radius, y)) return true;
    if (IsWall(map, x - radius, y)) return true;
    if (IsWall(map, x, y + radius)) return true;
    if (IsWall(map, x, y - radius)) return true;
    
    // Check diagonals at 0.7 * radius (approximately 1/sqrt(2) * radius)
    float diag = 0.7f * radius;
    if (IsWall(map, x + diag, y + diag)) return true;
    if (IsWall(map, x - diag, y + diag)) return true;
    if (IsWall(map, x + diag, y - diag)) return true;
    if (IsWall(map, x - diag, y - diag)) return true;
    
    return false;
}
#ifndef RENDERER_H
#define RENDERER_H

#include "../World/player.h"
#include "../World/map.h"

void InitRenderer(void);
void RenderWorld(Player player, Map map);
void RenderMinimap(Player player, Map map);
void UnloadRenderer(void);

#endif // RENDERER_H
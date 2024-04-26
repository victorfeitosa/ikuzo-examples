#ifndef __ENTITY_H__
#define __ENTITY_H__

#include <psxgpu.h>
#include "display.h"

typedef struct {
  TILE tile;
  uint16_t px;
  uint16_t py;
} Tile;

Tile CreateTile(uint8_t w, uint8_t h, uint8_t r, uint8_t g, uint8_t  b)
{
  Tile ent;
  setTile(&ent.tile);
  setWH(&ent.tile, w, h);
  setRGB0(&ent.tile, r, g, b);

  return ent;
}

void SortTile(RenderContext *ctx, Tile *ent, uint16_t px, uint16_t py)
{
  DrawEnv *buff = active_buffer(ctx);
  setXY0(&ent->tile, px, py);
  addPrim(buff->ot[1], &ent->tile);
}

#endif  //__ENTITY_H__
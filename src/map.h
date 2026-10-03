#include "core.h"

typedef enum {
  EMPTY, WALL, PILLAR, GATE
} CellType;

typedef struct {
  u8 active;
} Pillar;

typedef struct {
  u8 active;
  f32 offset;
} Gate;

typedef struct {
  u8 id;
  CellType type;
  union Type {
    Pillar pillar;
    Gate gate;
  } d;
} Cell;

typedef struct {
  Cell** cells;
  i16 width, height;
} Map;

extern Map map;

void read_map(const char* path);
Cell* map_at(u8 x, u8 y);

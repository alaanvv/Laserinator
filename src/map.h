typedef enum {
  EMPTY, WALL, PILLAR
} CellType;

typedef struct {
  int active;
} Pillar;

typedef struct {
  union Type {
    Pillar pillar;
  } data;
  CellType type;
} Cell;

Cell map[7][10] = {
  {(Cell) { .type = WALL }, (Cell) { .type = WALL }, (Cell) { .type = WALL }, (Cell) { .type = WALL }, (Cell) { .type = WALL }, (Cell) { .type = WALL }, (Cell) { .type = WALL }, (Cell) { .type = WALL }, (Cell) { .type = WALL }, (Cell) { .type = WALL }},
  {(Cell) { .type = WALL }, (Cell) { .type = EMPTY }, (Cell) { .type = EMPTY }, (Cell) { .type = EMPTY }, (Cell) { .type = EMPTY }, (Cell) { .type = EMPTY }, (Cell) { .type = EMPTY }, (Cell) { .type = EMPTY }, (Cell) { .type = EMPTY }, (Cell) { .type = WALL }},
  {(Cell) { .type = WALL }, (Cell) { .type = EMPTY }, (Cell) { .type = EMPTY }, (Cell) { .type = EMPTY }, (Cell) { .type = EMPTY }, (Cell) { .type = EMPTY }, (Cell) { .type = EMPTY }, (Cell) { .type = EMPTY }, (Cell) { .type = EMPTY }, (Cell) { .type = WALL }},
  {(Cell) { .type = WALL }, (Cell) { .type = WALL }, (Cell) { .type = PILLAR, .data.pillar.active = 0 }, (Cell) { .type = WALL }, (Cell) { .type = PILLAR, .data.pillar.active = 0 }, (Cell) { .type = WALL }, (Cell) { .type = PILLAR, .data.pillar.active = 0 }, (Cell) { .type = WALL }, (Cell) { .type = EMPTY }, (Cell) { .type = WALL }},
  {(Cell) { .type = WALL }, (Cell) { .type = EMPTY }, (Cell) { .type = EMPTY }, (Cell) { .type = EMPTY }, (Cell) { .type = EMPTY }, (Cell) { .type = EMPTY }, (Cell) { .type = EMPTY }, (Cell) { .type = EMPTY }, (Cell) { .type = EMPTY }, (Cell) { .type = WALL }},
  {(Cell) { .type = WALL }, (Cell) { .type = EMPTY }, (Cell) { .type = EMPTY }, (Cell) { .type = EMPTY }, (Cell) { .type = EMPTY }, (Cell) { .type = EMPTY }, (Cell) { .type = EMPTY }, (Cell) { .type = EMPTY }, (Cell) { .type = EMPTY }, (Cell) { .type = WALL }},
  {(Cell) { .type = WALL }, (Cell) { .type = WALL }, (Cell) { .type = WALL }, (Cell) { .type = WALL }, (Cell) { .type = WALL }, (Cell) { .type = WALL }, (Cell) { .type = WALL }, (Cell) { .type = WALL }, (Cell) { .type = WALL }, (Cell) { .type = WALL }},
};

int map2[10][10] = {
  {1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
  {1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
  {1, 0, 2, 0, 0, 0, 0, 0, 0, 1},
  {1, 0, 0, 0, 0, 0, 1, 0, 2, 1},
  {1, 0, 0, 0, 1, 1, 1, 0, 0, 1},
  {1, 1, 1, 0, 0, 0, 1, 2, 0, 1},
  {1, 0, 0, 0, 0, 0, 1, 0, 0, 1},
  {1, 2, 1, 0, 0, 0, 0, 0, 0, 1},
  {1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
  {1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
};

int map_bk[25][25] = {
  {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
  {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
  {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
  {1, 0, 0, 2, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1},
  {1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1},
  {1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1},
  {1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1},
  {1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
  {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1},
  {1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1},
  {1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1},
  {1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 2, 0, 1},
  {1, 0, 0, 2, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1},
  {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
  {1, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
  {1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 1},
  {1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1},
  {1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1},
  {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
  {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 1},
  {1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
  {1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1},
  {1, 0, 2, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1},
  {1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1},
  {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
};

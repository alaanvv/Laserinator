typedef enum {
  EMPTY, WALL, PILLAR, GATE
} CellType;

typedef struct {
  int active;
  int gate_id;
} Pillar;

typedef struct {
  int active;
  float offset;
  int id;
} Gate;

typedef struct {
  union Type {
    Pillar pillar;
    Gate gate;
  } data;
  CellType type;
} Cell;

Cell map[7][10] = {
  {(Cell) { .type = WALL }, (Cell) { .type = WALL }, (Cell) { .type = WALL }, (Cell) { .type = WALL }, (Cell) { .type = WALL }, (Cell) { .type = WALL }, (Cell) { .type = WALL }, (Cell) { .type = WALL }, (Cell) { .type = WALL }, (Cell) { .type = WALL }},
  {(Cell) { .type = WALL }, (Cell) { .type = EMPTY }, (Cell) { .type = EMPTY }, (Cell) { .type = EMPTY }, (Cell) { .type = EMPTY }, (Cell) { .type = EMPTY }, (Cell) { .type = EMPTY }, (Cell) { .type = EMPTY }, (Cell) { .type = EMPTY }, (Cell) { .type = WALL }},
  {(Cell) { .type = WALL }, (Cell) { .type = EMPTY }, (Cell) { .type = EMPTY }, (Cell) { .type = EMPTY }, (Cell) { .type = EMPTY }, (Cell) { .type = EMPTY }, (Cell) { .type = EMPTY }, (Cell) { .type = EMPTY }, (Cell) { .type = EMPTY }, (Cell) { .type = WALL }},
  {(Cell) { .type = WALL }, (Cell) { .type = WALL }, (Cell) { .type = PILLAR, .data.pillar.active = 0, .data.pillar.gate_id = 1 }, (Cell) { .type = WALL }, (Cell) { .type = PILLAR, .data.pillar.active = 0, .data.pillar.gate_id = -1 }, (Cell) { .type = WALL }, (Cell) { .type = PILLAR, .data.pillar.active = 0, .data.pillar.gate_id = -1 }, (Cell) { .type = WALL }, (Cell) { .type = GATE, .data.gate.active = 0, .data.gate.id = 1 }, (Cell) { .type = WALL }},
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

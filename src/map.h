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

Cell map[][10] = {
  {(Cell) { .type = WALL }, (Cell) { .type = WALL }, (Cell) { .type = WALL }, (Cell) { .type = WALL }, (Cell) { .type = WALL }, (Cell) { .type = WALL }, (Cell) { .type = WALL }, (Cell) { .type = WALL }, (Cell) { .type = WALL }, (Cell) { .type = WALL }},
  {(Cell) { .type = WALL }, (Cell) { .type = EMPTY }, (Cell) { .type = EMPTY }, (Cell) { .type = EMPTY }, (Cell) { .type = EMPTY }, (Cell) { .type = EMPTY }, (Cell) { .type = EMPTY }, (Cell) { .type = EMPTY }, (Cell) { .type = EMPTY }, (Cell) { .type = WALL }},
  {(Cell) { .type = WALL }, (Cell) { .type = EMPTY }, (Cell) { .type = EMPTY }, (Cell) { .type = EMPTY }, (Cell) { .type = EMPTY }, (Cell) { .type = EMPTY }, (Cell) { .type = EMPTY }, (Cell) { .type = EMPTY }, (Cell) { .type = EMPTY }, (Cell) { .type = WALL }},
  {(Cell) { .type = WALL }, (Cell) { .type = WALL }, (Cell) { .type = PILLAR, .data.pillar.gate_id = 1 }, (Cell) { .type = WALL }, (Cell) { .type = PILLAR, .data.pillar.gate_id = 2 }, (Cell) { .type = WALL }, (Cell) { .type = PILLAR, .data.pillar.gate_id = -1 }, (Cell) { .type = WALL }, (Cell) { .type = GATE, .data.gate.active = 0, .data.gate.id = 1 }, (Cell) { .type = WALL }},
  {(Cell) { .type = WALL }, (Cell) { .type = EMPTY }, (Cell) { .type = EMPTY }, (Cell) { .type = EMPTY }, (Cell) { .type = EMPTY }, (Cell) { .type = EMPTY }, (Cell) { .type = EMPTY }, (Cell) { .type = EMPTY }, (Cell) { .type = EMPTY }, (Cell) { .type = WALL }},
  {(Cell) { .type = WALL }, (Cell) { .type = EMPTY }, (Cell) { .type = EMPTY }, (Cell) { .type = EMPTY }, (Cell) { .type = EMPTY }, (Cell) { .type = EMPTY }, (Cell) { .type = EMPTY }, (Cell) { .type = EMPTY }, (Cell) { .type = EMPTY }, (Cell) { .type = WALL }},
  {(Cell) { .type = WALL }, (Cell) { .type = WALL }, (Cell) { .type = WALL }, (Cell) { .type = GATE, .data.gate.id = 2 }, (Cell) { .type = GATE, .data.gate.id = 2 }, (Cell) { .type = GATE, .data.gate.id = 2 }, (Cell) { .type = WALL }, (Cell) { .type = WALL }, (Cell) { .type = WALL }, (Cell) { .type = WALL }},
  {(Cell) { .type = WALL }, (Cell) { .type = EMPTY }, (Cell) { .type = EMPTY }, (Cell) { .type = EMPTY }, (Cell) { .type = EMPTY }, (Cell) { .type = EMPTY }, (Cell) { .type = EMPTY }, (Cell) { .type = EMPTY }, (Cell) { .type = EMPTY }, (Cell) { .type = WALL }},
  {(Cell) { .type = WALL }, (Cell) { .type = EMPTY }, (Cell) { .type = EMPTY }, (Cell) { .type = EMPTY }, (Cell) { .type = EMPTY }, (Cell) { .type = EMPTY }, (Cell) { .type = EMPTY }, (Cell) { .type = EMPTY }, (Cell) { .type = EMPTY }, (Cell) { .type = WALL }},
  {(Cell) { .type = WALL }, (Cell) { .type = EMPTY }, (Cell) { .type = EMPTY }, (Cell) { .type = EMPTY }, (Cell) { .type = EMPTY }, (Cell) { .type = EMPTY }, (Cell) { .type = EMPTY }, (Cell) { .type = EMPTY }, (Cell) { .type = EMPTY }, (Cell) { .type = WALL }},
  {(Cell) { .type = WALL }, (Cell) { .type = EMPTY }, (Cell) { .type = PILLAR }, (Cell) { .type = EMPTY }, (Cell) { .type = EMPTY }, (Cell) { .type = EMPTY }, (Cell) { .type = EMPTY }, (Cell) { .type = PILLAR }, (Cell) { .type = EMPTY }, (Cell) { .type = WALL }},
  {(Cell) { .type = WALL }, (Cell) { .type = EMPTY }, (Cell) { .type = EMPTY }, (Cell) { .type = EMPTY }, (Cell) { .type = EMPTY }, (Cell) { .type = EMPTY }, (Cell) { .type = EMPTY }, (Cell) { .type = EMPTY }, (Cell) { .type = EMPTY }, (Cell) { .type = WALL }},
  {(Cell) { .type = WALL }, (Cell) { .type = WALL }, (Cell) { .type = WALL }, (Cell) { .type = WALL }, (Cell) { .type = WALL }, (Cell) { .type = WALL }, (Cell) { .type = WALL }, (Cell) { .type = WALL }, (Cell) { .type = WALL }, (Cell) { .type = WALL }},
};

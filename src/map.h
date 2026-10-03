#include <stdio.h>
#include <stdlib.h>

typedef enum {
  EMPTY , WALL  , PILLAR, GATE
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
  CellType type;
  union Type {
    Pillar pillar;
    Gate gate;
  } data;
} Cell;

Cell** map;
int map_w, map_h;

void read_map() {
  FILE* file = fopen("map/map.txt", "r");

  fscanf(file, "%dx%d\n", &map_w, &map_h);

  char cell;
  map = malloc(sizeof(Cell*) * map_h);
  for (int y = 0; y < map_h; y++) {
    map[y] = malloc(sizeof(Cell) * map_w);
    for (int x = 0; x < map_w; x++) {
      fscanf(file, "%c", &cell);
      switch (cell) {
        case 'W': map[y][x].type = WALL; break;
        case 'E': map[y][x].type = EMPTY; break;
        case 'P': map[y][x].type = PILLAR; break;
        case 'G': map[y][x].type = GATE; break;
      }
    }
    fscanf(file, "\n");
  }

  int x, y, id;
  while (fscanf(file, "%*s %d %d %d", &x, &y, &id) != EOF) {
    switch (map[y][x].type) {
      case PILLAR: map[y][x].data.pillar.gate_id = id; break;
      case GATE: map[y][x].data.gate.id = id; break;
      default: break;
    }
  }
}

#include <stdio.h>
#include <stdlib.h>

#include "map.h"

Map map;

void read_map(const char* path) {
  FILE* file = fopen(path, "r");

  fscanf(file, "%hd %hd\n", &map.width, &map.height);

  char cell;
  i16 id;
  map.cells = malloc(sizeof(Cell*) * map.height);
  for (int y = 0; y < map.height; y++) {
    map.cells[y] = malloc(sizeof(Cell) * map.width);
    for (int x = 0; x < map.width; x++) {
      fscanf(file, "%c%hd", &cell, &id);
      switch (cell) {
        case 'W': map_at(x, y)->type = WALL;   break;
        case 'E': map_at(x, y)->type = EMPTY;  break;
        case 'P': map_at(x, y)->type = PILLAR; break;
        case 'G': map_at(x, y)->type = GATE;   break;
      }
      map_at(x, y)->id = id;
    }
    fscanf(file, "\n");
  }
}

Cell* map_at(u8 x, u8 y) {
  return &map.cells[y][x];
}

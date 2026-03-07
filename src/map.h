#ifndef MAP_H

#define MAP_H

void loadMaps();
void loadBlockSets();

struct block { int tiles[4][4]; };
struct block get_block();

struct blockSet {
  char *name;
  char *block_data;
};

struct connection {
  char *id;
  char *name;
  int offset;
};

struct map {
  char *name;
  char *id;
  struct blockSet *bst;
  struct connection north;
  struct connection east;
  struct connection south;
  struct connection west;
  char border_block;
  int width;
  int height;
  char *map_data;
};
struct map* get_map(char *map);

#endif

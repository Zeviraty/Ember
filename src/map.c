#define STB_DS_IMPLEMENTATION
#include "stb_ds.h"
#include <stdio.h>
#include "constants.h"
#include "tinydir.h"
#include <stdbool.h>
#include <ctype.h>

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

struct { char *key; struct map value; } *maps = NULL;
struct { char *key; struct blockSet value; } *bsts = NULL;

struct block { int tiles[4][4]; };

bool LOADED_MAPS = 0;
bool LOADED_BSTS = 0;

char *next_field(char **ptr, char *end) {
    if (*ptr >= end) return "";
    char *start = *ptr;
    while (*ptr < end && **ptr != '\0') (*ptr)++;
    if (*ptr < end) (*ptr)++;  // skip null
    return start;
};

char *next_subfield(char **ptr, char *end) {
    if (*ptr >= end) return "";
    char *start = *ptr;
    while (*ptr < end && **ptr != '\x01' && **ptr != '\0') (*ptr)++;
    if (*ptr < end) {
        **ptr = '\0';
        (*ptr)++;
    }
    return start;
};

char *till_end(char **ptr, char *end) {
    if (*ptr >= end) return "";
    char *start = *ptr;
    while (*ptr < end) (*ptr)++;
    return start;
};

void parse_map(char *buffer, size_t len, struct map *m, char **bst_name_out) {
    char *ptr = buffer;
    char *end = buffer + len;

    m->name = strdup(next_field(&ptr, end));
    m->id   = strdup(next_field(&ptr, end));
    *bst_name_out = next_field(&ptr, end);

    struct connection *dirs[4] = { &m->north, &m->east, &m->south, &m->west };

    for (int d = 0; d < 4; d++) {
        if (ptr >= end) break;
        char *field = next_field(&ptr, end);
        if (field[0] == '0' && field[1] == '\0') continue;

        char *sub    = field;
        char *rname  = next_subfield(&sub, end);
        char *id     = next_subfield(&sub, end);
        int offset   = atoi(sub);

        dirs[d]->id     = id;
        dirs[d]->name   = rname;
        dirs[d]->offset = offset;
    }
    m->width    = atoi(next_field(&ptr, end));
    m->height   = atoi(next_field(&ptr, end));
    m->map_data = till_end(&ptr, end);
}

void loadBlockSets() {
  tinydir_dir dir;
  size_t i;

  tinydir_open_sorted(&dir, BLOCKSETS_FOLDER);
#ifdef DEBUG
  printf("Opening: %s\n", BLOCKSETS_FOLDER);
  printf("Found %zu blocksets\n", dir.n_files);
#endif

  for (i = 0; i < dir.n_files; i++)
  {
    tinydir_file file;
    tinydir_readfile_n(&dir, &file, i);
    if (file.is_dir) continue;

    FILE *fptr = fopen(file.path, "rb");
    if (!fptr) continue;

    fseek(fptr, 0, SEEK_END);
    long fsize = ftell(fptr);
    rewind(fptr);

    char *buffer = (char*)calloc(fsize, 1);
    fread(buffer, 1, fsize, fptr);
    fclose(fptr);

    struct blockSet bst = {0};
    bst.name = strdup(file.name);
    bst.block_data = buffer;
    shput(bsts, bst.name, bst);
  }
  tinydir_close(&dir);
  LOADED_BSTS = 1;
#ifdef DEBUG
  for (int i = 0; i < shlen(bsts); ++i) printf("Name: %s\n", bsts[i].key);
  fflush(stdout);
#endif
};

void loadMaps() {
  tinydir_dir dir;
  size_t i;

  tinydir_open_sorted(&dir, MAPS_FOLDER);
#ifdef DEBUG
  printf("Opening: %s\n", MAPS_FOLDER);
  printf("Found %zu maps\n", dir.n_files);
#endif

  if (!LOADED_BSTS) { printf("ERROR: BLOCKSETS HAVE NOT LOADED BEFORE MAPS"); exit(1); }

  for (i = 0; i < dir.n_files; i++)
  {

    tinydir_file file;
    tinydir_readfile_n(&dir, &file, i);
    if (file.is_dir) continue;

    FILE *fptr = fopen(file.path, "rb");
    if (!fptr) continue;

    fseek(fptr, 0, SEEK_END);
    long fsize = ftell(fptr);
    rewind(fptr);

    char *buffer = (char*)calloc(fsize, 1);
    size_t bytes_read = fread(buffer, 1, fsize, fptr);
    fclose(fptr);

    struct map m = {0};
    char *bst_name = NULL;
    parse_map(buffer, bytes_read, &m, &bst_name);
    m.bst = (struct blockSet *)shgetp_null(bsts, bst_name);
    if (!m.bst) { printf("ERROR: blockset '%s' not found, skipping map '%s'\n", bst_name, m.name); continue; }
    m.bst = &shget(bsts, m.bst->name);

    shput(maps, m.name, m);
  }
  tinydir_close(&dir);
  LOADED_MAPS = 1;
#ifdef DEBUG
  for (int i = 0; i < shlen(maps); ++i) {
      printf("Name: %s | ID: %s | BlockSet: %s | North: %s | East: %s | South: %s | West %s\n",
          maps[i].key,
          maps[i].value.id,
          maps[i].value.bst->name,
          maps[i].value.north.id,
          maps[i].value.east.id,
          maps[i].value.south.id,
          maps[i].value.west.id);
  };
  fflush(stdout);
#endif
};

struct block get_block(char *blstr, int block_index) {
    struct blockSet bl = shget(bsts, blstr);
    struct block b = {0};
    int base = block_index * 16;
    for (int y = 0; y < 4; y++)
        for (int x = 0; x < 4; x++)
            b.tiles[y][x] = (unsigned char)bl.block_data[base + y*4 + x];
    return b;
}

struct map* get_map(char *map) {
    int idx = shgeti(maps, map);
    if (idx < 0) { printf("ERROR: map '%s' not found\n", map); exit(1); }
    return &maps[idx].value;
}

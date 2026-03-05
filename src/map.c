#define STB_DS_IMPLEMENTATION
#include "stb_ds.h"
#include <stdio.h>
#include "constants.h"
#include "tinydir.h"

struct blockSet {
  char *name;
};

struct connection {
  char *id;
  char *name;
  int offset;
};

struct map {
  char *name;
  char *id;
  struct blockSet bst;
  struct connection north;
  struct connection east;
  struct connection south;
  struct connection west;
  char* map_data;
};

struct { char *key; struct map value; } *maps = NULL;

char *next_field(char **ptr, char *end) {
    if (*ptr >= end) return "";
    char *start = *ptr;
    while (*ptr < end && **ptr != '\0') (*ptr)++;
    if (*ptr < end) (*ptr)++;  // skip null
    return start;
}

char *next_subfield(char **ptr, char *end) {
    if (*ptr >= end) return "";
    char *start = *ptr;
    while (*ptr < end && **ptr != '\x01' && **ptr != '\0') (*ptr)++;
    if (*ptr < end) {
        **ptr = '\0';
        (*ptr)++;
    }
    return start;
}

void parse_map(char *buffer, size_t len, struct map *m) {
    char *ptr = buffer;
    char *end = buffer + len;

    m->name     = next_field(&ptr, end);
    m->id       = next_field(&ptr, end);
    m->bst.name = next_field(&ptr, end);

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

    m->map_data = next_field(&ptr, end);
}

void loadMaps() {
  tinydir_dir dir;
  size_t i;

  tinydir_open_sorted(&dir, MAPS_FOLDER);
#ifdef DEBUG
  printf("Opening: %s\n", MAPS_FOLDER);
  printf("Found %zu maps\n", dir.n_files);
#endif

  for (i = 0; i < dir.n_files; i++)
  {
    tinydir_file file;
    tinydir_readfile_n(&dir, &file, i);
    if (file.is_dir) continue;

    FILE *fptr = fopen(file.path, "rb");
    if (!fptr) continue;

    char *buffer = (char*)calloc(150, sizeof(char));
    size_t bytes_read = fread(buffer, 1, 150, fptr);
    fclose(fptr);

    struct map m = {0};
    parse_map(buffer, bytes_read, &m);

    hmput(maps, m.name, m);
  }
  tinydir_close(&dir);
  for (int i = 0; i < hmlen(maps); ++i) {
    
  };
#ifdef DEBUG
  for (int i = 0; i < hmlen(maps); ++i) {
      printf("Name: %s | ID: %s | BlockSet: %s | North: %s | East: %s | South: %s | West %s\n",
          maps[i].value.name,
          maps[i].value.id,
          maps[i].value.bst.name,
          maps[i].value.north.id,
          maps[i].value.east.id,
          maps[i].value.south.id,
          maps[i].value.west.id);
  };
  fflush(stdout);
#endif
}

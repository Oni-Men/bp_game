#include "texture.h"

#include <handy.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define E TextureEntry *

void initTextureMap(TextureMap *texMap) {
  for (int i = 0; i < TEX_MAP_SIZE; i++) {
    texMap->textures[i] = -1;
  }
}

int loadTexture(TextureMap *texMap, int tex, const char *path) {
  int id = HgImageLoad(path);
  if (id == -1) {
    printf("Error: Unable to load %s\n", path);
    return -1;
  }

  texMap->textures[tex] = id;
  printf("Load %s: %d\n", path, id);
  return id;
}

int getTextureId(TextureMap *texMap, int tex) { return texMap->textures[tex]; }

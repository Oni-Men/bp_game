#include "texture.h"

#include <handy.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void initTextureMap(TextureMap *texMap) {
  for (int i = 0; i < TEX_MAP_SIZE; i++) {
    texMap->entries[i] = NULL;
  }
}

TextureEntry *loadTexture(const char *path) {
  TextureEntry *texture = malloc(sizeof(TextureEntry));
  texture->path = path;
  texture->textureId = HgImageLoad(path);
  texture->prev = NULL;
  texture->next = NULL;
  printf("Load %s: %d\n", path, texture->textureId);
  return texture;
}

void putTexture(TextureMap *texMap, TextureEntry *entry) {
  int index = calcHash(entry->path) % TEX_MAP_SIZE;
  if (texMap->entries[index] == NULL) {
    texMap->entries[index] = entry;
  } else {
    TextureEntry *next, *curr;
    curr = texMap->entries[index];
    int exit = false;
    while (!exit) {
      next = curr->next;
      // 次の要素がNULLだったら、そこに入れる
      if (curr->next == NULL) {
        curr->next = entry;
        entry->prev = curr;
        exit = true;
      }
      // パスが同じだったとき、エントリを置き換える
      else if (strcmp(entry->path, next->path) == 0) {
        entry->next = next->next;
        curr->next = entry;
        entry->prev = curr;
        exit = true;
      }
      curr = next;
    }
  }
}

TextureEntry *getTexture(TextureMap *texMap, const char *path) {
  int index = calcHash(path) % TEX_MAP_SIZE;
  TextureEntry *curr = texMap->entries[index];
  while (curr != NULL) {
    if (strcmp(path, curr->path) == 0) {
      return curr;
    } else if (curr->next != NULL) {
      curr = curr->next;
    } else {
      break;
    }
  }
  return NULL;
}

int getTextureId(TextureMap *texMap, const char *path) {
  TextureEntry *entry = getTexture(texMap, path);
  if (entry == NULL) {
    return -1;
  }
  return entry->textureId;
}

void removeTexture(TextureMap *texMap, const char *path) {
  TextureEntry *entry = getTexture(texMap, path);
}

void unloadAllTextures(TextureMap *texMap) {}

int calcHash(const char *str) {
  int hash = 0;
  for (; *str != '\0'; str++) {
    hash = (hash << 3) + *str;
  }
  return (hash < 0) ? -hash : hash;
}
#ifndef TEXTURE_H
#define TEXTURE_H

#define TEX_MAP_SIZE (64)
#define TEX_BG 0
#define TEX_PLAYER 1
#define TEX_ENEMY 2
#define TEX_ITEM 3
#define TEX_HEALTH 4
#define TEX_BULLET 5

typedef struct {
  int textures[TEX_MAP_SIZE];
} TextureMap;

void initTextureMap(TextureMap *texMap);

/**
 * @brief Load the texture from the path and create TextureEntry object.
 *
 * @param path
 * @return TextureEntry*
 */
int loadTexture(TextureMap *texMap, int tex, const char *path);

int getTextureId(TextureMap *texMap, int tex);

#endif
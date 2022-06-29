#define TEX_MAP_SIZE (64)

typedef struct textureEntry {
  int textureId;
  const char *path;
  struct textureEntry *next;
  struct textureEntey *prev;
} TextureEntry;

typedef struct {
  TextureEntry *entries[TEX_MAP_SIZE];
} TextureMap;

void initTextureMap(TextureMap *texMap);

/**
 * @brief Load the texture from the path and create TextureEntry object.
 *
 * @param path
 * @return TextureEntry*
 */
TextureEntry *loadTexture(const char *path);

/**
 * @brief Register the TextureEntry with the TextureMap you specified.
 *
 * @param texMap
 * @param entry
 */
void putTexture(TextureMap *texMap, TextureEntry *entry);

/**
 * @brief Remove the TextureEntry which associated with the given path.
 *
 * @param texMap
 * @param path
 */
void removeTexture(TextureMap *texMap, const char *path);

/**
 * @brief Unload all registered textures.
 *
 * @param texMap
 */
void unloadAllTextures(TextureMap *texMap);

/**
 * @brief Get the TextureEntry object which associated with the given path.
 * If the texture were not found. return NULL.
 *
 * @param path
 * @return int
 */
TextureEntry *getTexture(TextureMap *texMap, const char *path);

/**
 * @brief Get the texturId which associated with the given path.
 * If the texture were not found, return -1.
 *
 * @param path
 * @return int
 */
int getTextureId(TextureMap *texMap, const char *path);

int calcHash(const char *str);
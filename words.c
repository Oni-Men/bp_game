#include "words.h"

#include <stdlib.h>
#include <string.h>
#include <time.h>

#define BUFSIZE (1024)

typedef struct WordList *WordList;
struct WordList {
  char *val;
  WordList next;
};

size_t len_word_list;
char **word_list;

void LoadWords() {
  printf("section start: LoadWords\n");
  FILE *f;
  size_t str_len;
  char buf[BUFSIZE];

  if ((f = fopen(WORDS_FILE_PATH, "r")) == NULL) {
    printf("Couldn't open the file: %s\n", WORDS_FILE_PATH);
    return;
  }

  printf("start loading word list\n");

  WordList list = malloc(sizeof(WordList));
  WordList first = list;

  size_t size = 0;
  while (fgets(buf, BUFSIZE, f) != NULL) {
    char *p = strchr(buf, '\n');
    if (p) {
      *p = '\0';
    }
    str_len = strlen(buf);
    list->val = malloc(sizeof(char) * str_len);
    strncpy(list->val, buf, str_len);
    list->next = malloc(sizeof(WordList));
    list = list->next;
    size++;
  }
  list->next = NULL;
  list = first;

  WordList prev;
  len_word_list = size;
  word_list = malloc(sizeof(char **));
  for (size_t i = 0; i < len_word_list; i++) {
    word_list[i] = list->val;
    prev = list;
    list = list->next;
    free(prev);
  }

  fclose(f);

  printf("section info: Complete loading %zu words.\n", len_word_list);
  printf("section end: LoadWords\n");
}

char *GetRandomWord() {
  int r;
  srand(time(NULL));
  rand();
  r = ((double)rand() / RAND_MAX) * (len_word_list);

  return word_list[r];
}

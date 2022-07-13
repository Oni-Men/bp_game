#ifndef WORDS_H
#define WORDS_H
#include <stdio.h>

#define WORDS_FILE_PATH "./assets/words.txt"

/**
 * @brief ファイルから単語リストを読み込む
 *
 */
void LoadWords();

/**
 * @brief ランダムな単語を取得する
 */
char *GetRandomWord();

#endif
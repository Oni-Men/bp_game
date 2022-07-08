#include <stdio.h>

#define WORDS_FILE_PATH "./assets/words.txt"

/**
 * @brief ファイルから単語リストを読み込む
 *
 */
void LoadWords();

/**
 * @brief ランダムな単語を取得する
 *
 * @param pw 文字列へのポインタ
 * @param pn 文字列の長さへのポインタ
 */
char *GetRandomWord();
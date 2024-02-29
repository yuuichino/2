#include <stdio.h>
#include <stdlib.h> // いくつかの一般ユーティリティ関数とマクロを用いるために必要なヘッダファイルをインクルード
#include <string.h> // 文字列操作に関する関数を用いるために必要なヘッダファイルをインクルード
#include <math.h>
#include <unistd.h> // 多くのインプリメンテーション固有の関数を用いるために必要なヘッダファイルをインクルード
#include <netinet/in.h> // インターネット・プロトコル・ファミリーの定義を用いるために必要なヘッダファイルをインクルード
#include<stdbool.h> // 真理値の二つの値を取るやつ

// 定義
#define DEFAULT_PORT 51000 // デフォルトのポート番号を定義
#define MAX_LEN_NAME 10 // ユーザー名は最大１０文字
#define MAX_NUM_CLIENTS 5 // クライアント数は最大５
#define MAX_LEN_BUFFER 256 // バッファの最大文字数256
#define MAX_LEN_ADDR 32 // アドレスの最大文字数32
#define BROADCAST -1 // データ送信時のエラー

#define CMOVE_COMMAND 'M' // キャラの移動
#define IMOVE_COMMAND 'I' // アイテムの移動
#define MMOVE_COMMAND 'D' //カーソル移動コマンド
#define END_COMMAND 'E' // 終了する時のコマンド定義
#define NULL_COMMAND 'N' // データが何も無い時
#define START_COMMAND 'S' //同期コマンド
#define CSTART_COMMAND 'C'//同期完了コマンド

// クライアントの情報
typedef struct {
  int cid;
  int sock;
  struct sockaddr_in addr;
  char name[MAX_LEN_NAME];
} CLIENT;

// データの情報
typedef struct {
  int cid;
  char command;
  int cx;
  int cy;
  int ix;
  int iy;
  int mx;
  int my;
  int go;
  int gc;
  int win;
  int itemnum[4];
  int wana[4];
} CONTAINER;


#include <stdio.h>
#include <stdlib.h> // いくつかの一般ユーティリティ関数とマクロを用いるために必要なヘッダファイルをインクルード
#include "constants.h" // constants.hを用いるために必要なヘッダファイルをインクルード

// 関数の定義
extern void setup_server(int, u_short); // サーバの設定
extern int control_requests(); // コントロールをリクエスト 
extern void terminate_server(); // サーバーの終了

/************************************************************************
main関数
************************************************************************/
int main(int argc, char *argv[]) {
  int num_cl = 1;
  u_short port = DEFAULT_PORT;

  switch (argc) {
  case 1:
    break;
  case 2:
    num_cl = atoi(argv[1]);
    break;
  case 3:
    num_cl = atoi(argv[1]);
    port = atoi(argv[2]);
    break;
  default:
    fprintf(stderr, "Usage: %s [number of clients] [port number]\n", argv[0]);
    return 1;
  }

  if (num_cl < 0 || num_cl > MAX_NUM_CLIENTS) {
    fprintf(stderr, "Max number of clients is %d\n", MAX_NUM_CLIENTS);
    return 1;
  }

  fprintf(stderr, "Number of clients = %d\n", num_cl);
  fprintf(stderr, "Port number = %d\n", port);

  setup_server(num_cl, port); // サーバーの設定

  int cond = 1;
  while (cond) {
      cond = control_requests();
  }

  terminate_server(); // サーバーの終了

  return 0;
}

#include <stdio.h>
#include <stdlib.h> // いくつかの一般ユーティリティ関数とマクロを用いるために必要なヘッダファイルをインクルード
#include <string.h> // 文字列操作に関する関数を用いるために必要なヘッダファイルをインクルード
#include <unistd.h> // 多くのインプリメンテーション固有の関数を用いるために必要なヘッダファイルをインクルード
#include <sys/socket.h> // BSDソケットの中核となる関数とデータ構造を用いるために必要なヘッダファイルをインクルード
#include <sys/types.h> // typedefシンボルおよび構造体のコレクションを用いるために必要なヘッダファイルをインクルード
#include <netinet/in.h> // インターネット・プロトコル・ファミリーの定義を用いるために必要なヘッダファイルをインクルード
#include <arpa/inet.h> // インターネット操作用の定義を用いるために必要なヘッダファイルをインクルード
#include <errno.h> // エラー条件の報告に関するいくつかのマクロを用いるために必要なヘッダファイルをインクルード
#include "constants.h" // constants.hを用いるために必要なヘッダファイルをインクルード
#include <time.h>


// 引数の設定
static CLIENT clients[MAX_NUM_CLIENTS]; // クライアントの構造体
static int num_clients; // クライアント数
static int num_socks; // ソケット数
static fd_set mask; // maskをセット
static CONTAINER data; // メッセージについての構造体

// 関数の定義
void setup_server(int, u_short); // サーバーの設定
int control_requests(); // コントロールをリクエスト
void terminate_server(); // サーバーの終了

static void send_data(int, void *, int); // データを送る
static int receive_data(int, void *, int); // データを受け取る
static void handle_error(char *); // エラーの出力

//ゲーム開始フラグ
int st1 = 0;
int st2 = 0;
int st3 = 0;
int st4 = 0;

//乱数生成フラグ
int ax = 0;
/************************************************************************
setup_server関数
************************************************************************/
void setup_server(int num_cl, u_short port) {
  int rsock, sock = 0;
  struct sockaddr_in sv_addr, cl_addr;

  fprintf(stderr, "Server setup is started.\n");

  num_clients = num_cl; // クライアントの数を代入

  // ソケット生成
  rsock = socket(AF_INET, SOCK_STREAM, 0);
  if (rsock < 0) {
    handle_error("socket()"); // ソケット生成失敗時のエラー出力
  }
  fprintf(stderr, "sock() for request socket is done successfully.\n");

  sv_addr.sin_family = AF_INET; // アドレスの種類＝インターネット
  sv_addr.sin_port = htons(port); // ポート番号＝port
  sv_addr.sin_addr.s_addr = INADDR_ANY; // INADDR_ANY=任意アドレスから受付可

  int opt = 1;
  setsockopt(rsock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

  // ソケットに設定を結びつける
  if (bind(rsock, (struct sockaddr *)&sv_addr, sizeof(sv_addr)) != 0) {
    handle_error("bind()"); // ソケットに設定を結ぶことが出来なかった時のエラー出力
  }
  fprintf(stderr, "bind() is done successfully.\n");

  // 接続ソケットの準備
  if (listen(rsock, num_clients) != 0) {
    handle_error("listen()"); // 接続ソケットの準備失敗時のエラー出力
  }
  fprintf(stderr, "listen() is started.\n");

  int i, max_sock = 0;
  socklen_t len;
  char src[MAX_LEN_ADDR];
  for (i = 0; i < num_clients; i++) {
    len = sizeof(cl_addr);
    sock = accept(rsock, (struct sockaddr *)&cl_addr, &len); // 接続ソケットの生成と接続の確立
    if (sock < 0) {
      handle_error("accept()"); // 接続ソケットの生成と接続の確立失敗時のエラー出力
    }
    if (max_sock < sock) {
      max_sock = sock;
    }

    // コネクションからバッファへのデータ読み込み
    if (read(sock, clients[i].name, MAX_LEN_NAME) == -1) {
      handle_error("read()"); // コネクションからバッファへのデータ読み込み失敗時のエラー出力
    }
    clients[i].cid = i;
    clients[i].sock = sock;
    clients[i].addr = cl_addr;
    memset(src, 0, sizeof(src));
    inet_ntop(AF_INET, (struct sockaddr *)&cl_addr.sin_addr, src, sizeof(src));
    fprintf(stderr, "Client %d is accepted (name=%s, address=%s, port=%d).\n", i, clients[i].name, src, ntohs(cl_addr.sin_port));
  }

  close(rsock);

  int j;
  for (i = 0; i < num_clients; i++) {
    send_data(i, &num_clients, sizeof(int)); // クライアント数のデータを送信
    send_data(i, &i, sizeof(int)); // iの数のデータを送信
    for (j = 0; j < num_clients; j++) {
      send_data(i, &clients[j], sizeof(CLIENT)); // クライアントのデータを送信
    }
  }

  num_socks = max_sock + 1;
  FD_ZERO(&mask); // maskをゼロにクリア
  FD_SET(0, &mask); // 0番目のFDに対応する値を1にセット

  for (i = 0; i < num_clients; i++) {
    FD_SET(clients[i].sock, &mask); // clients[i].sock番目のFDに対応する値を1にセット
  }
  fprintf(stderr, "Server setup is done.\n");
}

/************************************************************************
contorol_requests関数
************************************************************************/
int control_requests() {
  fd_set read_flag = mask;
  memset(&data, 0, sizeof(CONTAINER));
  if (select(num_socks, (fd_set *)&read_flag, NULL, NULL, NULL) == -1) {
    handle_error("select()"); // FDの集合から読み込み/書き込み/例外発生のFDの発見の失敗時にエラー出力
  }

  int i, result = 1;
  for (i = 0; i < num_clients; i++) {
    if (FD_ISSET(clients[i].sock, &read_flag)) { // clients[i].sock番目のFDが1かどうか確認
      receive_data(i, &data, sizeof(data)); 
      switch (data.command) {
      case END_COMMAND: // 終了のコマンド
        send_data(BROADCAST, &data, sizeof(data)); // 終了のデータを送信
        result = 0;
        break;
      case CMOVE_COMMAND: // キャラ移動コマンド
        printf("%dPlayerがCMOVE通ってるよ\n", data.cid);
        send_data(BROADCAST, &data, sizeof(data)); // キャラの座標データを送信
        result = 1;
        ax=0;
        break;
      case IMOVE_COMMAND: // アイテム移動コマンド
        send_data(BROADCAST, &data, sizeof(data));
        result = 1;
        break;
      case MMOVE_COMMAND: // カーソル移動コマンド
        send_data(BROADCAST, &data, sizeof(data));
        result = 1;
        break;
      case NULL_COMMAND: // コマンドが何もない時
        send_data(BROADCAST, &data, sizeof(data)); // キャラの座標データを送信
        result = 1;

        break;
      case START_COMMAND:
        printf("Receive START by %d\n", data.cid);
        switch(data.cid){
              case 0: 
                st1 = 1;
                break;
              case 1: 
                st2 = 1;
                break;
              case 2: 
                st3 = 1;
                break;
              case 3: 
                st4 = 1;
                break;
            }
        
          
        if( (st1 + st2 + st3 + st4) == num_clients) {
            data.command = CSTART_COMMAND;
            if(ax == 0){
            srand(time(NULL)); //乱数初期化
            printf("aiueo\n");
            ax = 1;
              for(int i = 0 ; i<4 ; i++){
                data.itemnum[i] = rand() % 8;
                printf("after srand itemnum == %d.\n", data.itemnum[i]);
              }
            }

            printf("CSTART\n");
            st1 = 0;
            st2 = 0;
            st3 = 0;
            st4 = 0;
        }
        
        else {
            data.command = NULL_COMMAND;
        }
        result = 1;
        send_data(BROADCAST, &data, sizeof(data));
        printf("Receive START by %d\n", data.cid);
        break;

      default: // その他のコマンド
        fprintf(stderr, "control_requests(): %c is not a valid command.\n", data.command);
        printf("data.command == %c.\n", data.command);
        exit(1);
      }
    }
  }

  return result;
}

/************************************************************************
send_data関数
************************************************************************/
static void send_data(int cid, void *data, int size) {
  if ((cid != BROADCAST) && (0 > cid || cid >= num_clients)) {
    fprintf(stderr, "send_data(): client id is illeagal.\n");
    exit(1);
  }
  if ((data == NULL) || (size <= 0)) {
    fprintf(stderr, "send_data(): data is illeagal.\n");
    exit(1);
  }

  if (cid == BROADCAST) {
    int i;
    for (i = 0; i < num_clients; i++) {
      if (write(clients[i].sock, data, size) < 0) {
        handle_error("write()"); // 書き込み失敗時のエラー出力
      }
    }
  } else {
    if (write(clients[cid].sock, data, size) < 0) {
      handle_error("write()"); // 書き込み失敗時のエラー出力
    }
  }
}

/************************************************************************
receive_data関数
************************************************************************/
static int receive_data(int cid, void *data, int size) {
  if ((cid != BROADCAST) && (0 > cid || cid >= num_clients)) {
    fprintf(stderr, "receive_data(): client id is illeagal.\n");
    exit(1);
  }
  if ((data == NULL) || (size <= 0)) {
    fprintf(stderr, "receive_data(): data is illeagal.\n");
  	exit(1);
  }

  return read(clients[cid].sock, data, size); //コネクションからバッファへのデータを読み込んだ値を返す
}

/************************************************************************
handle_error関数
************************************************************************/
static void handle_error(char *message) {
  perror(message);
  fprintf(stderr, "%d\n", errno);
  exit(1);
}

/************************************************************************
terminate_client関数
************************************************************************/
void terminate_server(void) {
  int i;
  for (i = 0; i < num_clients; i++) {
    close(clients[i].sock);
  }
  fprintf(stderr, "All connections are closed.\n");
  exit(0);
}
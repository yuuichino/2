#include <stdio.h>
#include <stdlib.h> // いくつかの一般ユーティリティ関数とマクロを用いるために必要なヘッダファイルをインクルード
#include <string.h> // 文字列操作に関する関数を用いるために必要なヘッダファイルをインクルード
#include <unistd.h> // 多くのインプリメンテーション固有の関数を用いるために必要なヘッダファイルをインクルード 
#include <sys/socket.h> // BSDソケットの中核となる関数とデータ構造を用いるために必要なヘッダファイルをインクルード 
#include <sys/types.h> // typedefシンボルおよび構造体のコレクションを用いるために必要なヘッダファイルをインクルード
#include <netinet/in.h> // インターネット・プロトコル・ファミリーの定義を用いるために必要なヘッダファイルをインクルード
#include <netdb.h> // プロトコル名とホスト名を数値アドレスに変換するのに必要なヘッダファイルをインクルード
#include <errno.h> // エラー条件の報告に関するいくつかのマクロを用いるために必要なヘッダファイルをインクルード

#include "constants.h" // constants.hを用いるために必要なヘッダファイルをインクルード

//変数の設定
static int num_clients; //クライアント数
static int myid; //クライアントID
static int sock; //ソケット
static int num_sock; //ソケットの数
static fd_set mask; //maskをセット
static CLIENT clients[MAX_NUM_CLIENTS]; //クライアントの構造体

//関数の定義
int setup_client(char *, u_short); //クライアントの設定
void terminate_client();//クライアントの終了

void send_data(void *, int); //データを送る
int receive_data(void *, int); //データを受け取る
void handle_error(char *); //エラーの出力
int clients_count(); //クライアント数を共有

/************************************************************************
setup_client関数
************************************************************************/
int setup_client(char *server_name, u_short port) {
  struct hostent *server;
  struct sockaddr_in sv_addr;

  fprintf(stderr, "Trying to connect server %s (port = %d).\n", server_name, port);
  if ((server = gethostbyname(server_name)) == NULL) {
    handle_error("gethostbyname()"); // サーバーネームが入力されなかった時のエラー出力
  }

  //ソケット生成
  sock = socket(AF_INET, SOCK_STREAM, 0); 
  if (sock < 0) {
    handle_error("socket()"); // ソケット生成失敗時のエラー出力
  }

  sv_addr.sin_family = AF_INET; // アドレスの種類＝インターネット
  sv_addr.sin_port = htons(port); // ポート番号＝port
  sv_addr.sin_addr.s_addr = *(u_int *)server->h_addr_list[0];

  //クライアントからサーバーへの接続要求
  if (connect(sock, (struct sockaddr *)&sv_addr, sizeof(sv_addr)) != 0) {
    handle_error("connect()"); // クライアントからサーバーへの接続要求失敗時のエラー出力
  }

  fprintf(stderr, "Input your name: ");
  char user_name[MAX_LEN_NAME]; // 最大１０文字でユーザー名を入力
  if (fgets(user_name, sizeof(user_name), stdin) == NULL) {
    handle_error("fgets()"); // ユーザー名が入力されなかった時のエラー出力
  }
  user_name[strlen(user_name) - 1] = '\0'; // ユーザー名を格納した配列の最後に\0を入れる
  send_data(user_name, MAX_LEN_NAME); // ユーザー名のデータを送る

  fprintf(stderr, "Waiting for other clients...\n");
  receive_data(&num_clients, sizeof(int)); //クライアントの数のデータを受け取る
  fprintf(stderr, "Number of clients = %d.\n", num_clients);
  receive_data(&myid, sizeof(int)); //クライアントのIDを受け取る
  fprintf(stderr, "Your ID = %d.\n", myid);
  int i;
  for (i = 0; i < num_clients; i++) {
    receive_data(&clients[i], sizeof(CLIENT)); //クライアントのデータを受け取る
  }

  num_sock = sock + 1;
  FD_ZERO(&mask); // maskをゼロクリア
  FD_SET(0, &mask); //0番目のFDに対応する値を１にセット
  FD_SET(sock, &mask); //sock番目のFDに対応する値を１にセット
  fprintf(stderr, "Connect is completed \n");

  return myid;
}

/************************************************************************
send_data関数
************************************************************************/
void send_data(void *data, int size) {
  if ((data == NULL) || (size <= 0)) {
        fprintf(stderr, "send_data(): data is illeagal.\n");
	exit(1);
  }

  if (write(sock, data, size) == -1) {
    handle_error("write()"); // 書き込み失敗時のエラー出力
  }
}

/************************************************************************
receive_data関数
************************************************************************/
int receive_data(void *data, int size) {
  if ((data == NULL) || (size <= 0)) {
    fprintf(stderr, "receive_data(): data is illeagal.\n");
  	exit(1);
  }

  return(read(sock, data, size)); // コネクションからバッファへのデータを読み込んだ値を返す
}

/************************************************************************
handle_error関数
************************************************************************/
void handle_error(char *message) {
  perror(message);
  fprintf(stderr, "%d\n", errno);
  exit(1);
}

/************************************************************************
terminate_client関数
************************************************************************/
void terminate_client() {
  fprintf(stderr, "Connection is closed.\n");
  close(sock);
  exit(0);
}

/************************************************************************
clients_count関数
************************************************************************/
int clients_count(){
  return num_clients;
}
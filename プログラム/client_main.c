#include <stdio.h>
#include <stdlib.h>
#include <time.h> // 乱数を初期化するために必要
#include <math.h>
#include <SDL2/SDL_image.h> // 画像処理系
#include <SDL2/SDL_mixer.h> // 音声処理系
#include <SDL2/SDL_ttf.h> // フォント処理系
#include <SDL2/SDL2_gfxPrimitives.h> // 図形処理系
#include <SDL2/SDL_net.h> // ネットワーク処理系
#include <SDL2/SDL.h>	// SDLを用いるために必要なヘッダファイル

#include<stdbool.h> // 真理値の二つの値を取るやつ

#include "constants.h" // constants.hを用いるために必要なヘッダファイルをインクルード

#define timernum 1

SDL_Event event; // イベントデータを格納する構造体

//関数の定義
extern int setup_client(char *, u_short); // クライアントの設定
extern int control_requests(); // コントロールのリクエスト
extern void terminate_client(); // クライアントの終了
extern void send_data(void *data, int size);
extern int receive_data(void *data, int size);
extern int clients_count();


typedef struct { // キャラクターの状態を管理する構造体
  SDL_bool right; // 右移動
  SDL_bool left; // 左移動
  SDL_bool jump; // ジャンプ
  SDL_bool dash; // ダッシュ
  SDL_bool exist; //存在
  SDL_bool zimen; // キャラが地面と触れている
  SDL_bool fall; // キャラが自由落下
  SDL_bool atama; // キャラの頭がオブジェクトの底（天井？床？）に当たったとき
} CharaStts;

typedef struct { // 罠の状態を管理する構造体
  SDL_bool right;
  SDL_bool left;
  SDL_bool up;
  SDL_bool down;
  SDL_bool listup; // 罠選択画面を切り替えるフラグ
} ItemStts;

typedef struct { // ラウンドごとの設置アイテムの画像を保存するやつ
  SDL_Texture *itemtextureround11;
  SDL_Texture *itemtextureround12;
  SDL_Texture *itemtextureround13;
  SDL_Texture *itemtextureround14;
  SDL_Texture *itemtextureround21;
  SDL_Texture *itemtextureround22;
  SDL_Texture *itemtextureround23;
  SDL_Texture *itemtextureround24;
  SDL_Texture *itemtextureround31;
  SDL_Texture *itemtextureround32;
  SDL_Texture *itemtextureround33;
  SDL_Texture *itemtextureround34;
  SDL_Texture *itemtextureround41;
  SDL_Texture *itemtextureround42;
  SDL_Texture *itemtextureround43;
  SDL_Texture *itemtextureround44;
  SDL_Texture *itemtextureround51;
  SDL_Texture *itemtextureround52;
  SDL_Texture *itemtextureround53;
  SDL_Texture *itemtextureround54;       
  SDL_Texture *itemtextureround61;
  SDL_Texture *itemtextureround62;
  SDL_Texture *itemtextureround63;
  SDL_Texture *itemtextureround64;   
  SDL_Texture *itemtextureround71;
  SDL_Texture *itemtextureround72; 
  SDL_Texture *itemtextureround73;
  SDL_Texture *itemtextureround74;  
  SDL_Texture *itemtextureround81;
  SDL_Texture *itemtextureround82;
  SDL_Texture *itemtextureround83;
  SDL_Texture *itemtextureround84;     
} ItemTextureRound;

typedef struct { // 罠画面で表示する４つのアイテムの画像を保存するやつ
  SDL_Texture *itemtextureList1;
  SDL_Texture *itemtextureList2;
  SDL_Texture *itemtextureList3;
  SDL_Texture *itemtextureList4;
} ItemTextureList;

typedef struct{
  SDL_Rect dst_itemround1;
  SDL_Rect dst_itemround2;
  SDL_Rect dst_itemround3;
  SDL_Rect dst_itemround4;
  SDL_Rect dst_itemround5;
  SDL_Rect dst_itemround6;
  SDL_Rect dst_itemround7;
  SDL_Rect dst_itemround8;
  SDL_Rect dst_itemround11; // ラウンドごとの罠の画像の座標
  SDL_Rect dst_itemround12; //  番号の意味は(round数,クライアント番号)
  SDL_Rect dst_itemround13;
  SDL_Rect dst_itemround14;
  SDL_Rect dst_itemround21;
  SDL_Rect dst_itemround22;
  SDL_Rect dst_itemround23;
  SDL_Rect dst_itemround24;
  SDL_Rect dst_itemround31;
  SDL_Rect dst_itemround32;
  SDL_Rect dst_itemround33;
  SDL_Rect dst_itemround34;
  SDL_Rect dst_itemround41;
  SDL_Rect dst_itemround42;
  SDL_Rect dst_itemround43;
  SDL_Rect dst_itemround44;
  SDL_Rect dst_itemround51;
  SDL_Rect dst_itemround52;
  SDL_Rect dst_itemround53;
  SDL_Rect dst_itemround54;
  SDL_Rect dst_itemround61;
  SDL_Rect dst_itemround62;
  SDL_Rect dst_itemround63;
  SDL_Rect dst_itemround64;
  SDL_Rect dst_itemround71;
  SDL_Rect dst_itemround72;
  SDL_Rect dst_itemround73;
  SDL_Rect dst_itemround74;
  SDL_Rect dst_itemround81;
  SDL_Rect dst_itemround82;
  SDL_Rect dst_itemround83;
  SDL_Rect dst_itemround84;
}ItemCoordinate;

typedef struct { // 移動する罠の初期座標を保存しておく構造体
  SDL_Rect MoveItemRect11;
  SDL_Rect MoveItemRect12;
  SDL_Rect MoveItemRect13;
  SDL_Rect MoveItemRect14;

  SDL_Rect MoveItemRect21;
  SDL_Rect MoveItemRect22;
  SDL_Rect MoveItemRect23;
  SDL_Rect MoveItemRect24;
  
  SDL_Rect MoveItemRect31;
  SDL_Rect MoveItemRect32;
  SDL_Rect MoveItemRect33;
  SDL_Rect MoveItemRect34;

  SDL_Rect MoveItemRect41;
  SDL_Rect MoveItemRect42;
  SDL_Rect MoveItemRect43;
  SDL_Rect MoveItemRect44;

  SDL_Rect MoveItemRect51;
  SDL_Rect MoveItemRect52;
  SDL_Rect MoveItemRect53;
  SDL_Rect MoveItemRect54;

  SDL_Rect MoveItemRect61;
  SDL_Rect MoveItemRect62;
  SDL_Rect MoveItemRect63;
  SDL_Rect MoveItemRect64;

  SDL_Rect MoveItemRect71;
  SDL_Rect MoveItemRect72;
  SDL_Rect MoveItemRect73;
  SDL_Rect MoveItemRect74;

  SDL_Rect MoveItemRect81;
  SDL_Rect MoveItemRect82;
  SDL_Rect MoveItemRect83;
  SDL_Rect MoveItemRect84;
    
} MoveItemRects;

typedef struct {
  SDL_bool ground; // 地面
} MapStts;

CharaStts CS;
ItemStts IS;

MapStts MS;
ItemTextureList ITL;
ItemTextureRound ITR;
ItemCoordinate IC;

MoveItemRects MIR;

static CONTAINER data1;
static CONTAINER data2;

// メイン関数
int main(int argc, char* argv[]){

  IS.listup = true;

  int fg = 0; //　ウィンドウ切り替えのフラグ
  int pg = 0; //  罠選択画面フラグ  2はキャラ操作 0は罠選択 1は罠移動&設置
  int gc = 0; // ゲームクリアフラグ
  int go = 1; // ゲーム可能フラグ
  int win = 0; // 勝利カウント
  int win1 = 0; // 勝利カウント1
  int win2 = 0; // 勝利カウント2
  int win3 = 0; // 勝利カウント3
  int win4 = 0; // 勝利カウント4
  int win_result1 = 0; // 最終ラウンド勝利数
  int win_result2 = 0; // 最終ラウンド勝利数
  int win_result = 0; // 最終ラウンド勝利数
  int triple = 0;

  //各プレイヤー毎の点数兼リザルト画面上のx座標
  int pl1 = 380;
  int pl2 = 380;
  int pl3 = 380;
  int pl4 = 380; 

 //画像座標用
  int chara1x = 0;
  int chara1y = 0;
  int chara2x = 0;
  int chara2y = 0;
  int chara3x = 0;
  int chara3y = 0;
  int chara4x = 0;
  int chara4y = 0;

  //デス時flag
  int death[4] = {0,0,0,0};

  //ゴール時flag
  int goal[4] = {0,0,0,0};

  // 現在が何ラウンド目か
  int round = 1;
  
  // アイテム一覧を表示するときの初期座標
  int itemx[4] = {500, 700, 500, 700};
  int itemy[4] = {250, 250, 450, 450};

  u_short port = DEFAULT_PORT;
  char server_name[MAX_LEN_NAME];

  sprintf(server_name, "localhost");

  switch (argc) {
    case 1:
      break;
    case 2:
      sprintf(server_name, "%s", argv[1]);
      break;
    case 3:
      sprintf(server_name, "%s", argv[1]);
      port = (u_short)atoi(argv[2]);
      break;
    default:
      fprintf(stderr, "Usage: %s [server name] [port number]\n", argv[0]);
      return 1;
  }

  data1.cid = setup_client(server_name, port); // クライアントの設定
  
  SDL_Window* window; // ウィンドウデータを格納する構造体
  SDL_Renderer* renderer; // 2Dレンダリングコンテキスト(描画設定)を格納する構造体

  SDL_Rect building1 = {50, 555, 210, 290}; // 左のビル 
  SDL_Rect building2 = {966, 275, 305, 30}; // 右のビル

  SDL_Rect chara = {100, 505, 24, 50};  // キャラの位置情報
  SDL_Rect chara1 = {100, 505, 24, 50};  // キャラの位置情報
  SDL_Rect chara2 = {110, 505, 24, 50};  // キャラの位置情報
  SDL_Rect chara3 = {120, 505, 24, 50};  // キャラの位置情報
  SDL_Rect chara4 = {130, 505, 24, 50};  // キャラの位置情報

  SDL_Rect chara_prev = chara;

  SDL_Rect cursor = {100, 100, 20, 20}; 
  SDL_Rect cursor1 = {100, 100, 20, 20}; // 罠選択時のカーソル
  SDL_Rect cursor2 = {100, 100, 20, 20}; // 罠選択時のカーソル 
  SDL_Rect cursor3 = {100, 100, 20, 20}; // 罠選択時のカーソル 
  SDL_Rect cursor4 = {100, 100, 20, 20}; // 罠選択時のカーソル 

  SDL_Rect dst_item = {0, 0, 50, 50};
  SDL_Rect dst_item1 = {0, 0, 50, 50}; // 罠選択画面の罠画像の初期座標
  SDL_Rect dst_item2 = {0, 0, 50, 50}; 
  SDL_Rect dst_item3 = {0, 0, 50, 50}; 
  SDL_Rect dst_item4 = {0, 0, 50, 50}; 

  SDL_Rect dst_itemround1; // ラウンドごとの罠の画像の座標
  SDL_Rect dst_itemround2;
  SDL_Rect dst_itemround3;
  SDL_Rect dst_itemround4;
  SDL_Rect dst_itemround5;
  SDL_Rect dst_itemround6;
  SDL_Rect dst_itemround7;
  SDL_Rect dst_itemround8;

  SDL_bool marutogeflag = 0;

  int shine[8][4]; //[ラウンド][プレイヤー数] = 0(生)or1(死)
  int i,j;
  for(i=0;i<8;i++){
    for(j=0;j<4;j++){
      shine[i][j] = 0;
    }
  }

  int dosunl[8][4];//dosun1
    for(i=0;i<8;i++){
    for(j=0;j<4;j++){
      dosunl[i][j] = 0;
    }
  }

  int dosunr[8][4];//dosun2
    for(i=0;i<8;i++){
    for(j=0;j<4;j++){
      dosunr[i][j] = 0;
    }
  }
  int migikonbea[8][4];//migi
    for(i=0;i<8;i++){
    for(j=0;j<4;j++){
      migikonbea[i][j] = 0;
    }
  }
    int hidarikonbea[8][4];//hidari
    for(i=0;i<8;i++){
    for(j=0;j<4;j++){
      hidarikonbea[i][j] = 0;
    }
  }
 
  int marutoge[8][4];//marutoge
    for(i=0;i<8;i++){
    for(j=0;j<4;j++){
      marutoge[i][j] = 0;
    }
  }
  SDL_Surface *itemimage1 = IMG_Load("ing/zimen.png"); // 罠を読み込む(位置検討?)
  SDL_Surface *itemimage2 = IMG_Load("ing/migi.png");
  SDL_Surface *itemimage3 = IMG_Load("ing/hidari.png");
  SDL_Surface *itemimage4 = IMG_Load("ing/dosunl.png");
  SDL_Surface *itemimage5 = IMG_Load("ing/dosunr.png");
  SDL_Surface *itemimage6 = IMG_Load("ing/brack1.png");
  SDL_Surface *itemimage7 = IMG_Load("ing/MARUTOGE.png");
  SDL_Surface *itemimage8 = IMG_Load("ing/lzi2.png");

// 配列とか　末尾のaはarrayの略
SDL_Rect building_a[] = {building1, building2}; // 左右のビル
//罠選択画面の罠画像の初期座標
SDL_Rect dst_item_a[] = {dst_item1, dst_item2, dst_item3, dst_item4};
//罠選択画面の罠を保存しておくtextureの配列
SDL_Texture *ITLa[] = {ITL.itemtextureList1, ITL.itemtextureList2, ITL.itemtextureList3, ITL.itemtextureList4};
// 読み込んだ罠の画像の配列
SDL_Surface *IIa[] = {itemimage1, itemimage2, itemimage3, itemimage4, itemimage5, itemimage6, itemimage7, itemimage8};
// ラウンドごとの罠の画像を保存しておくtextureの配列（クライアント数である4つだけ用意する)
SDL_Texture *ITRa1[] = {ITR.itemtextureround11, ITR.itemtextureround21, ITR.itemtextureround31, ITR.itemtextureround41, ITR.itemtextureround51, ITR.itemtextureround61, ITR.itemtextureround71, ITR.itemtextureround81};
SDL_Texture *ITRa2[] = {ITR.itemtextureround12, ITR.itemtextureround22, ITR.itemtextureround32, ITR.itemtextureround42, ITR.itemtextureround52, ITR.itemtextureround62, ITR.itemtextureround72, ITR.itemtextureround82};
SDL_Texture *ITRa3[] = {ITR.itemtextureround13, ITR.itemtextureround23, ITR.itemtextureround33, ITR.itemtextureround43, ITR.itemtextureround53, ITR.itemtextureround63, ITR.itemtextureround73, ITR.itemtextureround83};
SDL_Texture *ITRa4[] = {ITR.itemtextureround14, ITR.itemtextureround24, ITR.itemtextureround34, ITR.itemtextureround44, ITR.itemtextureround54, ITR.itemtextureround64, ITR.itemtextureround74, ITR.itemtextureround84};
// ラウンドごとの罠の座標を保存しておくrectの配列（クライアント数である4つだけ用意する)
SDL_Rect dst_itemround_a[] = {IC.dst_itemround1, IC.dst_itemround2, IC.dst_itemround3, IC.dst_itemround4, IC.dst_itemround5, IC.dst_itemround6, IC.dst_itemround7, IC.dst_itemround8};
SDL_Rect dst_itemround_a1[] = {IC.dst_itemround11, IC.dst_itemround21, IC.dst_itemround31, IC.dst_itemround41, IC.dst_itemround51, IC.dst_itemround61, IC.dst_itemround71, IC.dst_itemround81};
SDL_Rect dst_itemround_a2[] = {IC.dst_itemround12, IC.dst_itemround22, IC.dst_itemround32, IC.dst_itemround42, IC.dst_itemround52, IC.dst_itemround62, IC.dst_itemround72, IC.dst_itemround82};
SDL_Rect dst_itemround_a3[] = {IC.dst_itemround13, IC.dst_itemround23, IC.dst_itemround33, IC.dst_itemround43, IC.dst_itemround53, IC.dst_itemround63, IC.dst_itemround73, IC.dst_itemround83};
SDL_Rect dst_itemround_a4[] = {IC.dst_itemround14, IC.dst_itemround24, IC.dst_itemround34, IC.dst_itemround44, IC.dst_itemround54, IC.dst_itemround64, IC.dst_itemround74, IC.dst_itemround84};

// 移動する罠の初期座標を保存しておくrectの配列
SDL_Rect MoveItemRects_a1[] = {MIR.MoveItemRect11, MIR.MoveItemRect21, MIR.MoveItemRect31, MIR.MoveItemRect41, MIR.MoveItemRect51, MIR.MoveItemRect61, MIR.MoveItemRect71, MIR.MoveItemRect81};
SDL_Rect MoveItemRects_a2[] = {MIR.MoveItemRect12, MIR.MoveItemRect22, MIR.MoveItemRect32, MIR.MoveItemRect42, MIR.MoveItemRect52, MIR.MoveItemRect62, MIR.MoveItemRect72, MIR.MoveItemRect82};
SDL_Rect MoveItemRects_a3[] = {MIR.MoveItemRect13, MIR.MoveItemRect23, MIR.MoveItemRect33, MIR.MoveItemRect43, MIR.MoveItemRect53, MIR.MoveItemRect63, MIR.MoveItemRect73, MIR.MoveItemRect83};
SDL_Rect MoveItemRects_a4[] = {MIR.MoveItemRect14, MIR.MoveItemRect24, MIR.MoveItemRect34, MIR.MoveItemRect44, MIR.MoveItemRect54, MIR.MoveItemRect64, MIR.MoveItemRect74, MIR.MoveItemRect84};


  SDL_Thread *thread;
  SDL_atomic_t atm;
  
  /**************************************/
  /* Wiiリモコン関連の初期化（ここから） */
  /**************************************/

  /**************************************/
  /* Wiiリモコン関連の初期化（ここまで） */
  /**********************************
  
  /**************************************/
  /* SDL関連の初期化（ここから）         */
  /**************************************/
  
  // SDL初期化
  if(SDL_Init(SDL_INIT_EVERYTHING) < 0) {
    printf("failed to initialize SDL.\n");
    exit(-1);
  }

  	// 描画する画像形式を指定して初期化
	IMG_Init(IMG_INIT_PNG);
  IMG_Init(IMG_INIT_JPG);

    // MP3ファイルをよみこむための初期化
  Mix_Init(MIX_INIT_MP3);

  if(Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 1024)==-1){
    printf("Failed: %s\n", Mix_GetError());
    exit(-1);
  } 


  char s[256];
  sprintf(s, "Player %d",data1.cid + 1);
  // ウィンドウ生成・表示SDL_RenderCopy(renderer, texture1, &src_rect1, &dst_rect1); // テクスチャをレンダラーにコピー（設定のサイズで）
  window = SDL_CreateWindow(s, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1300, 800, 0);
  if(window == NULL){
    printf("Failed to create window.\n");
    exit(-1);
  }

  // 描画処理
  renderer = SDL_CreateRenderer(window, -1, 0); // 生成したウィンドウに対してレンダリングコンテキスト（RC）を生成
  SDL_SetRenderDrawColor(renderer, 255, 255, 255, 0); // 生成したRCに描画色として白を設定
  SDL_RenderClear(renderer); // 生成したRCを白でクリア＝塗りつぶす（ただし，メモリに描画データを反映させただけなので，画面には表示されない）
  
  /**************************************/
  /* SDL関連の初期化（ここまで）         */
  /**************************************/

  int timer = 0; //ジャンプの処理が早くなりすぎないようにタイミングを管理する
  int y_temp; // ジャンプの重力とかそのへん
  int y_prev; // ジャンプの重力とかそのへん2
  int ccolor = 0; // キャラクターの色？たぶんいらない 
  int y_zimen = 555;

  Mix_Music *music; // Music 型でデータを格納する変数を宣言

  music = Mix_LoadMUS("ing/BGM.mp3"); // Music型で読み込み

  Mix_PlayMusic(music, -1); // Music型サウンドを無限に再生

  printf("preVol:%d\n", Mix_VolumeMusic(MIX_MAX_VOLUME/2)); // 音量を半分にして, 前の音量を表示
  printf("Current volume: %d\n", Mix_VolumeMusic(-1));
   
  SDL_Surface *image4 = IMG_Load("ing/start.png"); 
  SDL_Texture *texture5 = SDL_CreateTextureFromSurface(renderer, image4); // 読み込んだ画像からテクスチャを作成
  SDL_QueryTexture(texture5, NULL, NULL, &image4->w, &image4->h); // 画像の情報を取得
  SDL_Rect src_rect5 = {0, 0, image4->w, image4->h}; // コピー元画像の領域（この場合，画像全体が設定される）
  SDL_Rect dst_rect5 = {0, 0, 1300, 800}; // 画像のコピー先の座標と領域（x, y, w, h）

  SDL_Surface *image = IMG_Load("ing/map001.png"); 
  SDL_Texture *texture1 = SDL_CreateTextureFromSurface(renderer, image); // 読み込んだ画像からテクスチャを作成
  SDL_QueryTexture(texture1, NULL, NULL, &image->w, &image->h); // 画像の情報を取得
  SDL_Rect src_rect1 = {0, 0, image->w, image->h}; // コピー元画像の領域（この場合，画像全体が設定される）
  SDL_Rect dst_rect1 = {0, 0, 1300, 800}; // 画像のコピー先の座標と領域（x, y, w, h）
    
  SDL_Surface *image2 = IMG_Load("ing/win.png"); 
  SDL_Texture *texture3 = SDL_CreateTextureFromSurface(renderer, image2); // 読み込んだ画像からテクスチャを作成
  SDL_QueryTexture(texture3, NULL, NULL, &image2->w, &image2->h); // 画像の情報を取得
  SDL_Rect src_rect3 = {0, 0, image2->w, image2->h}; // コピー元画像の領域（この場合，画像全体が設定される）
  SDL_Rect dst_rect3 = {100, 100, 1100, 600}; // 画像のコピー先の座標と領域（x, y, w, h）

  SDL_Surface *image3 = IMG_Load("ing/lose.png"); 
  SDL_Texture *texture4 = SDL_CreateTextureFromSurface(renderer, image3); // 読み込んだ画像からテクスチャを作成
  SDL_QueryTexture(texture4, NULL, NULL, &image3->w, &image3->h); // 画像の情報を取得
  SDL_Rect src_rect4 = {0, 0, image3->w, image3->h}; // コピー元画像の領域（この場合，画像全体が設定される）
  SDL_Rect dst_rect4 = {100, 100, 1100, 600}; // 画像のコピー先の座標と領域（x, y, w, h）

  SDL_Surface *image1 = IMG_Load("ing/wana.png"); 
  SDL_Surface *charaimage1 = IMG_Load("ing/chara1.png");
  SDL_Surface *charaimage2 = IMG_Load("ing/chara2.png");
  SDL_Surface *charaimage3 = IMG_Load("ing/chara3.png");
  SDL_Surface *charaimage4 = IMG_Load("ing/chara4.png");


 SDL_Texture *texture2 = SDL_CreateTextureFromSurface(renderer, image1); // 読み込んだ画像からテクスチャを作成
      
 SDL_Texture *charatexture1 = SDL_CreateTextureFromSurface(renderer,charaimage1);
 SDL_Texture *charatexture2 = SDL_CreateTextureFromSurface(renderer,charaimage2);
 SDL_Texture *charatexture3 = SDL_CreateTextureFromSurface(renderer,charaimage3);
 SDL_Texture *charatexture4 = SDL_CreateTextureFromSurface(renderer,charaimage4);

// surface itemimageをここでtextureに写しておく
SDL_Texture *iitexture1 = SDL_CreateTextureFromSurface(renderer, IIa[0]); // IIa = ItemImageArray = 罠画像のsurface
SDL_Texture *iitexture2 = SDL_CreateTextureFromSurface(renderer, IIa[1]);
SDL_Texture *iitexture3 = SDL_CreateTextureFromSurface(renderer, IIa[2]);
SDL_Texture *iitexture4 = SDL_CreateTextureFromSurface(renderer, IIa[3]);
SDL_Texture *iitexture5 = SDL_CreateTextureFromSurface(renderer, IIa[4]);
SDL_Texture *iitexture6 = SDL_CreateTextureFromSurface(renderer, IIa[5]);
SDL_Texture *iitexture7 = SDL_CreateTextureFromSurface(renderer, IIa[6]);
SDL_Texture *iitexture8 = SDL_CreateTextureFromSurface(renderer, IIa[7]);
// 罠の画像すべてが保存されているtextureの配列
SDL_Texture *IITa[] = {iitexture1, iitexture2, iitexture3, iitexture4, iitexture5, iitexture6, iitexture7, iitexture8};
 
 SDL_QueryTexture(texture2, NULL, NULL, &image1->w, &image1->h); // 画像の情報を取得
  SDL_Rect src_rect2 = {0, 0, image1->w, image1->h}; // コピー元画像の領域（この場合，画像全体が設定される）
  SDL_Rect dst_rect2 = {145, 130, 960, 540}; // 像のコピー先の座標と領域（x, y, w, h）
  SDL_Rect src_item1 = {0,0,50,50};

  SDL_Rect result = {325, 200, 600, 400}; // 像のコピー先の座標と領域（x, y, w, h）;   
  

  /**************************************/
  /* 処理開始                            */
  /**************************************/


// Wiiリモコンの代わりにキーボードを使用する
 int itemnum[4]; // 数種類のアイテムから1つランダムに選ぶ乱数を格納する

 int cond = 1;
 while(cond){
 
  if(fg == 0){
    Mix_PauseMusic(); // 再生中のサウンドを一時停止
    SDL_RenderCopy(renderer, texture5, &src_rect5, &dst_rect5); // テクスチャをレンダラーにコピー（設定のサイズで）
    // 文字列を描画
    stringColor(renderer, 525, 700, "Please Push the enter key and start", 0xff000000); 
    SDL_RenderPresent(renderer);
  } // ifの終了
  else if(fg == 1){
      Mix_ResumeMusic(); // 一時停止を解除
      SDL_RenderCopy(renderer,texture1,&src_rect1,&dst_rect1);
      if(IS.listup == true){
      printf("IS.listup == true.\n" );
      }
      

    if(pg == 0 && IS.listup == true ){ // 罠設置状態の時
      int i;
      for(i = 0; i<4; i++){ // 1から8までのアイテムの中から4つ選ぶ 
        itemnum[i] = data2.itemnum[i]; // 0から7までの乱数
        printf("itemnum == %d.\n", itemnum[i]);
        ITLa[i] = IITa[itemnum[i]]; // 罠画像をリストにコピー
        dst_item_a[i].x = itemx[i]; // 画像の初期座標をアイテムと同期
        dst_item_a[i].y = itemy[i];

      } // forの終わり
      for(i=0;i<4;i++){//キャラ状態をデフォルトに戻す
        death[i] = 0;
        goal[i] = 0;
      }

    IS.listup = false;

  } // ifの終わり

    SDL_SetRenderDrawColor(renderer, 225, 225, 0, 0); // 生成したレンダラーに描画色（RGBA）を設定

    //罠の描画
    for(int i = 0; i < round; i++){
      SDL_RenderCopy(renderer,ITRa1[i],&src_item1,&dst_itemround_a1[i]);  // src = 元の画像のサイズとか(画像サイズはだいたい50x50なのであんまり変更しなくて良い)
      SDL_RenderCopy(renderer,ITRa2[i],&src_item1,&dst_itemround_a2[i]);
      SDL_RenderCopy(renderer,ITRa3[i],&src_item1,&dst_itemround_a3[i]);
      SDL_RenderCopy(renderer,ITRa4[i],&src_item1,&dst_itemround_a4[i]);
    }

    if(pg == 0){
      SDL_RenderCopy(renderer, texture2, &src_rect2, &dst_rect2); //罠ボックスの画像描画
    }

    if(pg == 3){
      round ++; // ラウンドを加算    
      printf("round == %d.\n", round);
      SDL_SetRenderDrawColor(renderer,255, 255, 255, 1);
      SDL_RenderFillRect(renderer, &result);
      char r[256];
      sprintf(r, "round%d result", round - 1);
      stringColor(renderer,400,580,r,0xff000000);

      stringColor(renderer, 890, 580, "WIN", 0xff000000);       
      stringColor(renderer,350,250,"PL1",0xff000000);
      stringColor(renderer,350,340,"PL2",0xff000000);
      stringColor(renderer,350,430,"PL3",0xff000000);
      stringColor(renderer,350,520,"PL4",0xff000000);

      boxColor(renderer,380,205,pl1,295,0xff00ff00);
      boxColor(renderer,380,295,pl2,385,0xff0000ff);
      boxColor(renderer,380,385,pl3,475,0xffff0000);
      boxColor(renderer,380,475,pl4,565,0xff00ffff);  
      lineColor(renderer,900,200,900,570,0xff000000);
        SDL_RenderPresent(renderer);
        SDL_Delay(5000);

      if(round == 8){
        if(win1 <= win2){
          win_result1 = win2;
        }
        else if(win2 <= win1){
          win_result1 = win1;
        }
        if(win3 <= win4){
          win_result2 = win4;
        }
        else if(win4 <= win3){
          win_result2 = win3;
        }
        if(win_result1 <= win_result2){
          win_result = win_result2;
        }
        else if(win_result2 <= win_result1){
          win_result = win_result1;
        }

        // 最終ラウンド終了処理
        if(win == win_result){
          SDL_RenderCopy(renderer, texture3, &src_rect3, &dst_rect3); // テクスチャをレンダラーにコピー（設定のサイズで）
        }
        else{
          SDL_RenderCopy(renderer, texture4, &src_rect4, &dst_rect4); // テクスチャをレンダラーにコピー（設定のサイズで）
        }
        SDL_RenderPresent(renderer);
        SDL_Delay(5000);
        Mix_FreeMusic(music);
        music=NULL; // 解放したことを明示するために
        Mix_CloseAudio();
        SDL_DestroyTexture(texture3);
        SDL_DestroyTexture(texture4);
        SDL_DestroyRenderer(renderer); // RCの破棄（解放）
        SDL_DestroyWindow(window); // 生成したウィンドウの破棄（消去）
        SDL_AtomicDecRef(&atm);
        Mix_Quit();
        SDL_Quit();
        return 0;
      }

      if(win1 == 5 || win2 == 5 || win3 == 5 || win4 == 5){
        if(win == 5){
          SDL_RenderCopy(renderer, texture3, &src_rect3, &dst_rect3); // テクスチャをレンダラーにコピー（設定のサイズで）
        }
        else{
          SDL_RenderCopy(renderer, texture4, &src_rect4, &dst_rect4); // テクスチャをレンダラーにコピー（設定のサイズで）
        }
        SDL_RenderPresent(renderer);
        SDL_Delay(5000);
        Mix_FreeMusic(music);
        music=NULL; // 解放したことを明示するために
        Mix_CloseAudio();
        SDL_DestroyTexture(texture3);
        SDL_DestroyTexture(texture4);
        SDL_DestroyRenderer(renderer); // RCの破棄（解放）
        SDL_DestroyWindow(window); // 生成したウィンドウの破棄（消去）
        SDL_AtomicDecRef(&atm);
        Mix_Quit();
        SDL_Quit();
        return 0;                 
      }

        chara.y = 505;
        chara.x = 100;
        CS.jump = 0;
        CS.fall = 0;
        pg = 0; // 罠選択画面へ
        IS.listup = true; //罠一覧を表示
        gc = 0;
        go = 1;
    }
  } // else ifの終了強制終了

  data1.cx = chara.x;
  data1.cy = chara.y;
  data1.command = NULL_COMMAND;

    if ( SDL_PollEvent(&event) && go ){
      switch (event.type){
      case SDL_KEYDOWN://キーが押された場合
        switch(event.key.keysym.sym){
  
        case SDLK_ESCAPE://Escキーが押された場合
            printf("Terminated.\n");
            Mix_FreeMusic(music);
            music=NULL; // 解放したことを明示するために
            Mix_CloseAudio();
            SDL_DestroyRenderer(renderer); // RCの破棄（解放）
            SDL_DestroyWindow(window); // 生成したウィンドウの破棄（消去）
            SDL_AtomicDecRef(&atm);
            Mix_Quit();
            SDL_Quit();
            return 0;
            break; 

        case SDLK_w: // wキーを押したとき
          if(pg == 0 || pg == 1){
            IS.up = true;
          } 
          break;

        case SDLK_d: // dが押された時右移動フ防ぐラグをtrueにする
         if(pg == 2 ){ // ゲーム中はキャラ移動
          CS.right = true;
         }
         else if(pg == 0 || pg == 1){ // 罠画面中は罠移動
          IS.right = true;
         }
        break;
      
        case SDLK_s: // sキーを押したとき
          if(pg == 0 || pg == 1){
            IS.down = true;
          }
          break;

        case SDLK_a: // aが押され強制終了た時左移動フラグをtrueにする
          if(pg == 2 ){
            CS.left = true;
          }
          else if(pg == 0 || pg ==1 ){
            IS.left = true;
          }
          break;
        
        case SDLK_RETURN: // エンターキーを押したとき
          if(fg == 0){
            data1.command = START_COMMAND; //全員がスタート押す同期
          }
          if(pg == 1){ //罠移動しているときにEnterで設置
            
            IS.up = false;
            IS.left = false;
            IS.down = false;
            IS.right = false;
            

            data1.command = START_COMMAND; //全員が罠を置いているか同期
          }

          else if(pg == 0){ 
            
            // 罠を選択した時
            

            for(int i = 0; i < 4; i++){ //dst_item_a[i] = 罠選択画面での罠の矩形
              if(cursor.x > dst_item_a[i].x && cursor.x < dst_item_a[i].x + dst_item_a[i].w){ // x座標の当たり判定
                if(cursor.y > dst_item_a[i].y && cursor.y < dst_item_a[i].y + dst_item_a[i].h){ // y座標の当たり判定
 
            data1.command = START_COMMAND;

            switch(data1.cid){
              case 0: 
                data1.wana[0] = i;
                break;
              case 1: 
                data1.wana[1] = i;
                break;
              case 2: 
                data1.wana[2] = i;
                break;
              case 3: 
                data1.wana[3] = i;
                break;
            }
                }
              }
            }

          }
          break;
        case SDLK_SPACE: // スペースが押された時ジャンプフラグをtrueにして、なんかキャラの座標をちょっといじる
          if(pg == 2){

            if(triple >= 3){
              CS.jump = false;
            }
            if(triple < 3){    
              CS.jump = true;
              CS.zimen = false;
              CS.atama = false;
              y_prev = chara.y;
              chara.y = chara.y - 20;
            }
            triple+=1;
          }
          break;  

        case SDLK_LSHIFT: // 左シフトが押された時ダッシュフラグをtrueにする
         if(pg == 2){
          CS.dash = true;
         }

          break;

        default: 
          break;

        } // KEYDOWNの終わり
        break;

      case SDL_KEYUP: //キーが離された場合
        switch(event.key.keysym.sym){

        case SDLK_LEFT:
          pg = 2;
          CS.exist = 1;
          break;

        case SDLK_w: // wを離したとき
          if(pg == 0 || pg == 1)
            IS.up = false;
          break;

        case SDLK_a:
          if(pg == 2)
            CS.left = false;
          else if(pg == 0 || pg == 1)
            IS.left = false;
          break;

        case SDLK_s: // sを離したとき
          if(pg == 0 || pg == 1)
            IS.down = false;
          break;

        case SDLK_d:
          if(pg == 2)
            CS.right = false;
          else if(pg == 0 || pg == 1)
            IS.right = false;
          break;

        case SDLK_LSHIFT: // 左シフトが離された時ダッシュフラグをfalseにする
          CS.dash = false;
          break;

        default:
          break;  

        } // KEYUPの終わり
        break;
       

      } //switch(event.type)の終わり

    } //SDL_PollEventの終わり強制終了

/*****************************************************************************
当たり判定
***************************************************************************/

    //キャラが壁からはみ出ないようにする
    // マップの左端
    if(chara.x<=0){
      chara.x = 0;
    } 
    // マップの右端
    if(chara.x>=1280){
      chara.x = 1280;
    } 

    // 左のビルの当たり判定
    if(chara.x >= building1.x - chara.w + 3 && chara.x <= building1.x + building1.w - 3){ // ビルの上面
        if(chara.y >= building1.y - chara.h){
            CS.zimen = true;
            CS.jump = false;
            chara.y = building1.y - chara.h;

        }
    }
    else{ // x座標がビル上でない時
        if((chara.y == building1.y - chara.h && CS.jump == false) && CS.fall == false){ // キャラのy座標がビルの高さでなおかつジャンプフラグ、自由落下フラグがfalseの時
            CS.fall = true; // 自由落下フラグをオンに
            y_prev = chara.y; // 自由落下の処理に関する式
        }
    }
  
    if(chara.y >= building1.y - chara.h + 1 && chara.y <= building1.y + building1.h){ // ビルの側面
        if(chara.x >= building1.x - chara.w + 1 &&  chara.x <= building1.x - chara.w + 2){
            chara.x = building1.x - chara.w;
        }

        else if(chara.x <= building1.x + building1.w - 1 && chara.x >= building1.x + building1.w - 2){
            chara.x = building1.x + building1.w;
        }
    }

    // 右のビルの当たり判定
    if(chara.x >= building2.x - chara.w + 3 && chara.x <= building2.x + building2.w - 3){ // ビルの上面
        if(chara.y >= building2.y - chara.h && chara.y <= building2.y - chara.h + building2.h){
          if(gc == 0){
            CS.zimen = true;
            CS.jump = false;
            CS.right = false;
            CS.left = false;
            go = 0;
            gc = 1;
            win++;
            data1.gc = gc;
            data1.win = win;
            data1.command = START_COMMAND;
            chara.y = building2.y - chara.h;

            //キャラの描画情報変更(電算室で再確認必要)
            
            goal[data1.cid]=1;
          }
        }
        else if( building2.y + building2.h - 20 <= chara.y &&chara.y <= building2.y  + building2.h){
          chara.y = building2.y + building2.h;
          printf(" 頭当たった.\n");
          CS.atama = true;
        }
    }
    else{ // x座標がビル上でない時
        if((chara.y == building2.y - chara.h && CS.jump == false) && CS.fall == false){ // キャラのy座標がビルの高さでなおかつジャンプフラグ、自由落下フラグがfalseの時
            CS.fall = true; // 自由落下フラグをオンに
            y_prev = chara.y; // 自由落下の処理に関する式
        }
    }
    
    if(chara.y >= building2.y - chara.h + 1 && chara.y <= building2.y + building2.h){ // ビルの側面
        if(chara.x >= building2.x - chara.w + 1 &&  chara.x <= building2.x - chara.w + 2){
            chara.x = building2.x - chara.w;
        }

        else if(chara.x <= building2.x + building2.w - 1 && chara.x >= building2.x + building2.w - 2){
            chara.x = building2.x + building2.w;
        }
    }
/*****************************************************
罠自体の当たり判定
******************************************************/
// iからroundまでの各roundのすべての罠の当たり判定
      if(pg == 0){
        for(int i = 0 ; i < round - 1 ; i++){
          dst_itemround_a1[i].x = MoveItemRects_a1[i].x; 
        }          
      }

for(int i = 0 ; i < round ; i++){ 

  if(timer %timernum == 0){


/**************************************
ドッスン
*************************************/
    if(pg == 2){

    //1Pのドッスン
    if(dosunr[i][0] == 1){
      if(chara.x >= dst_itemround_a1[i].x - chara.w + 3){
        if(chara.y >= dst_itemround_a1[i].y - chara.h && chara.y <= dst_itemround_a1[i].y - chara.h + dst_itemround_a1[i].h){
          //ここにドッスンが右に動く関数を書く
          dst_itemround_a1[i].x += 10;
          data1.ix = dst_itemround_a1[i].x;
          data1.command = IMOVE_COMMAND;
        }
      }
    }

    if(dosunl[i][0] == 1){
      if(chara.x <= dst_itemround_a1[i].x + dst_itemround_a1[i].w - 3){
        if(chara.y >= dst_itemround_a1[i].y - chara.h && chara.y <= dst_itemround_a1[i].y - chara.h + dst_itemround_a1[i].h){
          //ここにドッスンが左に動く関数を書く
          dst_itemround_a1[i].x -= 10;
          data1.ix = dst_itemround_a1[i].x;
          data1.command = IMOVE_COMMAND;
        }
      }
    }

    if(dosunr[i][1] == 1){
      if(chara.x >= dst_itemround_a2[i].x + dst_itemround_a2[i].w - 3){
        if(chara.y >= dst_itemround_a2[i].y - chara.h && chara.y <= dst_itemround_a2[i].y - chara.h + dst_itemround_a2[i].h){
          //ここにドッスンが右に動く関数を書く
          dst_itemround_a2[i].x += 10;
          data1.ix = dst_itemround_a2[i].x;
          data1.command = IMOVE_COMMAND;
        }
      }
    }

    if(dosunl[i][1] == 1){
      if(chara.x <= dst_itemround_a2[i].x - chara.w + 3){
        if(chara.y >= dst_itemround_a2[i].y - chara.h && chara.y <= dst_itemround_a2[i].y - chara.h + dst_itemround_a2[i].h){
          //ここにドッスンが左に動く関数を書く
          dst_itemround_a2[i].x -= 10;
          data1.ix = dst_itemround_a2[i].x;
          data1.command = IMOVE_COMMAND;
        }
      }
    }
  
  }
  }

/***************************************
 まるとげ
 **************************************/


    if(marutoge[i][0] == 1){
      printf("通った０");
      if(pg == 0){
        for(int i = 0 ; i < round - 1 ; i++){
          dst_itemround_a1[i].x = MoveItemRects_a1[i].x; 
        }          
      }

          if(pg == 2){
            if(timer %timernum == 0){
              if(dst_itemround_a1[i].x < MoveItemRects_a1[i].x - 100){
                marutogeflag = 0;
              }
              if(dst_itemround_a1[i].x > MoveItemRects_a1[i].x + 100){
                marutogeflag = 1;
              }
              switch(marutogeflag){
                case 0:
                  dst_itemround_a1[i].x += 1;
                  break;
                case 1:
                  dst_itemround_a1[i].x -= 1;
                  break;
              }

            }
          }
    

    if(marutoge[i][1] == 1){
      printf("通った１");
      if(pg == 0){
        for(int i = 0 ; i < round - 1 ; i++){
          dst_itemround_a2[i].x = MoveItemRects_a2[i].x; 
        }          
      }
          if(pg == 2){
            if(timer %timernum == 0){

              if(dst_itemround_a2[i].x < MoveItemRects_a2[i].x - 100){
                marutogeflag = 0;
              }
              if(dst_itemround_a2[i].x > MoveItemRects_a2[i].x + 100){
                marutogeflag = 1;
              }
              switch(marutogeflag){
                case 0:
                  dst_itemround_a2[i].x += 1;
                  break;
                case 1:
                  dst_itemround_a2[i].x -= 1;
                  break;
              }

            }
          }
    }

    if(marutoge[i][2] == 1){
      if(pg == 0){
        for(int i = 0 ; i < round - 1 ; i++){
          dst_itemround_a3[i].x = MoveItemRects_a3[i].x; 
        }          
      }
          if(pg == 2){
            if(timer %timernum == 0){
              if(dst_itemround_a3[i].x < MoveItemRects_a3[i].x - 100){
                marutogeflag = 0;
              }
              if(dst_itemround_a3[i].x > MoveItemRects_a3[i].x + 100){
                marutogeflag = 1;
              }
              switch(marutogeflag){
                case 0:
                  dst_itemround_a3[i].x += 1;
                  break;
                case 1:
                  dst_itemround_a3[i].x -= 1;
                  break;
              }

            }
          }
    }

    if(marutoge[i][3] == 1){
      if(pg == 0){
        for(int i = 0 ; i < round - 1 ; i++){
          dst_itemround_a4[i].x = MoveItemRects_a4[i].x; 
        }          
      }
          if(pg == 2){
            if(timer %timernum == 0){

              if(dst_itemround_a4[i].x < MoveItemRects_a4[i].x - 100){
                marutogeflag = 0;
              }
              if(dst_itemround_a4[i].x > MoveItemRects_a4[i].x + 100){
                marutogeflag = 1;
              }
              switch(marutogeflag){
                case 0:
                  dst_itemround_a4[i].x += 1;
                  break;
                case 1:
                  dst_itemround_a4[i].x -= 1;
                  break;
              }

            }
          }
    }
    }

/******************************
まるとげここまで
******************************/

    if(chara.x >= dst_itemround_a1[i].x - chara.w + 3 && chara.x <= dst_itemround_a1[i].x + dst_itemround_a1[i].w - 3){  // 罠の上面
        if(chara.y >= dst_itemround_a1[i].y - chara.h && chara.y <= dst_itemround_a1[i].y - chara.h + dst_itemround_a1[i].h){
            CS.zimen = true;
            triple = 0;
            CS.jump = false;
            chara.y = dst_itemround_a1[i].y - chara.h;
            if(migikonbea[i][0] == 1){
              chara.x += 1;
            }
            else if(hidarikonbea[i][0] == 1){
              chara.x -= 1;
            }
            else if(shine[i][0] == 1  && migikonbea[i][0] != 1 && hidarikonbea[i][0] != 1){
              
              data1.command = START_COMMAND;
              CS.right = false;
              CS.left = false;
              CS.fall = false;
              CS.jump = false;
              go = 0;
              data1.go = 1;
              death[data1.cid] = 1;
              printf("shine[%d]でやられたよ.command%c\n", i ,data1.command);
            }
        }
        else if( dst_itemround_a1[i].y + dst_itemround_a1[i].h - 20 <= chara.y && chara.y <= dst_itemround_a1[i].y  + dst_itemround_a1[i].h){ // 罠に頭をぶつけた時
          chara.y = dst_itemround_a1[i].y + dst_itemround_a1[i].h;
          CS.atama = true;
          if(shine[i][0] == 1 && migikonbea[i][0] != 1 && hidarikonbea[i][0] != 1){
            printf("si\n");
              data1.command = START_COMMAND;
              CS.right = false;
              CS.left = false;
              CS.fall = false;
              CS.jump = false;
              go = 0;
              data1.go = 1;
              death[data1.cid] = 1;
              printf("shine[%d]でやられたよ.command%c\n", i ,data1.command);
            }
        }
    }
    if(chara.x >= dst_itemround_a2[i].x - chara.w + 3 && chara.x <= dst_itemround_a2[i].x + dst_itemround_a2[i].w - 3){  // 罠の上面
        if(chara.y >= dst_itemround_a2[i].y - chara.h && chara.y <= dst_itemround_a2[i].y - chara.h + dst_itemround_a2[i].h){
            CS.zimen = true;
            triple = 0;
            CS.jump = false;
            chara.y = dst_itemround_a2[i].y - chara.h;
            if(migikonbea[i][j] == 1){
              chara.x += 1;
            }
            else if(hidarikonbea[i][1] == 1){
              chara.x -= 1;
            }
            else if(shine[i][1] == 1  && migikonbea[i][1] != 1 && hidarikonbea[i][1] != 1){
              data1.command = START_COMMAND;
              CS.right = false;
              CS.left = false;
              CS.fall = false;
              CS.jump = false;
              go = 0;
              data1.go = 1;
              death[data1.cid] = 1;
              printf("shine[%d]でやられたよ.command%c\n", i ,data1.command);
            }
        }
        else if( dst_itemround_a2[i].y + dst_itemround_a2[i].h - 20 <= chara.y && chara.y <= dst_itemround_a2[i].y  + dst_itemround_a2[i].h){ // 罠に頭をぶつけた時
          chara.y = dst_itemround_a2[i].y + dst_itemround_a2[i].h;
          CS.atama = true;
          if(shine[i][1] == 1  && migikonbea[i][1] != 1 && hidarikonbea[i][1] != 1){
              data1.command = START_COMMAND;
              CS.right = false;
              CS.left = false;
              CS.fall = false;
              CS.jump = false;
              go = 0;
              data1.go = 1;
              death[data1.cid] = 1;
              printf("shine[%d]でやられたよ.command%c\n", i ,data1.command);
            }
        }
    }
    if(chara.x >= dst_itemround_a3[i].x - chara.w + 3 && chara.x <= dst_itemround_a3[i].x + dst_itemround_a3[i].w - 3){  // 罠の上面
        if(chara.y >= dst_itemround_a3[i].y - chara.h && chara.y <= dst_itemround_a3[i].y - chara.h + dst_itemround_a3[i].h){
            CS.zimen = true;
            triple = 0;
            CS.jump = false;
            chara.y = dst_itemround_a3[i].y - chara.h;
            if(migikonbea[i][2] == 1){
              chara.x += 1;
            }
            else if(hidarikonbea[i][2] == 1){
              chara.x -= 1;
            }
            else if(shine[i][2] == 1  && migikonbea[i][2] != 1 && hidarikonbea[i][2] != 1){
              data1.command = START_COMMAND;
              CS.right = false;
              CS.left = false;
              CS.fall = false;
              CS.jump = false;
              go = 0;
              data1.go = 1;
              death[data1.cid] = 1;
              printf("shine[%d]でやられたよ.command%c\n", i ,data1.command);
            }
        }
        else if( dst_itemround_a3[i].y + dst_itemround_a3[i].h - 20 <= chara.y && chara.y <= dst_itemround_a3[i].y  + dst_itemround_a3[i].h){ // 罠に頭をぶつけた時
          chara.y = dst_itemround_a3[i].y + dst_itemround_a3[i].h;
          CS.atama = true;
          if(shine[i][2] == 1 && migikonbea[i][2] != 1 && hidarikonbea[i][2] != 1 ){
              data1.command = START_COMMAND;
              CS.right = false;
              CS.left = false;
              CS.fall = false;
              CS.jump = false;
              go = 0;
              data1.go = 1;
              death[data1.cid] = 1;
              printf("shine[%d]でやられたよ.command%c\n", i ,data1.command);
            }
        }
    }
    if(chara.x >= dst_itemround_a4[i].x - chara.w + 3 && chara.x <= dst_itemround_a4[i].x + dst_itemround_a4[i].w - 3){  // 罠の上面
        if(chara.y >= dst_itemround_a4[i].y - chara.h && chara.y <= dst_itemround_a4[i].y - chara.h + dst_itemround_a4[i].h){
            CS.zimen = true;
            triple = 0;
            CS.jump = false;
            chara.y = dst_itemround_a4[i].y - chara.h;
            if(migikonbea[i][3] == 1){
              chara.x += 1;
            }
            else if(hidarikonbea[i][3] == 1){
              chara.x -= 1;
            }
            else if(shine[i][3] == 1  && migikonbea[i][3] != 1 && hidarikonbea[i][3] != 1){
              data1.command = START_COMMAND;
              CS.right = false;
              CS.left = false;
              CS.fall = false;
              CS.jump = false;
              go = 0;
              data1.go = 1;
              death[data1.cid] = 1;
              printf("shine[%d]でやられたよ.command%c\n", i ,data1.command);
            }
        }
        else if( dst_itemround_a4[i].y + dst_itemround_a4[i].h - 20 <= chara.y && chara.y <= dst_itemround_a4[i].y  + dst_itemround_a4[i].h){ // 罠に頭をぶつけた時
          chara.y = dst_itemround_a4[i].y + dst_itemround_a4[i].h;
          CS.atama = true;
          if(shine[i][3] == 1 && migikonbea[i][3] != 1 && hidarikonbea[i][3] != 1){
              data1.command = START_COMMAND;
              CS.right = false;
              CS.left = false;
              CS.fall = false;
              CS.jump = false;
              go = 0;
              data1.go = 1;
              death[data1.cid] = 1;
              printf("shine[%d]でやられたよ%c\n", i ,data1.command);
            }
        }
    }
    else{ 
        if((chara.y == dst_itemround_a1[i].y - chara.h && CS.jump == false) && CS.fall == false){  // 罠から降りた時
            CS.fall = true; // 自由落下フラグをオンに
            y_prev = chara.y; // 自由落下の処理に関する式
        }
        if((chara.y == dst_itemround_a2[i].y - chara.h && CS.jump == false) && CS.fall == false){  // 罠から降りた時
            CS.fall = true; // 自由落下フラグをオンに
            y_prev = chara.y; // 自由落下の処理に関する式
        }
        if((chara.y == dst_itemround_a3[i].y - chara.h && CS.jump == false) && CS.fall == false){  // 罠から降りた時
            CS.fall = true; // 自由落下フラグをオンに
            y_prev = chara.y; // 自由落下の処理に関する式
        }
        if((chara.y == dst_itemround_a4[i].y - chara.h && CS.jump == false) && CS.fall == false){  // 罠から降りた時
            CS.fall = true; // 自由落下フラグをオンに
            y_prev = chara.y; // 自由落下の処理に関する式
        }
    }
    
    if(chara.y >= dst_itemround_a1[i].y - chara.h + 1 && chara.y <= dst_itemround_a1[i].y + dst_itemround_a1[i].h){  
        if(chara.x >= dst_itemround_a1[i].x - chara.w + 1 &&  chara.x <= dst_itemround_a1[i].x - chara.w + 2){
            chara.x = dst_itemround_a1[i].x - chara.w;
            if(shine[i][0] == 1 && migikonbea[i][0] != 1 && hidarikonbea[i][0] != 1){
              data1.command = START_COMMAND;
              CS.right = false;
              CS.left = false;
              CS.fall = false;
              CS.jump = false;
              go = 0;
              data1.go = 1;
              death[data1.cid] = 1;
              printf("shine[%d]でやられたよ%c\n", i ,data1.command);
            }
        }

        else if(chara.x <= dst_itemround_a1[i].x + dst_itemround_a1[i].w - 1 && chara.x >= dst_itemround_a1[i].x + dst_itemround_a1[i].w - 2){
            chara.x = dst_itemround_a1[i].x + dst_itemround_a1[i].w;
            if(shine[i][0] == 1 && migikonbea[i][0] != 1 && hidarikonbea[i][0] != 1){
              data1.command = START_COMMAND;
              CS.right = false;
              CS.left = false;
              CS.fall = false;
              CS.jump = false;
              go = 0;
              data1.go = 1;
              death[data1.cid] = 1;
              printf("shine[%d]でやられたよ.command%c\n", i ,data1.command);
            }
        }
    }
    if(chara.y >= dst_itemround_a2[i].y - chara.h + 1 && chara.y <= dst_itemround_a2[i].y + dst_itemround_a2[i].h){  
        if(chara.x >= dst_itemround_a2[i].x - chara.w + 1 &&  chara.x <= dst_itemround_a2[i].x - chara.w + 2){
            chara.x = dst_itemround_a2[i].x - chara.w;
            if(shine[i][1] == 1 && migikonbea[i][1] != 1 && hidarikonbea[i][1] != 1){
              data1.command = START_COMMAND;
              CS.right = false;
              CS.left = false;
              CS.fall = false;
              CS.jump = false;
              go = 0;
              data1.go = 1;
              death[data1.cid] = 1;
              printf("shine[%d]でやられたよ.command%c\n", i ,data1.command);
            }
        }

        else if(chara.x <= dst_itemround_a2[i].x + dst_itemround_a2[i].w - 1 && chara.x >= dst_itemround_a2[i].x + dst_itemround_a2[i].w - 2){
            chara.x = dst_itemround_a2[i].x + dst_itemround_a2[i].w;
            if(shine[i][1] == 1 && migikonbea[i][1] != 1 && hidarikonbea[i][1] != 1){
              data1.command = START_COMMAND;
              CS.right = false;
              CS.left = false;
              CS.fall = false;
              CS.jump = false;
              go = 0;
              data1.go = 1;
              death[data1.cid] = 1;
              printf("shine[%d]でやられたよ.command%c\n", i ,data1.command);
            }
        }
    } 
    if(chara.y >= dst_itemround_a3[i].y - chara.h + 1 && chara.y <= dst_itemround_a3[i].y + dst_itemround_a3[i].h){  
        if(chara.x >= dst_itemround_a3[i].x - chara.w + 1 &&  chara.x <= dst_itemround_a3[i].x - chara.w + 2){
            chara.x = dst_itemround_a3[i].x - chara.w;
            if(shine[i][2] == 1 && migikonbea[i][2] != 1 && hidarikonbea[i][2] != 1){
              data1.command = START_COMMAND;
              CS.right = false;
              CS.left = false;
              CS.fall = false;
              CS.jump = false;
              go = 0;
              data1.go = 1;
              death[data1.cid] = 1;
              printf("shine[%d]でやられたよ.command%c\n", i ,data1.command);
            }
        }

        else if(chara.x <= dst_itemround_a3[i].x + dst_itemround_a3[i].w - 1 && chara.x >= dst_itemround_a3[i].x + dst_itemround_a3[i].w - 2){
            chara.x = dst_itemround_a3[i].x + dst_itemround_a3[i].w;
            if(shine[i][2] == 1 && migikonbea[i][2] != 1 && hidarikonbea[i][2] != 1){
              data1.command = START_COMMAND;
              CS.right = false;
              CS.left = false;
              CS.fall = false;
              CS.jump = false;
              go = 0;
              data1.go = 1;
              death[data1.cid] = 1;
              printf("shine[%d]でやられたよ.command%c\n", i ,data1.command);
            }
        }
    }  
    if(chara.y >= dst_itemround_a4[i].y - chara.h + 1 && chara.y <= dst_itemround_a4[i].y + dst_itemround_a4[i].h){  
        if(chara.x >= dst_itemround_a4[i].x - chara.w + 1 &&  chara.x <= dst_itemround_a4[i].x - chara.w + 2){
            chara.x = dst_itemround_a4[i].x - chara.w;
            if(shine[i][3] == 1 && migikonbea[i][3] != 1 && hidarikonbea[i][3] != 1){
              data1.command = START_COMMAND;
              CS.right = false;
              CS.left = false;
              CS.fall = false;
              CS.jump = false;
              go = 0;
              data1.go = 1;
              death[data1.cid] = 1;
              printf("shine[%d]でやられたよ.command%c\n", i ,data1.command);
            }
        }

        else if(chara.x <= dst_itemround_a4[i].x + dst_itemround_a4[i].w - 1 && chara.x >= dst_itemround_a4[i].x + dst_itemround_a4[i].w - 2){
            chara.x = dst_itemround_a4[i].x + dst_itemround_a4[i].w;
            if(shine[i][3] == 1 && migikonbea[i][3] != 1 && hidarikonbea[i][3] != 1){
              data1.command = START_COMMAND;
              CS.right = false;
              CS.left = false;
              CS.fall = false;
              CS.jump = false;
              go = 0;
              data1.go = 1;
              death[data1.cid] = 1;
              printf("shine[%d]でやられたよ.command%c\n", i ,data1.command);
            }
        }
    }     


  // } 
}

/**************************************************************************************************************
当たり判定終わり
**************************************************************************************************************/


    // マップの下いったらゲーム不可（後に改善）
    if(chara.y>=800){
      data1.command = START_COMMAND;
      chara.y = 800;
      CS.right = false;
      CS.left = false;
      CS.fall = false;
      CS.jump = false;
      go = 0;
      data1.go = 1;
    } 

    // キャラのマップの当たり判定
    if((chara.x >= 30 && chara.x <= 250 && chara.y >= 505 && chara.y <= 515) || (chara.x >= 960 && chara.x <= 1270 && chara.y >= 225 && chara.y <= 235)){  
      MS.ground = true;
      triple = 0;
    }
    else {
      MS.ground = false;
    }

/**********************************************************************************
関数
**********************************************************************************/

/***************************************************************
キャラ移動の関数ここから
**************************************************************/

// キャラの移動関数
    if(CS.right == true && timer  %timernum  == 0){ // timerが２の倍数のとき移動する。早すぎる場合は10とかにするとよい
     printf("CS.right == true.\n");
      if(CS.dash == false){ // ダッシュフラグがtrueの時+4、falseの時+2
        chara.x += 4;
      }
      else{
        chara.x += 8;
      }
      data1.cx = chara.x;
      data1.command = CMOVE_COMMAND;
    }

// キャラの左移動
    if(CS.left == true && timer  %timernum  == 0){
    // printf("CS.left == true.\n");
      if(CS.dash == false){ // ダッシュフラグがtrueの時-4、falseの時-2
        chara.x-= 4;
      }
      else{
        chara.x -= 8;
      }
      data1.cx = chara.x;
      data1.command = CMOVE_COMMAND;
    }

// ジャンプの関数
    if(CS.jump == true && timer  %timernum == 0) {  // ジャンプフラグがオンの間、y座標を放物線の運動     ここにCS.fall = falseの条件を追加で空中ジャンプ不可になるはず
            // printf("CS.jump is True\n");
            CS.zimen = false; // 地面から離れた
            CS.fall = false; 
            y_temp = chara.y; // ジャンプの重力がどうこうみたいな式
	        chara.y += (chara.y - y_prev) +1;
	        y_prev = y_temp;
          data1.cy = chara.y;
          data1.command = CMOVE_COMMAND;
    }

// キャラの自由落下関数
    if(CS.fall == true && timer %timernum == 0 && go == 1) {  // ジャンプフラグがオンの間、y座標を放物線の運動
        // printf("CS.fall is True\n");
        CS.zimen = false; // 地面から離れた
        y_temp = chara.y; // 以下自由落下の式
	    chara.y += (chara.y - y_prev) +1;
	    y_prev = y_temp;
      data1.cy = chara.y;
      data1.command = CMOVE_COMMAND;
	}

// キャラが地面(オブジェクト)に接地している時
    if(CS.zimen == true){ 
        triple = 0;
        CS.fall = false; // 自由落下フラグをオフに
        CS.jump = false; // ジャンプフラグをオフに
        CS.atama = false;
    }
    else if(CS.zimen == false){ // 地面に接地していない時
        if(CS.jump == false){ // ジャンプ状態ではない時
            CS.fall = true; // 自由落下する
        }
    }

// キャラの頭がオブジェクトの下面にぶつかったとき
  if(CS.atama == true){
    CS.jump = false;
    CS.fall = true;
  }    

// 移動処理を意図的に遅くする関数 
    timer++;
    if(timer==50){ // 10 or 100
      timer = 0;
    }

/***************************************************************
キャラ移動の関数ここまで
**************************************************************/

/****************************************************************
罠、カーソルの移動関数ここから
*****************************************************************/

// 罠本体、カーソルの右移動関数
    if(IS.right == true && timer %timernum == 0){
      if(pg == 0 && timer %2 == 0){ //カーソル移動
        cursor.x += 4;
        data1.mx = cursor.x;
        data1.command = MMOVE_COMMAND; 
      }
      else if(pg == 1){ //罠移動
        dst_itemround_a[round -1].x += 4; // 罠の移動
        data1.ix = dst_itemround_a[round -1].x;
        data1.command = IMOVE_COMMAND;
      }
    }

// 罠の左移動関数
    if(IS.left == true && timer %timernum == 0){
      if(pg == 0 && timer %2 == 0){ //カーソル移動
        cursor.x -= 4;
        data1.mx = cursor.x;
        data1.command = MMOVE_COMMAND;
      }
      else if(pg == 1){ // 罠の移動
        dst_itemround_a[round -1].x -= 4;
        data1.ix = dst_itemround_a[round -1].x;
        data1.command = IMOVE_COMMAND;
      }
    }

// 罠の上移動関数
    if(IS.up == true && timer %timernum == 0){
      if(pg == 0 && timer %2 == 0){ //カーソル移動
        cursor.y -= 4;
        data1.my = cursor.y;
        data1.command = MMOVE_COMMAND;
      }
      else if(pg == 1){ //罠の移動
        dst_itemround_a[round -1].y -= 4;
        data1.iy = dst_itemround_a[round -1].y;
        data1.command = IMOVE_COMMAND;
      }
    }

// 罠の下移動関数
    if(IS.down == true && timer %timernum == 0){
      if(pg == 0 && timer %2 == 0){ //カーソルの移動
        cursor.y += 4;
        data1.my = cursor.y;
        data1.command = MMOVE_COMMAND;
      }
      else if(pg == 1){ //罠の移動
        dst_itemround_a[round -1].y += 4;
        data1.iy = dst_itemround_a[round -1].y;
        data1.command = IMOVE_COMMAND;
      }
    }

/************************************************
罠、カーソル移動の関数ここまで
**************************************************/
// markprintf("data1.command == %c.\n",data1.command);
send_data(&data1, sizeof(CONTAINER));
for(int i = 0; i < clients_count(); i++){
receive_data(&data2, sizeof(CONTAINER));

switch(data2.cid){
  case 0:
    data1.wana[0] = data2.wana[0];
    break;
  case 1: 
    data1.wana[1] = data2.wana[1];
    break;
  case 2: 
    data1.wana[2] = data2.wana[2];
    break;
  case 3: 
    data1.wana[3] = data2.wana[3];
    break;
}

for(int j = 0 ; j<4 ; j++){
  data1.itemnum[j] = data2.itemnum[j];

}
/************************************************************:
リザルト画面の計算
受け取ったクライアントのidがgc=1(クリアしていたとき)に矩形を伸ばす
***************************************************************/

         if(data2.cid == 0 && data2.gc == 1 && win1 <= 5){
          pl1 = 380 + 104 * data2.win;
          win1 = data2.win;

          }

         if(data2.cid == 1 && data2.gc == 1 && win2 <= 5){
         pl2 = 380 + 104 * data2.win;
         win2 = data2.win;
         }

        if(data2.cid == 2 && data2.gc == 1 && win3 <= 5){
        pl3 = 380 + 104 * data2.win;
        win3 = data2.win;
        }

        if(data2.cid == 3 && data2.gc == 1 && win4 <= 5){
        pl4 = 380 + 104 * data2.win;
        win4 = data2.win;
        }

/***********************
ここまで
************************/

    switch(data2.command){
      case CSTART_COMMAND:
      if(fg == 0){ //全員がスタート画面でEnterを押した
        fg = 1;
        pg = 0;

      }
      else if(fg == 1){ //ゲーム中

        if(pg == 0){ //全員が罠を選び終わった


          pg = 1;
          for(int i = 0; i < 4; i++){ //dst_item_a[i] = 罠選択画面での罠の矩形
              if(cursor.x > dst_item_a[i].x && cursor.x < dst_item_a[i].x + dst_item_a[i].w){ // x座標の当たり判定
                if(cursor.y > dst_item_a[i].y && cursor.y < dst_item_a[i].y + dst_item_a[i].h){ // y座標の当たり判定

                  //当たったら死ぬフラグを立てる
                  for(int j=0;j<2;j++){
                    printf("1pは%d番目の罠を選んだ\n", data2.wana[0] + 1);
                    printf("2pは%d番目の罠を選んだ\n", data2.wana[1] + 1);
                    printf("1pの選んだ罠は%d\n", data2.itemnum[data2.wana[0]]);
                    printf("2pの選んだ罠は%d\n", data2.itemnum[data2.wana[1]]);
                    switch(data2.itemnum[data2.wana[j]]){
                      case 1://migi
                        migikonbea[round - 1][j] = 1;
                        printf("右コンベアフラグon\n");
                        break;
                      case 2://hidari
                        hidarikonbea[round - 1][j] = 1;
                        printf("左コンベアフラグon\n");
                        break;
                      case 3: //ドッスン
                        shine[round - 1][j] = 1;
                        dosunl[round - 1][j] = 1;
                        printf("左ドッスンでやられるよ\n");
                        break;
                      case 4: //ドッスン
                        shine[round - 1][j] = 1;
                        dosunr[round - 1][j] = 1;
                        printf("右ドッスンでやられるよ\n");
                        break;
                      case 5: //ブラックホール
                        shine[round - 1][j] = 1;
                        printf("ブラックホールでやられる\n");
                        break;
                      case 6: //まるとげ
                        shine[round - 1][j] = 1;
                        marutoge[round - 1][j] = 1;
                        printf("まるとげでやられるよ。左右に動くよ\n");
                        break;             
                      
                    }
                  }


                  ITRa1[round -1] = ITLa[data2.wana[0]]; // 選択したリストをラウンドに保存する
                  dst_itemround_a1[round - 1] = dst_item_a[i]; // 座標を合わせる
                  ITRa2[round -1] = ITLa[data2.wana[1]]; // 選択したリストをラウンドに保存する
                  dst_itemround_a2[round - 1] = dst_item_a[i]; // 座標を合わせる
                }
              }
            }


        }

        else if(pg == 1){ //全員が罠を設置した
          pg = 2;
          chara1.x = data2.cx;
          chara1.y = data2.cy;
          chara2.x = data2.cx;
          chara2.y = data2.cy;
          chara3.x = data2.cx;
          chara3.y = data2.cy;
          chara4.x = data2.cx;
          chara4.y = data2.cy;
        }

        else if(pg == 2){ //全員ゲームを終了した
          printf("終わったよ\n");
          pg = 3;
          chara1.y = data2.cy;
          chara2.y = data2.cy;
          chara3.y = data2.cy;
          chara4.y = data2.cy;
        }
      }
    break;

    case END_COMMAND:
        cond = 0;
    break;

    case CMOVE_COMMAND: //キャラの移動
        switch(data2.cid){
          case 0:
             if(chara1.x>data2.cx){ //左を向く
             chara1x = 50;
             chara1y = 50;}
             else if(chara1.x<data2.cx){ //右を向く
             chara1x = 50;
             chara1y = 0;}
              chara1.x = data2.cx;
              chara1.y = data2.cy;
              break;
          case 1:
              if(chara2.x>data2.cx){
              chara2x = 50;
              chara2y = 50;}
              else if(chara2.x<data2.cx){
              chara2x = 50;
              chara2y = 0;}
              chara2.x = data2.cx;
              chara2.y = data2.cy;
              break;
          case 2:
              if(chara3.x>data2.cx){
                chara3x =50;
                chara3y = 50;}
              else if(chara3.x<data2.cx){
                chara3x = 50;
                chara3y = 0;}
              chara3.x = data2.cx;
              chara3.y = data2.cy;
              break;
          case 3:
             if(chara4.x>data2.cx){
               chara4x = 50;
               chara4y = 50;}
             else if(chara4.x<data2.cx){
               chara4x = 50;
               chara4y = 0;}
              chara4.x = data2.cx;
              chara4.y = data2.cy;
              break;
        }
    break;

    case MMOVE_COMMAND: //カーソルの移動
        switch(data2.cid){
          case 0:
              cursor1.x = data2.mx;
              cursor1.y = data2.my;
              break;
          case 1:
              cursor2.x = data2.mx;
              cursor2.y = data2.my;
              break;
          case 2:
              cursor3.x = data2.mx;
              cursor3.y = data2.my;
              break;
          case 3:
              cursor4.x = data2.mx;
              cursor4.y = data2.my;
              break;
        }
    break;    

    case IMOVE_COMMAND: //罠の移動
        switch(data2.cid){
          case 0:
              dst_itemround_a1[round-1].x = data2.ix;
              dst_itemround_a1[round-1].y = data2.iy;
              MoveItemRects_a1[round - 1].x = data2.ix;
              MoveItemRects_a1[round - 1].x = data2.ix;
              break;
          case 1:
              dst_itemround_a2[round-1].x = data2.ix;
              dst_itemround_a2[round-1].y = data2.iy;
              MoveItemRects_a2[round - 1].x = data2.ix;
              MoveItemRects_a2[round - 1].x = data2.ix;
              break;
          case 2:
              dst_itemround_a3[round-1].x = data2.ix;
              dst_itemround_a3[round-1].y = data2.iy;
              MoveItemRects_a3[round - 1].x = data2.ix;
              MoveItemRects_a3[round - 1].x = data2.ix;
              break;
          case 3:
              dst_itemround_a4[round-1].x = data2.ix;
              dst_itemround_a4[round-1].y = data2.iy;
              MoveItemRects_a4[round - 1].x = data2.ix;
              MoveItemRects_a4[round - 1].x = data2.ix;
              break;
        }

        if(pg==2){
          printf("pgは２です");
          if(dosunr[round-1][0] == 1 || dosunl[round-1][0] == 1){
            dst_itemround_a1[i].x = data2.ix;
          }
          if(dosunr[round-1][1] == 1 || dosunl[round-1][1] == 1){
            dst_itemround_a2[i].x = data2.ix;
          }
        }
    break;    

    case NULL_COMMAND:
      switch(data2.cid){
        case 0:
        if(death[0] == 1)
        chara1y = 150;
        else if(goal[0] == 1)
        chara1y = 100;
        else{
        chara1x=0;
        chara1y=0;
        }
         break;
        case 1:
        if(death[1] == 1)
        chara2y = 150;
        else if(goal[1] == 1)
        chara2y = 100;
        else{
          chara2x=0;
          chara2y=0;
      } 
          break;
        case 2:
        if(death[2] == 1)
        chara3y = 150;
        else if(goal[2]==1)
        chara3y = 100;
        else{
          chara3x=0;
          chara3y=0;
        }
          break;
        case 3:
        if(death[3] == 1)
        chara4y = 150;
        else if(goal[3] == 1)
        chara4y = 100;
        else{
          chara4x=0;
          chara4y=0;
        }
          break;
      }
    break;

    default: 
      fprintf(stderr, "exe_command(): %c is not a valid command.\n", data2.command);
      exit(1);


    }  
}

    /***********************************/
    /* 処理の記述ここまで               */
    /***********************************/

  // アイテム選択画面でアイテムを表示する
if(fg != 0){
  if(pg == 0){

    for(int i = 0; i < 4; i++){ // 罠選択画面で４つの罠を表示
      SDL_RenderCopy(renderer,ITLa[i],&src_item1,&dst_item_a[i]);  // src = 元の画像のサイズとか(画像サイズはだいたい50x50なのであんまり変更しなくて良い)
    }
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderFillRect(renderer, &cursor1);

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderFillRect(renderer, &cursor2);

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderFillRect(renderer, &cursor3);

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderFillRect(renderer, &cursor4);
  }

  /***********************************************************************************
  キャラの描画ここから
  ************************************************************************************/
  SDL_Rect src_chara1 = {chara1x,chara1y,50,50};
  SDL_Rect dst_chara1 = {chara1.x,chara1.y,50,50};
  SDL_RenderCopy(renderer,charatexture1,&src_chara1,&dst_chara1);
 
  SDL_Rect src_chara2 = {chara2x,chara2y,50,50};
  SDL_Rect dst_chara2 = {chara2.x,chara2.y,50,50};
  SDL_RenderCopy(renderer,charatexture2,&src_chara2,&dst_chara2);
  
  SDL_Rect src_chara3 = {chara3x,chara3y,50,50};
  SDL_Rect dst_chara3 = {chara3.x,chara3.y,50,50};
  SDL_RenderCopy(renderer,charatexture3,&src_chara3,&dst_chara3);

  SDL_Rect src_chara4 = {chara4x,chara4y,50,50};
  SDL_Rect dst_chara4 = {chara4.x,chara4.y,50,50};
  SDL_RenderCopy(renderer,charatexture4,&src_chara4,&dst_chara4);

}

  /**************************************************************************************
  キャラの描画ここまで
  ***************************************************************************************/

  SDL_RenderPresent(renderer);	// 描画データを表示
 
 } // whileの終わり
  
  // 終了処理
  Mix_FreeMusic(music);
  music=NULL; // 解放したことを明示するために
  Mix_CloseAudio();
  SDL_DestroyRenderer(renderer); // RCの破棄（解放）
  SDL_DestroyWindow(window); // 生成したウィンドウの破棄（消去）
  SDL_AtomicDecRef(&atm);
  Mix_Quit();
  SDL_Quit();
  return 0;
}
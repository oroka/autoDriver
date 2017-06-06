/*
 * 概要
 * 超音波センサとXBEEからの情報でモーターを制御する。
 * ハードウェア
 * - RX631 周波数 50MHz HOCO
 * - モーター
 * - 超音波センサ(PE3(36)前 - PE4(35)右)
 * - XBEE(APIモード利用）(PE2(37)RxD12 - PE1(38)TxD12)
 */
#include "iodefine.h"

void main(void);

//ルート情報
struct routeList{
	signed char posX;
	signed char posY;
	struct routeList* next;
};

//超音波センサの感知距離Xとして
//障害物あり・障害物なしでマップ配列を作成する。初期値0,障害物なし1,障害物あり2
//自身の位置情報を保持。置いた位置を原点（0,0)に設定
//モーターの回転速度と移動距離の関係を確認
//



void main(void){
	initCar();
	
	while(1){
		
	}
}

/*
 * test5.c
 * 前面の超音波センサによる走行制御
 * 前進まえに左右に首を振る（角度（回転時間）調整）
 * 最大160度を調査する（円形構造のユニットを想定（自作機体）左右90度位置に障害物があっても進行可能なため）
 */
#include "iodefine.h"
#include "global.h"

#define ROTATE_ANGLE_10_TIME 100//10度回転するために必要な時間
#define GOAHEAD_100_TIME 100//100mm進むために必要な時間

//グローバル変数はCファイルに書く（http://d.hatena.ne.jp/saitodevel01/20110321/1300685958）
unsigned char SCI12_RXI12_INTERRUPT;//SCI12割込みフラグ
unsigned char MANUAL_SPEED_FLAG;//速度調整自動・手動切り替えフラグ
unsigned char manual_motor_speed;//手動移動時速度変数

unsigned char US_SEARCH_COMPLETE = 0;//回転探索終了フラグ

/*
	PE3:Front - PE4:Front-Left(旧Left) - PC5:Front-Right - PC6:Back(旧Right)
*/
void main(void)
{
	unsigned char angle_count;
	initUltrasonicController();
	initCMT0();
	initMotorController();
	initSCI12();
	mcSetSpeed(10);
	
	while(1){
		//探索
		while(!US_SEARCH_COMPLETE){
			mcTurnRight();
			//最大8回(80度）の回転探索
			for(angle_count=0; angle_count<8; angle_count++){
				cmt0timer(ROTATE_ANGLE_10_TIME);//10度の時間に調整
				//センサ感知すれば左に90度回転して再探索
				if(PORTE.PIDR.BIT.B3 == 0){
					mcTurnLeft();
					cmt0timer(ROTATE_ANGLE_10_TIME * 9);
					break;
				}
			}
			if(angle_count==8){
				//回転調査前の位置まで戻る
				mcTurnLeft();
				cmt0timer(ROTATE_ANGLE_10_TIME * 8);//80度分
				//最大8回(80度）の回転探索
				for(angle_count=0; angle_count<8; angle_count++){
					cmt0timer(ROTATE_ANGLE_10_TIME);//10度の時間に調整
					//センサ感知すれば右に90度回転して再探索（この時点で右に障害物がないことは調査済み）
					if(PORTE.PIDR.BIT.B3 == 0){
						mcTurnRight();
						cmt0timer(ROTATE_ANGLE_10_TIME * 9);
						break;
					}
				}
			}
			//回転探索範囲でセンサ感知していないなら探索終了フラグを立てる
			if(angle_count==8) US_SEARCH_COMPLETE = 1;
		}
		//前進
		if(US_SEARCH_COMPLETE){
			//センサ感知可能距離分前進（200mm-300mm)
			mcGoStraight();
			cmt0timer(GOAHEAD_100_TIME*2);
			US_SEARCH_COMPLETE = 0;
		}
	}
}

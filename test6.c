/*
 * test6.c
 * test3.c派生（test3.cはXBeeのATモード利用)
 * XBeeのAPIモード利用
 */
#include "iodefine.h"
#include "global.h"

//グローバル変数はCファイルに書く（http://d.hatena.ne.jp/saitodevel01/20110321/1300685958）
unsigned char SCI12_RXI12_INTERRUPT;//SCI12割込みフラグ
unsigned char MANUAL_SPEED_FLAG;//速度調整自動・手動切り替えフラグ
unsigned char MANUAL_MOTOR_SPEED;//手動移動時速度変数

enum autor_control_mode{ AUTOR_AUTO=0, AUTOR_MANUAL=1, AUTOR_RETURN=2 };
unsigned char AUTOR_CONTROL_MODE = AUTOR_AUTO;

unsigned char XBEE_FRAME_START = 0;
unsigned char XBEE_FRAME_STRING[50];
unsigned char XBEE_FRAME_LENGTH = 51;
unsigned char XBEE_FRAME_COUNT = 0;
unsigned char XBEE_FRAME_TYPE = 0;

unsigned char XBEE_DB_DATA_NOW = 0;
unsigned char XBEE_DB_DATA_PREV = 0;
unsigned char XBEE_DB_DATA_GOAL = 0;

unsigned char roAddr[] = { 0x00, 0x13, 0xA2, 0x00, 0x40, 0xE7, 0x46, 0xBD };
unsigned char coAddr[] = { 0x00, 0x13, 0xA2, 0x00, 0x40, 0xD8, 0xD2, 0x68 };

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
		if(SCI12.SSR.BIT.ORER == 1 || SCI12.SSR.BIT.FER == 1 || SCI12.SSR.BIT.PER == 1){
			clearSCI12ErrorFlag();//フラグを解除し、RDRを読み捨てる
			enableSCI12();
		}
		
		if(SCI12_RXI12_INTERRUPT){
			unsigned char str = SCI12.RDR;
			if(str==0x7E){
				XBEE_FRAME_START = 1;
			}
			
			if(XBEE_FRAME_START){
				if(XBEE_FRAME_COUNT==2){
					XBEE_FRAME_LENGTH = str;
				}
				if(XBEE_FRAME_COUNT==3){
					XBEE_FRAME_TYPE = str;
				}
				XBEE_FRAME_STRING[XBEE_FRAME_COUNT++] = str;
				if(XBEE_FRAME_COUNT == XBEE_FRAME_LENGTH+4){
					XBEE_FRAME_START = 0;
				}
			}else{
				//XBEEAPI以外のデータ
				XBEE_FRAME_TYPE = 0;
			}
			
			SCI12_RXI12_INTERRUPT = 0;
		}
		
		//通信中以外に処理する工程
		if(!XBEE_FRAME_START){
			switch(AUTOR_CONTROL_MODE){
				case AUTOR_AUTO:
					break;
				case AUTOR_MANUAL:
					if(XBEE_FRAME_TYPE == 0x92){
						//IOデータによるリモコン操作
						//とりあえず、リモコンのパッド一つで操作
						unsigned short yAxis = XBEE_FRAME_STRING[21];
						unsigned short xAxis = XBEE_FRAME_STRING[23];
						yAxis = yAxis << 8 + XBEE_FRAME_STRING[22];
						xAxis = xAxis << 8 + XBEE_FRAME_STRING[24];
						if(yAxis < 100){
							mcGoToBack();
						}else if(yAxis > 155){
							mcGoStraight();
						}else if(xAxis < 100){
							mcTurnLeft();
						}else if(xAxis > 155){
							mcTurnRight();
						}
					}
					break;
				case AUTOR_RETURN:
					if(XBEE_FRAME_TYPE == 0x97){
						//電波強度XBEE_FRAME_STRING[18]
						//データに基づき行動する
						//縦方向距離計測
						XBEE_DB_DATA_NOW = XBEE_FRAME_STRING[18];
						if(XBEE_DB_DATA_PREV == 0){
							mcGoStraight();
							cmt0timer(500);
						}else{
							if(XBEE_DB_DATA_NOW < XBEE_DB_DATA_PREV - 10){
								mcGoStraight();
								cmt0timer(500);
							}else if(XBEE_DB_DATA_NOW > XBEE_DB_DATA_PREV - 10){
								mcGoToBack();
								cmt0timer(500);
							}else{
								if(!XBEE_DB_DATA_GOAL){
									mcTurnRight();//90度回転
									cmt0timer(500);
								}else{
									mcStop();
									AUTOR_CONTROL_MODE = AUTOR_MANUAL;
								}
							}
						}
						XBEE_DB_DATA_PREV = XBEE_DB_DATA_NOW;
						XBEE_FRAME_TYPE = 0;
					}else{
						xbee_get_db(roAddr);
					}
					break;
				default:
					break;
			}
		}
	}
}
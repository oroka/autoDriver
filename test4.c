/* 
 * test4.c
 * test3.cの走行プログラムをユーザーによる操作でも衝突は回避するように修正する
 * test3からの変更点
 * 超音波センサを後ろに付け替えるRight->Back
 * さらに左位置の超音波センサを左前に付け替える Left->Front-Left
 * パソコンからの操作（回転をQ:左回転、E:右回転に変更)
 * パソコンからの操作(Z:速度低下,C:速度上昇,L:自動走行時の速度変化の可否指定)を追加
 */
//#include "typedefine.h"
#include "iodefine.h"
#include "global.h"

//グローバル変数はCファイルに書く（http://d.hatena.ne.jp/saitodevel01/20110321/1300685958）
unsigned char SCI12_RXI12_INTERRUPT;
unsigned char MANUAL_SPEED_FLAG;
unsigned char manual_motor_speed;

/*
	PORTE.PDR.BIT.B3 = 0;//Front
	PORTE.PDR.BIT.B4 = 0;//Front-Left(旧Left)
	PORTC.PDR.BIT.B5 = 0;//Front-Right
	PORTC.PDR.BIT.B6 = 0;//Back(旧Right)
*/
void main(void)
{
	#if 1
	initUltrasonicController();
	initCMT0();
	initMotorController();
	initSCI12();
	mcSetSpeed(10);
	#endif
	
	#if 0
	initCar();
	#endif
	
	while(1){
		#if 1
		//シリアルエラーフラグ暫定対策
		if(SCI12.SSR.BIT.ORER == 1 || SCI12.SSR.BIT.FER == 1 || SCI12.SSR.BIT.PER == 1){
			clearSCI12ErrorFlag();//フラグを解除し、RDRを読み捨てる
			enableSCI12();
		}
		
		//シリアル割込みが入った場合(intprg.c内Excep_SCI12_RXI12）
		//パソコンからのコマンド（W:前進,S:後退,Q:左回転,E:右回転）で操作
		//(Z:速度低下,C:速度上昇)を追加
		//XBeeはATモード(透過)
		//超音波センサは全てHighの時
		if(SCI12_RXI12_INTERRUPT){
			unsigned char str = SCI12.RDR;
			switch(str){
				case 'W':
					if(PORTE.PIDR.BIT.B3 == 1) mcGoStraight();
					else mcStop();
					break;
				case 'S'://変更点：超音波センサ位置を後ろに付け替える : PC6
					if(PORTC.PIDR.BIT.B6 == 1) mcGoToBack();
					else mcStop();
					break;
				case 'Q':
					mcTurnLeft();
					break;
				case 'E':
					mcTurnRight();
					break;
				case 'Z':
					mcSpeedDown();
					break;
				case 'C':
					mcSpeedUp();
					break;
				case 'L':
					if(!MANUAL_SPEED_FLAG) MANUAL_SPEED_FLAG = 1;
					else MANUAL_SPEED_FLAG = 1;
					break;
				default:
					break;
			}
			SCI12_RXI12_INTERRUPT = 0;
		}
		//それ以外は簡易衝突回避プログラムにて自動走行
		else{
			if(PORTE.PIDR.BIT.B3 == 0){
				if(!MANUAL_SPEED_FLAG) mcSetSpeed(4);
				mcGoToBack();
				cmt0timer(500);
				mcTurnLeft();
				cmt0timer(500);
			}
			else if(PORTC.PIDR.BIT.B5 == 0){
				if(!MANUAL_SPEED_FLAG) mcSetSpeed(6);
				mcTurnLeft();
				cmt0timer(500);
			}else if(PORTC.PIDR.BIT.B6 == 0){//後ろ方向超音波センサ感知
				if(!MANUAL_SPEED_FLAG) mcSetSpeed(10);//最大速度に変更
				mcGoStraight();//前進
				cmt0timer(500);
			}else if(PORTE.PIDR.BIT.B4 == 0){
				if(!MANUAL_SPEED_FLAG) mcSetSpeed(6);
				mcTurnRight();
				cmt0timer(500);
			}
			else{
				if(!MANUAL_SPEED_FLAG) mcSetSpeed(8);
				mcGoStraight();
			}
		}
		#endif
		
		#if 0
		runCar();
		#endif
	}
}

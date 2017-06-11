/***********************************************************************/
/*                                                                     */
/*  FILE        :Main.c or Main.cpp                                    */
/*  DATE        :Tue, Oct 31, 2006                                     */
/*  DESCRIPTION :Main Program                                          */
/*  CPU TYPE    :                                                      */
/*                                                                     */
/*  NOTE:THIS IS A TYPICAL EXAMPLE.                                    */
/*                                                                     */
/***********************************************************************/
//#include "typedefine.h"
#include "iodefine.h"
#include "global.h"
#ifdef __cplusplus
//#include <ios>                        // Remove the comment when you use ios
//_SINT ios_base::Init::init_cnt;       // Remove the comment when you use ios
#endif

void main(void);
#ifdef __cplusplus
extern "C" {
void abort(void);
}
#endif

/*
	PORTE.PDR.BIT.B3 = 0;//Front
	PORTE.PDR.BIT.B4 = 0;//Left
	PORTC.PDR.BIT.B5 = 0;//Front-Right
	PORTC.PDR.BIT.B6 = 0;//Right
*/
void main(void)
{
	#if 1
	unsigned char ucount=0;
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
		//パソコンからのコマンド（W:前進,S:後退,A:左回転,D:右回転）で操作
		//XBeeはATモード(透過)
		if(SCI12_RXI12_INTERRUPT){
			unsigned char str = SCI12.RDR;
			switch(str){
				case 'W':
					mcGoStraight();
					break;
				case 'S':
					mcGoToBack();
					break;
				case 'A':
					mcTurnLeft();
					break;
				case 'D':
					mcTurnRight();
					break;
				default:
					break;
			}
			SCI12_RXI12_INTERRUPT = 0;
		}
		//それ以外は簡易衝突回避プログラムにて自動走行
		else{
			if(PORTE.PIDR.BIT.B3 == 0){
				mcSetSpeed(4);
				mcGoToBack();
				cmt0timer(500);
				mcTurnLeft();
				cmt0timer(500);
			}
			else if(PORTC.PIDR.BIT.B5 == 0){
				mcSetSpeed(6);
				mcTurnLeft();
				cmt0timer(500);
			}else if(PORTC.PIDR.BIT.B6 == 0){
				mcSetSpeed(4);
				mcTurnLeft();
				cmt0timer(500);
			}else if(PORTE.PIDR.BIT.B4 == 0){
				mcSetSpeed(4);
				mcTurnRight();
				cmt0timer(500);
			}
			else{
				mcSetSpeed(10);
				mcGoStraight();
			}
		}
		#endif
		
		#if 0
		runCar();
		#endif
	}
}

#ifdef __cplusplus
void abort(void)
{

}
#endif

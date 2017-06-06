/*
 * 概要
 * モーター制御
 * 電流方向による回転方向の制御とPWMによる回転数の制御
 * ポート設定（RX631 48pin）
 * 使用ポートPA6・PA4・PA3・PA1（回転方向）
 * 同上    PB3・PB5（回転数）
 * 動作未確認
 */

#include "iodefine.h"

#ifndef FREQUENCY
#define FREQUENCY 3000000//TGR(short型),PWM:64分周:46875count(1秒間)
#define MAXTCNT 469//10ms
#endif

#ifndef MOTORSPEEDMAX
#define MOTORSPEEDMAX 4
#endif

void initMotorController(void);
void runMotorController(void);
void mcSetSpeed(unsigned short);
void mcSpeedUp(void);
void mcSpeedDown(void);
void mcStop(void);
void mcGoStraight(void);
void mcGoToBack(void);
void mcTurnRight(void);
void mcTrunLeft(void);

//global value
unsigned char motorSpeed;//モーターの回転速度の段階数

void initMotorController(void){
	/* 使用ポート初期化 */
	//1:VCC(設定なし）
	//2:モーター１用出力A1
	//使用ポート	PA6(31)
	PORTA.PDR.BIT.B6 = 1;
	PORTA.PODR.BIT.B6 = 0;
	
	//3:モーター１用パルス出力
	//MTIOC0A(26)
	PORTB.PDR.BIT.B3 = 1;
	PORTB.PMR.BIT.B3 = 1;
	
	MPC.PWPR.BIT.B0WI = 0;
	MPC.PWPR.BIT.PFSWE = 1;
	MPC.PB3PFS.BIT.PSEL = 1;
	MPC.PWPR.BIT.PFSWE = 0;
	MPC.PWPR.BIT.B0WI = 1;
	
	SYSTEM.PRCR.WORD = 0xA503;
	SYSTEM.MSTPCRA.BIT.MSTPA9 = 0;
	SYSTEM.PRCR.WORD = 0xA500;
	
	ICU.IPR[142].BIT.IPR = 10;
	ICU.IR[142].BIT.IR = 0;
	ICU.IER[0x11].BIT.IEN6 = 1;
	ICU.IER[0x11].BIT.IEN7 = 1;
	
	MTU0.TCR.BYTE = 0x4B;//TGRBのコンペアマッチでTCNT=0, 立下りエッジでカウント PCLK/64
	MTU0.TMDR.BYTE = 0x02;//PWMモード１
	MTU0.TIORH.BYTE = 0x12;//A:初期low - コンペアマッチでhigh, B:初期low - コンペアマッチでlow
	
	MTU.TRWER.BIT.RWE = 0x1;//プロテクト解除
	//90%
	MTU0.TGRA = MAXTCNT;//コンペアマッチでHIGH(duty)
	MTU0.TGRB = MAXTCNT;//コンペアマッチでLOW(cycle)
	MTU.TRWER.BIT.RWE = 0x0;//プロテクト設定
	
	//MTU.TSTR.BIT.CST0 = 1;//MTU0-TCNTカウンタ開始
	
	//4:モーター１用出力A2
	//使用ポート	PA4(32)
	PORTA.PDR.BIT.B4 = 1;
	PORTA.PODR.BIT.B4 = 0;
	
	//5:モーター２用出力B1
	//使用ポート	PA3(33)
	PORTA.PDR.BIT.B3 = 1;
	PORTA.PODR.BIT.B3 = 0;
	
	//6:モーター２用パルス出力
	//MTIOC2A(25)
	PORTB.PDR.BIT.B5 = 1;
	PORTB.PMR.BIT.B5 = 1;
	
	MPC.PWPR.BIT.B0WI = 0;
	MPC.PWPR.BIT.PFSWE = 1;
	MPC.PB5PFS.BIT.PSEL = 1;
	MPC.PWPR.BIT.PFSWE = 0;
	MPC.PWPR.BIT.B0WI = 1;
	
	MTU2.TCR.BYTE = 0x4B;//TGRBのコンペアマッチでTCNT=0, 立下りエッジでカウント PCLK/64
	MTU2.TMDR.BYTE = 0x02;//PWMモード1
	MTU2.TIOR.BYTE = 0x12;//A:初期low - コンペアマッチでhigh, B:初期low - コンペアマッチでlow
	
	MTU.TRWER.BIT.RWE = 0x1;//プロテクト解除
	//90%
	MTU2.TGRA = MAXTCNT;//コンペアマッチでHIGH(duty)
	MTU2.TGRB = MAXTCNT;//コンペアマッチでLOW(cycle)
	MTU.TRWER.BIT.RWE = 0x0;//プロテクト設定
	
	ICU.IPR[150].BIT.IPR = 10;
	ICU.IR[150].BIT.IR = 0;
	ICU.IER[0x12].BIT.IEN6 = 1;
	ICU.IER[0x12].BIT.IEN7 = 1;
	
	//MTU.TSTR.BIT.CST0 = 1;//MTU0-TCNTカウンタ開始（TGRA,TGRB,TGRC,TGRDを同時に）
	
	//7:モーター２用出力B2
	//使用ポート	P47(CN2-56)
	PORT4.PDR.BIT.B7 = 1;
	PORT4.PODR.BIT.B7 = 0;
	//8:GND（設定なし）
}

//モーター制御更新
void runMotorController(void){
	//MTU.TSTR.BIT.CST0 = 1;//MTU0-TCNTカウンタ開始
	//MTU0.TGRA,MTU2.TGRAを変更で速度制御(最大はTGRBの設定値469)
}

//速度設定
void mcSetSpeed(unsigned short speed){
	if(speed < 1){
		mcStop();
		return;
	}
	//TGRA, TGRCを設定
	if(0 <= speed && speed <= MOTORSPEEDMAX){
		MTU.TSTR.BIT.CST0 = 0;
		MTU.TSTR.BIT.CST2 = 0;
		MTU.TRWER.BIT.RWE = 0x1;//プロテクト解除
		MTU0.TGRA = MTU2.TGRA = 0.1 * (MOTORSPEEDMAX - motorSpeed) * MAXTCNT + 1;
		MTU.TRWER.BIT.RWE = 0x0;//プロテクト設定
		MTU.TSTR.BIT.CST0 = 1;
		MTU.TSTR.BIT.CST2 = 1;
	}
}

//速度上昇
void mcSpeedUp(void){
	if(motorSpeed < MOTORSPEEDMAX){
		motorSpeed++;
		MTU.TSTR.BIT.CST0 = 0;
		MTU.TSTR.BIT.CST2 = 0;
		MTU.TRWER.BIT.RWE = 0x1;//プロテクト解除
		MTU0.TGRA = MTU2.TGRA = 0.1 * (MOTORSPEEDMAX - motorSpeed) * MAXTCNT + 1;
		MTU.TRWER.BIT.RWE = 0x0;//プロテクト設定
		MTU.TSTR.BIT.CST0 = 1;
		MTU.TSTR.BIT.CST2 = 1;
	}
}

//速度低下
void mcSpeedDown(void){
	if(0 < motorSpeed){
		motorSpeed--;
		MTU.TSTR.BIT.CST0 = 0;
		MTU.TSTR.BIT.CST2 = 0;
		if(motorSpeed > 1){
			MTU.TRWER.BIT.RWE = 0x1;
			MTU0.TGRA = MTU2.TGRA = (MOTORSPEEDMAX - motorSpeed) * MAXTCNT + 1;
			MTU.TRWER.BIT.RWE = 0x0;
			MTU.TSTR.BIT.CST0 = 1;
			MTU.TSTR.BIT.CST2 = 1;
		}else{
			mcStop();
		}
	}
}

void mcStop(void){
	MTU.TSTR.BIT.CST0 = 0;
	MTU.TSTR.BIT.CST2 = 0;
}

//順送
void mcGoStraight(void){
	//
	PORTA.PODR.BIT.B6 = 0;
	PORTA.PODR.BIT.B4 = 1;
	PORTA.PODR.BIT.B3 = 0;
	PORTA.PODR.BIT.B1 = 1;
}

//逆走
void mcGoToBack(void){
	PORTA.PODR.BIT.B6 = 1;
	PORTA.PODR.BIT.B4 = 0;
	PORTA.PODR.BIT.B3 = 1;
	PORTA.PODR.BIT.B1 = 0;
}

//右回転
void mcTurnRight(void){
	PORTA.PODR.BIT.B6 = 0;
	PORTA.PODR.BIT.B4 = 1;
	PORTA.PODR.BIT.B3 = 1;
	PORTA.PODR.BIT.B1 = 0;
}

//左回転
void mcTurnLeft(void){
	PORTA.PODR.BIT.B6 = 1;
	PORTA.PODR.BIT.B4 = 0;
	PORTA.PODR.BIT.B3 = 0;
	PORTA.PODR.BIT.B1 = 1;
}
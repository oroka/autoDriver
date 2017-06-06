﻿/*
 * 超音波センサ用設定ファイル
 * ポートを入力に設定する
 */
#include "global.h"
#include "iodefine.h"

void initUltrasonicController(void);
unsigned char runUltrasonicController(void);//センサー感知

void initUltrasonicController(){
	PORTE.PDR.BIT.B3 = 0;
	PORTE.PDR.BIT.B4 = 0;
}

unsigned char runUltrasonicController(){
	if(PORTE.PIDR.BIT.B3 == 0 && PORTE.PIDR.BIT.B4 == 0){
		return Ultra_All;
	}else if(PORTE.PIDR.BIT.B3 == 0){
		return Ultra_Right;
	}else if(PORTE.PIDR.BIT.B4 == 0){
		return Ultra_Front;
	}else{
		return Ultra_Nothing;
	}
}
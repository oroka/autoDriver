<<<<<<< HEAD
﻿#include "iodefine.h"

unsigned short cmt0_counter = 0;

void initCMT0(void){
	SYSTEM.PRCR.WORD = 0xA503;
	SYSTEM.MSTPCRA.BIT.MSTPA15 = 0;//CMT0,1
	SYSTEM.PRCR.WORD = 0xA500;
	
	//CMT0の動作停止
	CMT.CMSTR0.BIT.STR0 = 0;
	
	CMT0.CMCR.BIT.CMIE = 0x01;
	CMT0.CMCR.BIT.CKS = 0x00;//8分周・32分周・128分周・512分周
	
	CMT0.CMCNT = 0;
	
	//192MHz/2/8 = 12000000
	CMT0.CMCOR = 12 - 1;// 1usごとに割込み
	
	ICU.IPR[4].BYTE = 10;
	ICU.IER[3].BIT.IEN4 = 1;
}

void Excep_CMT0_CMI0(void){
	cmt0_counter++;
}

unsigned short getCMT0Counter(void){
	return cmt0_counter;
}

void clearCMT0Counter(void){
	cmt0_counter = 0;
}

void cmt0timer(unsigned short time){
	cmt0_counter = 0;
	CMT.CMSTR0.BIT.STR0 = 1;
	while(cmt0_counter < time) ;
	CMT.CMSTR0.BIT.STR0 = 0;
	return;
}

void startCMT0Ir(void){
	CMT.CMSTR0.BIT.STR0 = 1;
}

void stopCMT0Ir(void){
	CMT.CMSTR0.BIT.STR0 = 0;
=======
﻿#include "iodefine.h"

unsigned short cmt0_counter = 0;

void initCMT0(void){
	SYSTEM.PRCR.WORD = 0xA503;
	SYSTEM.MSTPCRA.BIT.MSTPA15 = 0;//CMT0,1
	SYSTEM.PRCR.WORD = 0xA500;
	
	//CMT0の動作停止
	CMT.CMSTR0.BIT.STR0 = 0;
	
	CMT0.CMCR.BIT.CMIE = 0x01;
	CMT0.CMCR.BIT.CKS = 0x00;//8分周・32分周・128分周・512分周
	
	CMT0.CMCNT = 0;
	
	//192MHz/2/8 = 12000000
	CMT0.CMCOR = 12 - 1;// 1usごとに割込み
	
	ICU.IPR[4].BYTE = 10;
	ICU.IER[3].BIT.IEN4 = 1;
}

void Excep_CMT0_CMI0(void){
	cmt0_counter++;
}

unsigned short getCMT0Counter(void){
	return cmt0_counter;
}

void clearCMT0Counter(void){
	cmt0_counter = 0;
}

void cmt0timer(unsigned short time){
	cmt0_counter = 0;
	CMT.CMSTR0.BIT.STR0 = 1;
	while(cmt0_counter < time) ;
	CMT.CMSTR0.BIT.STR0 = 0;
	return;
}

void startCMT0Ir(void){
	CMT.CMSTR0.BIT.STR0 = 1;
}

void stopCMT0Ir(void){
	CMT.CMSTR0.BIT.STR0 = 0;
>>>>>>> 89c1b6029a599b48540a8a0b4a27d5ddb0498dae
}
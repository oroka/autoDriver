#include "iodefine.h"
#include "vect.h"

unsigned short cmt0_counter = 0;

void initCMT0(void){
	SYSTEM.PRCR.WORD = 0xA503;
	SYSTEM.MSTPCRA.BIT.MSTPA15 = 0;//CMT0,1
	SYSTEM.PRCR.WORD = 0xA500;
	
	//CMT0‚Ì“®ì’â~
	CMT.CMSTR0.BIT.STR0 = 0;
	
	CMT0.CMCR.BIT.CMIE = 0x01;
	CMT0.CMCR.BIT.CKS = 0x03;//8•ªüE32•ªüE128•ªüE512•ªü
	
	CMT0.CMCNT = 0;
	
	//50MHz/2/512 = 48828
	CMT0.CMCOR = 49 - 1;// 1ms‚²‚Æ‚ÉŠ„‚İ
	
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
	while(cmt0_counter < time);
	CMT.CMSTR0.BIT.STR0 = 0;
	return;
}

void startCMT0Ir(void){
	CMT.CMSTR0.BIT.STR0 = 1;
}

void stopCMT0Ir(void){
	CMT.CMSTR0.BIT.STR0 = 0;
}

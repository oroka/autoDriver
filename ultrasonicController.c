/*
 * �����g�Z���T�p�ݒ�t�@�C��
 * �|�[�g����͂ɐݒ肷��
 */
#include "global.h"
#include "iodefine.h"

void initUltrasonicController(void);
unsigned char runUltrasonicController(void);//�Z���T�[���m

void initUltrasonicController(){
	PORTE.PDR.BIT.B3 = 0;//Front
	PORTE.PDR.BIT.B4 = 0;//Left
	PORTC.PDR.BIT.B5 = 0;//Front-Right
	PORTC.PDR.BIT.B6 = 0;//Right
}

unsigned char runUltrasonicController(){
	if(PORTE.PIDR.BIT.B3 == 0 && PORTE.PIDR.BIT.B4 == 0){
		return Ultra_All;
	}else if(PORTE.PIDR.BIT.B4 == 0){
		return Ultra_Right;
	}else if(PORTE.PIDR.BIT.B3 == 0){
		return Ultra_Front;
	}else{
		return Ultra_Nothing;
	}
}

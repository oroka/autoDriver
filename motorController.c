/*
 * �T�v
 * ���[�^�[����
 * �d�������ɂ���]�����̐����PWM�ɂ���]���̐���
 * �|�[�g�ݒ�iRX631 48pin�j
 * �g�p�|�[�gPA6�EPA4�EPA3�EPA1�i��]�����j
 * ����    PB3�EPB5�i��]���j
 * ���얢�m�F
 */

#include "iodefine.h"

#ifndef FREQUENCY
#define FREQUENCY 3000000//TGR(short�^),PWM:64����:46875count(1�b��)
#define MAXTCNT 469//10ms
#endif

#ifndef MOTORSPEEDMAX
#define MOTORSPEEDMAX 10
#endif

void initMotorController(void);
void runMotorController(void);
unsigned char mcSetSpeed(unsigned char);
unsigned char mcSpeedUp(void);
unsigned char mcSpeedDown(void);
void mcStop(void);
void mcGoStraight(void);
void mcGoToBack(void);
void mcTurnRight(void);
void mcTrunLeft(void);

//global value
unsigned char motorSpeed;//���[�^�[�̉�]���x�̒i�K��

void initMotorController(void){
	/* �g�p�|�[�g������ */
	//1:VCC(�ݒ�Ȃ��j
	//2:���[�^�[�P�p�o��A1
	//�g�p�|�[�g	PA6(31)
	PORTA.PDR.BIT.B6 = 1;
	PORTA.PODR.BIT.B6 = 0;
	
	//3:���[�^�[�P�p�p���X�o��
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
	
	MTU0.TCR.BYTE = 0x4B;//TGRB�̃R���y�A�}�b�`��TCNT=0, ������G�b�W�ŃJ�E���g PCLK/64
	MTU0.TMDR.BYTE = 0x02;//PWM���[�h�P
	MTU0.TIORH.BYTE = 0x12;//A:����low - �R���y�A�}�b�`��high, B:����low - �R���y�A�}�b�`��low
	
	MTU.TRWER.BIT.RWE = 0x1;//�v���e�N�g����
	//90%
	MTU0.TGRA = MAXTCNT;//�R���y�A�}�b�`��HIGH(duty)
	MTU0.TGRB = MAXTCNT;//�R���y�A�}�b�`��LOW(cycle)
	MTU.TRWER.BIT.RWE = 0x0;//�v���e�N�g�ݒ�
	
	//MTU.TSTR.BIT.CST0 = 1;//MTU0-TCNT�J�E���^�J�n
	
	//4:���[�^�[�P�p�o��A2
	//�g�p�|�[�g	PA4(32)
	PORTA.PDR.BIT.B4 = 1;
	PORTA.PODR.BIT.B4 = 0;
	
	//5:���[�^�[�Q�p�o��B1
	//�g�p�|�[�g	PA3(33)
	PORTA.PDR.BIT.B3 = 1;
	PORTA.PODR.BIT.B3 = 0;
	
	//6:���[�^�[�Q�p�p���X�o��
	//MTIOC2A(25)
	PORTB.PDR.BIT.B5 = 1;
	PORTB.PMR.BIT.B5 = 1;
	
	MPC.PWPR.BIT.B0WI = 0;
	MPC.PWPR.BIT.PFSWE = 1;
	MPC.PB5PFS.BIT.PSEL = 1;
	MPC.PWPR.BIT.PFSWE = 0;
	MPC.PWPR.BIT.B0WI = 1;
	
	MTU2.TCR.BYTE = 0x4B;//TGRB�̃R���y�A�}�b�`��TCNT=0, ������G�b�W�ŃJ�E���g PCLK/64
	MTU2.TMDR.BYTE = 0x02;//PWM���[�h1
	MTU2.TIOR.BYTE = 0x12;//A:����low - �R���y�A�}�b�`��high, B:����low - �R���y�A�}�b�`��low
	
	MTU.TRWER.BIT.RWE = 0x1;//�v���e�N�g����
	//90%
	MTU2.TGRA = MAXTCNT;//�R���y�A�}�b�`��HIGH(duty)
	MTU2.TGRB = MAXTCNT;//�R���y�A�}�b�`��LOW(cycle)
	MTU.TRWER.BIT.RWE = 0x0;//�v���e�N�g�ݒ�
	
	ICU.IPR[150].BIT.IPR = 10;
	ICU.IR[150].BIT.IR = 0;
	ICU.IER[0x12].BIT.IEN6 = 1;
	ICU.IER[0x12].BIT.IEN7 = 1;
	
	//MTU.TSTR.BIT.CST0 = 1;//MTU0-TCNT�J�E���^�J�n�iTGRA,TGRB,TGRC,TGRD�𓯎��Ɂj
	
	//7:���[�^�[�Q�p�o��B2
	//�g�p�|�[�g	PA1(34)
	PORTA.PDR.BIT.B1 = 1;
	PORTA.PODR.BIT.B1 = 0;
	//8:GND�i�ݒ�Ȃ��j
	
	mcSetSpeed(10);
}

//
void runMotorController(void){
	//MTU.TSTR.BIT.CST0 = 1;//MTU0-TCNT�J�E���^�J�n
	//MTU0.TGRA,MTU2.TGRA��ύX�ő��x����(�ő��TGRB�̐ݒ�l469)
}

//���x�ݒ�
unsigned char mcSetSpeed(unsigned char speed){
	if(speed < 2){
		mcStop();
		return 0;
	}
	//TGRA, TGRC��ݒ�
	if(0.0 <= speed && speed <= MOTORSPEEDMAX){
		motorSpeed = speed;
		MTU.TSTR.BIT.CST0 = 0;
		MTU.TSTR.BIT.CST2 = 0;
		MTU.TRWER.BIT.RWE = 0x1;//�v���e�N�g����
		MTU0.TGRA = MTU2.TGRA = 0.1 * (MOTORSPEEDMAX - motorSpeed) * MAXTCNT + 1;
		MTU.TRWER.BIT.RWE = 0x0;//�v���e�N�g�ݒ�
		MTU.TSTR.BIT.CST0 = 1;
		MTU.TSTR.BIT.CST2 = 1;
	}
	
	return motorSpeed;
}

//���x�㏸
unsigned char mcSpeedUp(void){
	if(motorSpeed < MOTORSPEEDMAX){
		motorSpeed++;
		MTU.TSTR.BIT.CST0 = 0;
		MTU.TSTR.BIT.CST2 = 0;
		MTU.TRWER.BIT.RWE = 0x1;//�v���e�N�g����
		MTU0.TGRA = MTU2.TGRA = 0.1 * (MOTORSPEEDMAX - motorSpeed) * MAXTCNT + 1;
		MTU.TRWER.BIT.RWE = 0x0;//�v���e�N�g�ݒ�
		MTU.TSTR.BIT.CST0 = 1;
		MTU.TSTR.BIT.CST2 = 1;
	}
	return motorSpeed;
}

//���x�ቺ
unsigned char mcSpeedDown(void){
	if(0 < motorSpeed){
		motorSpeed--;
		MTU.TSTR.BIT.CST0 = 0;
		MTU.TSTR.BIT.CST2 = 0;
		if(motorSpeed > 2){
			MTU.TRWER.BIT.RWE = 0x1;
			MTU0.TGRA = MTU2.TGRA = 0.1 * (MOTORSPEEDMAX - motorSpeed) * MAXTCNT + 1;
			MTU.TRWER.BIT.RWE = 0x0;
			MTU.TSTR.BIT.CST0 = 1;
			MTU.TSTR.BIT.CST2 = 1;
		}else{
			mcStop();
		}
	}
	return motorSpeed;
}

void mcStop(void){
	MTU.TSTR.BIT.CST0 = 0;
	MTU.TSTR.BIT.CST2 = 0;
}

//����
void mcGoStraight(void){
	//
	PORTA.PODR.BIT.B6 = 1;
	PORTA.PODR.BIT.B4 = 0;
	PORTA.PODR.BIT.B3 = 1;
	PORTA.PODR.BIT.B1 = 0;
}

//�t��
void mcGoToBack(void){
	PORTA.PODR.BIT.B6 = 0;
	PORTA.PODR.BIT.B4 = 1;
	PORTA.PODR.BIT.B3 = 0;
	PORTA.PODR.BIT.B1 = 1;
}

//�E��]
void mcTurnRight(void){
	PORTA.PODR.BIT.B6 = 0;
	PORTA.PODR.BIT.B4 = 1;
	PORTA.PODR.BIT.B3 = 1;
	PORTA.PODR.BIT.B1 = 0;
}

//����]
void mcTurnLeft(void){
	PORTA.PODR.BIT.B6 = 1;
	PORTA.PODR.BIT.B4 = 0;
	PORTA.PODR.BIT.B3 = 0;
	PORTA.PODR.BIT.B1 = 1;
}
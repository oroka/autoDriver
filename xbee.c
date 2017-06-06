#include "iodefine.h"

#define XBEE_SCI SCI12

int initSCI12(void);
int xbee_send_type17(unsigned char* addr, unsigned char* command, unsigned char param);
int xbee_send_type10(unsigned char* addr, unsigned char* data);
int xbee_checksum(unsigned char* data, unsigned short length);

int initSCI12(void){
	SYSTEM.PRCR.WORD = 0xA502;
	SYSTEM.MSTPCRB.BIT.MSTPB4 = 0;
	SYSTEM.PRCR.WORD = 0xA500;
	
	SCI12.SCR.BIT.TIE = 0;
	SCI12.SCR.BIT.RIE = 0;
	SCI12.SCR.BIT.TE = 0;
	SCI12.SCR.BIT.RE = 0;
	SCI12.SCR.BIT.TEIE = 0;
	
	PORTE.PMR.BIT.B2 = 1;//RXD12
	PORTE.PMR.BIT.B1 = 1;//TXD12
	
	PORTE.PDR.BIT.B2 = 0;//RXD12
	PORTE.PDR.BIT.B1 = 1;//TXD12
	
	MPC.PWPR.BIT.B0WI = 0;
	MPC.PWPR.BIT.PFSWE = 1;
	
	MPC.PE1PFS.BYTE = 0x0C;//TXD12
	MPC.PE2PFS.BYTE = 0x0C;//RXD12
	
	MPC.PWPR.BIT.PFSWE = 0;
	MPC.PWPR.BIT.B0WI = 1;
	
	SCI12.SCR.BIT.CKE = 0;//�����{�[���[�g�g�p
	
	SCI12.SIMR1.BIT.IICM = 0;
	SCI12.SPMR.BIT.CKPH = 0;
	SCI12.SPMR.BIT.CKPOL = 0;
	
	//SMR,SCMR,SEMR���W�X�^�ɑ��M�E��M�t�H�[�}�b�g��ݒ�
	SCI12.SMR.BIT.CKS = 0x01;//�N���b�N������PCLK/4
	SCI12.SMR.BIT.MP = 0;//�}���`�v���Z�b�T�ʐM�@�\�֎~�O�A���P
	SCI12.SMR.BIT.STOP = 0;//1�X�g�b�v�r�b�g
	SCI12.SMR.BIT.PM = 0;//�����p���e�B�ő���M
	SCI12.SMR.BIT.PE = 0;//�p���e�B�r�b�g�Ȃ�
	SCI12.SMR.BIT.CHR = 0;//�f�[�^��8�r�b�g�ő���M�O�A7�r�b�g�P
	SCI12.SMR.BIT.CM = 0;//�������������[�h�œ���0,�N���b�N�������[�h�P
	
	SCI12.SCMR.BIT.SMIF = 0;//�V���A���R�~���j�P�[�V�����C���^�t�F�[�X���[�h�O
	SCI12.SCMR.BIT.SINV = 0;//TDR���W�X�^�̓��e�����̂܂ܑ��M�A��M�f�[�^�����̂܂�RDR���W�X�^�Ɋi�[0,���]�P
	SCI12.SCMR.BIT.SDIR = 0;//LSB�t�@�[�X�g�ő���M
	
	SCI12.SEMR.BIT.ACS0 = 0;//�O���N���b�N����0,TMR�N���b�N����1
	SCI12.SEMR.BIT.ABCS = 0;//16�T�C�N��1�r�b�g�̓]�����[�g�A8�T�C�N��1�r�b�g�̓]�����[�g
	SCI12.SEMR.BIT.NFEN = 0;//RXD���͐M���m�C�Y�����@�\����0,�L��1
	
	SCI12.BRR = 38;//48mhz/64/2/9600-1
	
	SCI12.SCR.BIT.TEIE = 0;
	SCI12.SCR.BIT.MPIE = 0;
	SCI12.SCR.BIT.RE = 1;
	SCI12.SCR.BIT.TE = 1;
	SCI12.SCR.BIT.RIE = 1;
	SCI12.SCR.BIT.TIE = 0;
	
	ICU.IPR[250].BIT.IPR = 10;
	ICU.IR[250].BIT.IR = 0;
	ICU.IER[0x1F].BIT.IEN2 = 1;
	
	return 0;
}

/* �ڑ���p�����[�^�擾 */
int xbee_get_param(unsigned char* addr){
	return xbee_send_type17(addr, "IS", 2);
}
/* �ڑ���o�͐ݒ� */
//ON
int xbee_output_high(unsigned char* addr, unsigned char* port){
	return xbee_send_type17(addr, port, 1);
}
//OFF
int xbee_output_low(unsigned char* addr, unsigned char* port){
	return xbee_send_type17(addr, port, 0);
}
/* �t���[���^�C�v�P�V���M�p�֐� */
int xbee_send_type17(unsigned char* addr, unsigned char* command, unsigned char param){
	unsigned char uc = 0;
	unsigned char lnum = 0;
	unsigned char comp_data[256];
	comp_data[0] = 0x7E;
	comp_data[1] = 0x00;
	comp_data[2] = 0x10;
	comp_data[3] = 0x17;
	comp_data[4] = 0x01;
	for(uc=0; uc<8; uc++){
		comp_data[5+uc] = addr[uc];
	}
	comp_data[13] = 0xFF;
	comp_data[14] = 0xFE;
	comp_data[15] = 0x02;
	comp_data[16] = command[0];
	comp_data[17] = command[1];
	if(param==0){
		comp_data[18] = 0x04;
		lnum = 19;
	}else if(param==1){
		comp_data[18] = 0x05;
		lnum = 19;
	}else{
		lnum = 18;
	}
	comp_data[lnum] = xbee_checksum(comp_data, 16);
	
	for(uc=0; uc<=lnum; uc++){
		while(XBEE_SCI.SSR.BIT.TEND == 0);//�Ƃ肠����SCI12�i#define�Œu�������ĕύX���₷������)
		XBEE_SCI.TDR = comp_data[uc];
	}
	
	return 0;
}

//�f�[�^���M�p�֐�
int xbee_send_data(unsigned char* addr, unsigned char* data){
	return xbee_send_type10(addr, data);
}

//�t���[���^�C�v�P�O���M�p�֐�
int xbee_send_type10(unsigned char* addr, unsigned char* data){
	unsigned char uc = 0;
	unsigned char comp_data[256];
	unsigned short data_length = xbee_strlen(data);
	comp_data[0] = 0x7E;
	comp_data[1] = (data_length+18-4)>>8;
	comp_data[2] = data_length+18-4;
	comp_data[3] = 0x10;
	comp_data[4] = 0x01;
	for(uc=0; uc<8; uc++){
		comp_data[5+uc] = addr[uc];
	}
	comp_data[13] = 0xFF;
	comp_data[14] = 0xFE;
	comp_data[15] = 0x00;
	comp_data[16] = 0x00;
	for(uc=0; uc<data_length; uc++){
		comp_data[17+uc] = data[uc];
	}
	comp_data[17+data_length] = xbee_checksum(comp_data, 17 + data_length);
	
	for(uc=0; uc<=17+data_length; uc++){
		while(XBEE_SCI.SSR.BIT.TEND == 0);
		XBEE_SCI.TDR = comp_data[uc];
	}
	//�ԓ��p�P�b�g�̓t���[���^�C�v8B
	
	return 0;
}

/* ������̒�����Ԃ� */
int xbee_strlen(unsigned char* chrs){
	unsigned short length = 0;
	while(*chrs++ != '\0'){
		length++;
	}
	return length;
}

/* �`�F�b�N�T������ */
int xbee_checksum(unsigned char* data, unsigned short length){
	unsigned char checksum = 0;
	unsigned char uc;
	
	for(uc=3; uc<length; uc++){
		checksum += data[uc];
	}
	checksum &= 0xFF;
	checksum = 0xFF - checksum;
	
	return checksum;
}
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
	
	SCI12.SCR.BIT.CKE = 0;
	
	SCI12.SIMR1.BIT.IICM = 0;
	SCI12.SPMR.BIT.CKPH = 0;
	SCI12.SPMR.BIT.CKPOL = 0;
	
	SCI12.SMR.BIT.CKS = 0x01;
	SCI12.SMR.BIT.MP = 0;
	SCI12.SMR.BIT.STOP = 0;
	SCI12.SMR.BIT.PM = 0;
	SCI12.SMR.BIT.PE = 0;
	SCI12.SMR.BIT.CHR = 0;
	SCI12.SMR.BIT.CM = 0;
	
	SCI12.SCMR.BIT.SMIF = 0;
	SCI12.SCMR.BIT.SINV = 0;
	SCI12.SCMR.BIT.SDIR = 0;
	
	SCI12.SEMR.BIT.ACS0 = 0;
	SCI12.SEMR.BIT.ABCS = 0;
	SCI12.SEMR.BIT.NFEN = 0;
	
	SCI12.BRR = 20 -1;//38;//48mhz/64/2/9600-1
	
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

void clearSCI12ErrorFlag(void){
	char errorValue;
	SCI12.SCR.BIT.TE = 0;
	while(SCI12.SCR.BIT.TE == 1) ;
	errorValue = SCI12.SSR.BIT.ORER;
	SCI12.SSR.BIT.ORER = 0;
	while(SCI12.SSR.BIT.ORER == 1) ;
	errorValue = SCI12.SSR.BIT.FER;
	SCI12.SSR.BIT.FER = 0;
	while(SCI12.SSR.BIT.FER == 1) ;
	errorValue = SCI12.SSR.BIT.PER;
	SCI12.SSR.BIT.PER = 0;
	while(SCI12.SSR.BIT.PER == 1);
	
	errorValue = SCI12.RDR;
}

void enableSCI12(void){
	SCI12.SCR.BIT.TEIE = 0;
	SCI12.SCR.BIT.MPIE = 0;
	SCI12.SCR.BIT.RE = 1;
	SCI12.SCR.BIT.TE = 1;
	SCI12.SCR.BIT.RIE = 1;
	SCI12.SCR.BIT.TIE = 0;
}

int xbee_get_db(unsigned char* addr){
	return xbee_send_type17(addr, "DB", 2);
}

int xbee_get_param(unsigned char* addr){
	return xbee_send_type17(addr, "IS", 2);
}
//ON
int xbee_output_high(unsigned char* addr, unsigned char* port){
	return xbee_send_type17(addr, port, 1);
}
//OFF
int xbee_output_low(unsigned char* addr, unsigned char* port){
	return xbee_send_type17(addr, port, 0);
}
int xbee_send_type17(unsigned char* addr, unsigned char* command, unsigned char param){
	unsigned char uc = 0;
	unsigned char lnum = 0;
	unsigned char comp_data[256];
	comp_data[0] = 0x7E;
	comp_data[1] = 0x00;
	comp_data[2] = 0x0F;
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
	comp_data[lnum] = xbee_checksum(comp_data, 18);
	
	for(uc=0; uc<=lnum; uc++){
		while(XBEE_SCI.SSR.BIT.TEND == 0);
		XBEE_SCI.TDR = comp_data[uc];
	}
	
	return 0;
}

int xbee_send_data(unsigned char* addr, unsigned char* data){
	return xbee_send_type10(addr, data);
}

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
	
	return 0;
}

int xbee_strlen(unsigned char* chrs){
	unsigned short length = 0;
	while(*chrs++ != '\0'){
		length++;
	}
	return length;
}

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

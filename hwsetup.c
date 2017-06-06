/************************************************************************
*
* Device     : RX/RX600/RX63N,RX631
*
* File Name  : hwsetup.c
*
* Abstract   : Hardware Setup file.
*
* History    : 0.10  (2011-02-21)  [Hardware Manual Revision : 0.01]
*
* NOTE       : THIS IS A TYPICAL EXAMPLE.
*
* Copyright (C) 2011 Renesas Electronics Corporation.
* and Renesas Solutions Corp.
*
************************************************************************/

#include "iodefine.h"
#ifdef __cplusplus
extern "C" {
#endif
extern void HardwareSetup(void);
#ifdef __cplusplus
}
#endif

//ちょっと違うかも・・・
//6/7に確認すること
void HardwareSetup(void)
{
	int i;
	
	//プロテクト解除
	SYSTEM.PRCR.WORD = 0xA503;
	
	SYSTEM.OPCCR.BIT.OPCM = 0;
	while(SYSTEM.OPCCR.BIT.OPCMTSF == 1);
	
	SYSTEM.HOCOPCR.BIT.HOCOPCNT = 0;
	for(i=0; i<100; i++);
	SYSTEM.HOCOCR.BIT.HCSTP = 0;
	for(i=0; i<100; i++);
	
	SYSTEM.SCKCR.LONG = 0x00000011;
	SYSTEM.SCKCR2.WORD = 0x0032;
	SYSTEM.BCKCR.BYTE = 0x01;
	
	SYSTEM.SCKCR3.WORD = 0x0100;

	SYSTEM.PRCR.WORD = 0xA500;
}

#ifndef _GLCD_H_
#define _GLCD_H_

	//Display Var
	#define VAR_TIME  1  
	#define VAR_P1_IN  2
	#define VAR_P2_A  3
	#define VAR_P3_B  4
	#define VAR_TXT   5
	
	#define LDISP_FREEZE_VALUE	1
	#define LDISP_RECTEMP_VALUE 0

	extern void gSendDisplay(char Var_ID, vu16 value);
#endif

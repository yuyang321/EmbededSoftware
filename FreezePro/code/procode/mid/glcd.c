#include "bsp.h"
#include "..\gos\glib.h"
#include "glcd.h"

//lcd cmd table
uint8_t CMD_LCD[16];
/*
	lcd display 
*/
void gSendDisplay(char Var_ID, vu16 value)
{
	 uint8_t cmd_lenth;
   CMD_LCD[0] = 0x5A;
	 CMD_LCD[1] = 0xA5;
	 CMD_LCD[2] = 0x05;
	 CMD_LCD[3] = 0x82;
   switch(Var_ID)
	{
		case VAR_TIME:
			  CMD_LCD[4] = 0x10;
		    CMD_LCD[5] = 0x11;
		break;
		case VAR_P1_IN:
	      CMD_LCD[4] = 0x10;
		    CMD_LCD[5] = 0x13;
		break;
		case VAR_P2_A:
			  CMD_LCD[4] = 0x10;
		    CMD_LCD[5] = 0x12;
		break;
		case VAR_P3_B:
			  CMD_LCD[4] = 0x10;
		    CMD_LCD[5] = 0x14;
		break;
		case VAR_TXT:
				CMD_LCD[4] = 0x10;
		    CMD_LCD[5] = 0x05;
		break;
		default:
			  return;
	 }
   switch(Var_ID)
	{
		case VAR_TIME:			  
		case VAR_P1_IN:	     
		case VAR_P2_A:
		case VAR_P3_B:
			  CMD_LCD[6] = value>>8;
		    CMD_LCD[7] = value;
		    cmd_lenth = 8;
		    break;
		case VAR_TXT:
			  if(value == LDISP_RECTEMP_VALUE)
				{
					CMD_LCD[6] = 0xB8;
					CMD_LCD[7] = 0xB4;
					CMD_LCD[8] = 0xCE;
					CMD_LCD[9] = 0xC2;		
				}
				else if(value == LDISP_FREEZE_VALUE)
				{
					CMD_LCD[6] = 0xC0;
					CMD_LCD[7] = 0xE4;
					CMD_LCD[8] = 0xB6;
					CMD_LCD[9] = 0xB3;	
				}
			  CMD_LCD[2] = 0x07;
				cmd_lenth = 0x10;				
		    break;
		default:
			  return;
	 }
	 SendUsartString(USART1,CMD_LCD,cmd_lenth,BLOCK_TRANS_MODE);
}

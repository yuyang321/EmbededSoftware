#include "bsp.h"
#include "..\gos\glib.h"
#include "..\user\guser.h"
#include "gmodbus.h"
#include "gSensorAlg.h"

volatile MODBUS_FLAG_s mbflag;
MB_DATA_FRAME_s mbtxframe;
MB_DATA_FRAME_s mbrxframe;

//Read flow buf
uint8_t mbrdflowbuf[8] = {FSENSOR_DEV_ADDRESS,FSENSOR_FUNCODE_READ_REG,\
		0x00,0x02,0x00,0x02};

void gmbrdflowcrc(void)
{
	uint16_t crcode;
	crcode = CRC16_Modbus(mbrdflowbuf, 6);
	mbrdflowbuf[6] = crcode >> 8;
	mbrdflowbuf[7] = crcode;
}

void gstartmodtimer(MODTIMER_STYLE_e mstyle)
{
	uint16_t timeval;
	uint16_t itflag;
	switch(mstyle)
	{
		case MODTIMER_STYLE_RECFRAME:
			timeval = MODTIMER_RECFRAME_WAIT_VAL;
			itflag = TIM_IT_CC1;
			mbflag.mbrframeflag = ENABLE;
			TIM_SetCompare1(MODBUS_HARD_TIMER,((uint32_t)timeval+TIM_GetCounter(MODBUS_HARD_TIMER))% MODBUS_HARD_TIMER_PERIOD);
		break;
		case MODTIMER_STYLE_TXFRAME:
			timeval = MODTIMER_TXFRAME_WAIT_VAL;
			itflag = TIM_IT_CC2;
			mbflag.mbtframeflag = ENABLE;
			TIM_SetCompare2(MODBUS_HARD_TIMER,((uint32_t)timeval+TIM_GetCounter(MODBUS_HARD_TIMER))% MODBUS_HARD_TIMER_PERIOD);
		break;
		case MODTIMER_STYLE_ACKFRAME:
			timeval = MODTIMER_ACKFRAME_WAIT_VAL;
			itflag = TIM_IT_CC3;
			mbflag.mbacktimeoutcheckflag = ENABLE;
			TIM_SetCompare3(MODBUS_HARD_TIMER,((uint32_t)timeval+TIM_GetCounter(MODBUS_HARD_TIMER))% MODBUS_HARD_TIMER_PERIOD);
		default:
		break;
	}
	
	TIM_ClearITPendingBit(MODBUS_HARD_TIMER, itflag);
	TIM_ITConfig(MODBUS_HARD_TIMER,itflag,ENABLE);
}

static uint8_t gmbtxfsensor(FSENSOR_OPERATE_MODE_e oper)
{
	if( ENABLE == mbflag.mbtframeflag )
		return 1;
	switch(oper)
	{
		case FSENSOR_OPERATE_MODE_RDFLOW:
				SendUsartString(USART_FSENSOR,mbrdflowbuf,sizeof(mbrdflowbuf),UNBLOCK_TRANS_MODE);
		break;
		default:
		break;
	}
	return 0;
}

#define MODBUS_REC_DATA_BUF_NUM		25
uint8_t fsensebuf[MODBUS_REC_DATA_BUF_NUM];

static void gmbrecvmsg(void)
{
	static uint8_t *pfsbuf;
	static uint8_t recnum = 0;
	static uint16_t crcode,crcodecmp;
	static MB_RECV_STATE_s mbrecstate = MB_RECV_STATE_IDLE;
	switch(mbrecstate)
	{
		case MB_RECV_STATE_IDLE:
		  if(mbflag.mbrframeflag == ENABLE)
				mbrecstate = MB_RECV_STATE_START_REC;
		break;
		case MB_RECV_STATE_START_REC:
			if(mbflag.mbrframeflag == RESET)
			{
				if( (recnum = (GFIFOGetUsed(&FsensorRxFifo))) > 4 )
				{
					pfsbuf = fsensebuf;
					while(GFIFOPop(&FsensorRxFifo,pfsbuf) == EXE_FINISH)
					{
						pfsbuf++;
					}
					crcode = CRC16_Modbus(fsensebuf, recnum-2);
					crcodecmp = (((uint16_t)fsensebuf[recnum-2])<<8) | fsensebuf[recnum-1];
					if(crcode == crcodecmp)
					{
						mbflag.mbrframeokflag = ENABLE;
						mbrecstate = MB_RECV_STATE_IDLE;
					}	
					else
						mbrecstate = MB_RECV_STATE_ERR;
				}
				else
					mbrecstate = MB_RECV_STATE_ERR;
			}
		break;
		case MB_RECV_STATE_ERR:
				mbrecstate = MB_RECV_STATE_IDLE;
		break;
		default:
		break;
	}
}

typedef enum _MB_QUERY_PROCESS_E_{
	MB_QUERY_PROCESS_IDLE,
	MB_QUERY_PROCESS_TRANS,
	MB_QUERY_PROCESS_WAIT_ACK,
	MB_QUERY_PROCESS_ANALYZE_FRAME,
	MB_QUERY_PROCESS_ERR,
}MB_QUERY_PROCESS_e;

void gfsevent(FSENSOR_OPERATE_MODE_e oper)
{
	static uint16_t	rdwordl,rdwordh; 
	static MB_QUERY_PROCESS_e mbqp = MB_QUERY_PROCESS_IDLE;
	switch(mbqp)
	{
		case MB_QUERY_PROCESS_IDLE:
			if(ENABLE == mbflag.mbtenablefalg)
			{
					mbflag.mbtenablefalg = DISABLE;
					mbqp = MB_QUERY_PROCESS_TRANS;
			}	
		break;
		case MB_QUERY_PROCESS_TRANS:
			if( !gmbtxfsensor(oper) )
			{
				gstartmodtimer(MODTIMER_STYLE_ACKFRAME);	
				mbqp = MB_QUERY_PROCESS_WAIT_ACK;
			}
		break;
		case MB_QUERY_PROCESS_WAIT_ACK:
			if(ENABLE == mbflag.mbacktimeoutcheckflag)
			{
				if(ENABLE == mbflag.mbrframeokflag)
				{
					mbflag.mbrframeokflag = DISABLE;
					mbqp = MB_QUERY_PROCESS_ANALYZE_FRAME;
				}
			}
			else
			{
				mbqp = MB_QUERY_PROCESS_ERR;
				//sysfaultinfor.sysfault.fsquery_flag = ENABLE;
			}
		break;
		case MB_QUERY_PROCESS_ANALYZE_FRAME:
			switch(oper)
			{
				case FSENSOR_OPERATE_MODE_RDFLOW:
					if(fsensebuf[1] == FSENSOR_FUNCODE_READ_REG)
					{
						rdwordl = (((uint16_t)fsensebuf[3]) << 8 ) | fsensebuf[4];
						rdwordh = (((uint16_t)fsensebuf[5]) << 8 ) | fsensebuf[6];		
						sysvar.curflowval = (((uint32_t)rdwordl) << 16) +rdwordh;
					}
				break;
			}
			mbqp = MB_QUERY_PROCESS_IDLE;
		break;
		case MB_QUERY_PROCESS_ERR:
			mbqp = MB_QUERY_PROCESS_IDLE;
		break;
	}
	gmbrecvmsg();
}


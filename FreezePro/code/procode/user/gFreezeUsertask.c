#include "bsp.h"
#include "guser.h"
#include "..\mid\gSensorAlg.h"
#include "..\mid\gmodbus.h"
#include "..\mid\LCD\hmi.h"

/******************************************************************************/
//USART Buffer and fifo
uint8_t LCDRxBuf[USART_LCD_RxBUFF_SIZE];
uint8_t LCDTxBuf[USART_LCD_TxBUFF_SIZE];
FIFO_t LCDRxFifo;
FIFO_t LCDTxFifo;

uint8_t FsensorRxBuf[USART_FSENSOR_RxBUFF_SIZE];
uint8_t FsensorTxBuf[USART_FSENSOR_TxBUFF_SIZE];
FIFO_t FsensorRxFifo;
FIFO_t FsensorTxFifo;
/******************************************************************************/
//sys var
volatile SYS_MANAGE_VAR_s	sysvar = {\
	.PressInfor = {0,0,0,0},\
	.curflowval = 0,\
	.ProbeInfor = {SOFTPROBE_TYPE_CODE,1,2,0},\
	.sysetval = {SYS_SET_EFFECT_VAL_1,DISABLE},\
	.disptime = 0,\
	.fvctrlword = 0,\
	.surstate = SYS_SURGERY_STATE_IDLE,\
	.keyswinfor = {.ftswstate = FOOTSWITCH_STATUS_UNPRESSED},\
	.sysfaultinfor.sysfaultword = 0,\
	.systate = SYS_APL_STATE_INIT,\
	.workstate = WORK_STATE_NONE,\
	.sptaskstate = SOFTPROBE_TASK_STATE_INIT_READSET,\
	.value2status = DISABLE,\
	.value3status = DISABLE,\
	.value4status = DISABLE,\
	.freezectrlparam = {0,0,0,0}\
};
/*
	Probe Detect Timer
*/
//static uint16_t cp = 0;
static SOFT_TIMER_t ProbeDetectTimer;
static void gProbeDetectTimer(void *arg)
{
		__NOP();
}
/*
	LED Indicated Timer
*/
static SOFT_TIMER_t sledtimer;
static void gsledindicate(void *arg)
{
	STATUS_LED1_TOGGLE;
}
/*
	LCD Display timer
*/
static SOFT_TIMER_t lcdisptimer;
static void glcdisptime(void *arg)
{
	if( FOOTSWITCH_STATUS_PRESSED == sysvar.keyswinfor.ftswstate )
	{
		sysvar.disptime++;
	}
	stSysKeyParms.fPSensor1 = sysvar.PressInfor.Pre1Value;
	stSysKeyParms.fPSensor2 = sysvar.PressInfor.Pre2Value;
	stSysKeyParms.fPSensor3 = sysvar.PressInfor.Pre3Value;
	stSysKeyParms.uiPSensor4 = sysvar.PressInfor.Pre4Value;
	stSysKeyParms.fFSensor1 = sysvar.curflowval;
	stSysKeyParms.fTreatTime = sysvar.disptime;
	stSysKeyParms.sysState = sysvar.surstate;

	sysvar.ProbeInfor.type = stSysKeyParms.probeType;
	sysvar.fvctrlword = stSysKeyParms.valvePFlow;
	sysvar.sysetval.seteffect = stSysKeyParms.treatMode;
	sysvar.value2status = stSysKeyParms.value2status;
	sysvar.value3status = stSysKeyParms.value3status;
	sysvar.value4status = stSysKeyParms.value4status;
	sysvar.freezectrlparam.step1fvctrlword = (uint16_t)(stSysKeyParms.step1pressure) * 100; 
	sysvar.freezectrlparam.step1flow = stSysKeyParms.step1flow * 1000;
	sysvar.freezectrlparam.step2fvctrlword = (uint16_t)(stSysKeyParms.step2pressure) * 100; 
	sysvar.freezectrlparam.step2flow = stSysKeyParms.step2flow * 1000;

	if( FOOTSWITCH_STATUS_UNPRESSED == sysvar.keyswinfor.ftswstate )
	{
		if( ENABLE == sysvar.value2status )
			FREEZE_VALUE_CTRL2_START;
		else
			FREEZE_VALUE_CTRL2_SHUT;
		if( ENABLE == sysvar.value3status )
			FREEZE_VALUE_CTRL3_START;
		else
			FREEZE_VALUE_CTRL3_SHUT;
		if( ENABLE == sysvar.value4status )
			FREEZE_VALUE_CTRL4_START;
		else
			FREEZE_VALUE_CTRL4_SHUT;
		gSetMCP4725Dig(gCalFvCtrlWordFromPval(sysvar.fvctrlword*100));
	}
	UpdateUI();
}
/*
	KEY SCAN Timer
*/
static SOFT_TIMER_t fswitchtimer;
static void gfswitchdetect(void *arg)
{
	static uint8_t pressedCnt = 0,unpressedCnt = 0;
	static KEY_PRESS_STATE_MACHINE_e keymachine = KEY_UNPRESSED_FILT_DETECTING_STATE_E;
	switch(keymachine)
	{
		case KEY_PRESSED_FILT_DETECTING_STATE_E:
			if(FOOTSWITCH_STATUS_PRESSED == GET_FOOTSWITCH_STATUS())
			{
				pressedCnt++;
				if(pressedCnt >= VOLID_SWITCH_PRESSED_CNT)
				{
					pressedCnt = 0;
					sysvar.keyswinfor.ftswstate = FOOTSWITCH_STATUS_PRESSED;
					keymachine = KEY_UNPRESSED_FILT_DETECTING_STATE_E;
				}
			}
			else
			{
					pressedCnt = 0;
			}
		break;
		case KEY_UNPRESSED_FILT_DETECTING_STATE_E:
			if(FOOTSWITCH_STATUS_UNPRESSED == GET_FOOTSWITCH_STATUS())
			{
				unpressedCnt++;
				if(unpressedCnt >= VOLID_SWITCH_PRESSED_CNT)
				{
					unpressedCnt = 0;
					sysvar.keyswinfor.ftswstate = FOOTSWITCH_STATUS_UNPRESSED;
					keymachine = KEY_PRESSED_FILT_DETECTING_STATE_E;
				}
			}
			else
			{
					unpressedCnt = 0;
			}
		break;
		default:
			__NOP();
		break;
	}

}
/**************************************************************************************/
/*
	Delay Timer
*/
static SOFT_TIMER_t DelayTimer;
uint8_t delaytimerflag = DISABLE;
static void gDelayTimer(void *arg)
{
	delaytimerflag = ENABLE;
}
/*
	P4 Press Check Timer
*/
static SOFT_TIMER_t P4Checktimer;
static void gP4Checktimer(void *arg)
{
	if( sysvar.PressInfor.Pre4Value > P4_BLOCK_UPLIMIT_VALUE)
	{
		sysvar.sysfaultinfor.sysfault.p4val_flag = ENABLE;
		sysvar.sptaskstate = SOFTPROBE_TASK_STATE_FINISH;
	}
}
/*
	P3 Press Check Timer
*/
#define P3_BLOCK_UPLIMIT_VALUE		2000	//20bar
static SOFT_TIMER_t P3Checktimer;
static void gP3Checktimer(void *arg)
{
	if(sysvar.PressInfor.Pre3Value > P3_BLOCK_UPLIMIT_VALUE)
	{
		sysvar.sysfaultinfor.sysfault.p3val_flag = ENABLE;
		sysvar.hptaskstate = HARDPROBE_TASK_STATE_FINISH;
	}
}
/*
	flow control implement
	caution: key word init
*/
int16_t curerror;
//int16_t lastflowval;
static int16_t flowctrlword = 30;			//3L ȥ˪ģʽ����Ч������
static uint16_t pressctrlword = 4000;	//40bar
FLOW_CTRL_ALG_PARM_s flowpidparm = {10,2,0}; //consider the scope of parm
/*
	flow control Timer
*/
static SOFT_TIMER_t flowctrltimer;	//flow control cycle
#define PID_CTRL_DIRECTION		(-1)
static void gflowctrltimer(void *arg)
{
	//flow control algorithm
	/*
		feedvar acq cycle
		filt cycle
		alg cycle
		control cycle
	*/
	static int16_t lasterror = 0;
	static int16_t lasterrorsum = 0;
	//Get flow value
	//.....................
	curerror = (sysvar.curflowval - flowctrlword)*PID_CTRL_DIRECTION;
	sysvar.fvctrlword = gpidctrlalg(&flowpidparm, curerror, lasterror, lasterrorsum);
	lasterrorsum +=curerror;
	lasterror = curerror;
	gSetMCP4725Dig(sysvar.fvctrlword);
}
/*
	flow query timer
*/
static SOFT_TIMER_t flowquerytimer;
static void gflowquerytimer(void *arg)
{
	//start flow sensor query
	mbflag.mbtenablefalg = ENABLE;
}
/**************************************************************************************/
//system init
void GosInit(void)
{
	//timer init
	InitTimerlist();
	//Init Usart pipe
	GFIFOInit(&LCDRxFifo,LCDRxBuf,sizeof(LCDRxBuf));
	GFIFOInit(&LCDTxFifo,LCDTxBuf,sizeof(LCDTxBuf));
	GFIFOInit(&FsensorRxFifo,FsensorRxBuf,sizeof(FsensorRxBuf));
	GFIFOInit(&FsensorTxFifo,FsensorTxBuf,sizeof(FsensorTxBuf));
	//Init Indicated LED timer
	GTimerNodeInit(&sledtimer,gsledindicate, LED_INDICATE_INIT_INTERVAL_TIME , LED_INDICATE_INIT_INTERVAL_TIME );
	//Init FootSwitch timer
	GTimerNodeInit(&fswitchtimer,gfswitchdetect, FOOTSWITCH_DETECT_INTERVAL_TIME , FOOTSWITCH_DETECT_INTERVAL_TIME );
	//Init Probe Detect timer
	GTimerNodeInit(&ProbeDetectTimer,gProbeDetectTimer, PROBE_DETECT_INTERVAL_TIME, PROBE_DETECT_INTERVAL_TIME);
	//Init LCD Display timer
	GTimerNodeInit(&lcdisptimer,glcdisptime, LCD_DISPLAY_INTERVAL_TIME , LCD_DISPLAY_INTERVAL_TIME );
	//Init P4 check timer
	GTimerNodeInit(&P4Checktimer,gP4Checktimer, SYS_SOFTPROBE_P4_CHECK_INTERVAL_TIME , SYS_SOFTPROBE_P4_CHECK_INTERVAL_TIME );
	//Init P3 check timer
	GTimerNodeInit(&P3Checktimer,gP3Checktimer, SYS_HARDPROBE_P3_CHECK_INTERVAL_TIME , SYS_HARDPROBE_P3_CHECK_INTERVAL_TIME );
	//Init flow control timer
	//GTimerNodeInit(&flowctrltimer,gflowctrltimer, SYS_FLOW_CTRL_INTERVAL_TIME , SYS_FLOW_CTRL_INTERVAL_TIME );
	//Init flow query timer
	GTimerNodeInit(&flowquerytimer,gflowquerytimer, SYS_FLOW_QUERY_INTERVAL_TIME , SYS_FLOW_QUERY_INTERVAL_TIME );
	//Init get crc code
	gmbrdflowcrc();
	//Start SYS_APL
	SET_EVENT( SYS_APL_EVENT_E );
}

static SYS_INIT_STATE_e initstate = SYS_INIT_STATE_START;
static void gsysinitf(void)
{
	if(0 != sysvar.sysfaultinfor.sysfaultword)
	{
		initstate = SYS_INIT_STATE_FAULT_DEAL;
	}
	switch(initstate)
	{
		case SYS_INIT_STATE_START:
			GTimerStart(&lcdisptimer);
			GTimerStart(&P4Checktimer);
			GTimerStart(&ProbeDetectTimer);
			GTimerStart(&flowquerytimer);
			GTimerNodeInit(&DelayTimer,gDelayTimer, SYS_INIT_WAIT_ACQ_TIME , 0 );
			GTimerStart(&DelayTimer);
			initstate = SYS_INIT_STATE_WAIT_ACQ;
		break;
		case SYS_INIT_STATE_WAIT_ACQ:
			if(ENABLE == delaytimerflag)
			{
				delaytimerflag = DISABLE;
				initstate = SYS_INIT_STATE_P1_CHECK;
			}
		break;
		case SYS_INIT_STATE_P1_CHECK:
			if( sysvar.PressInfor.Pre1Value <= INIT_P1_UPLIMIT_VALUE && sysvar.PressInfor.Pre1Value >= INIT_P1_DOWNLIMIT_VALUE)
			{
				sysvar.sysfaultinfor.sysfault.p1val_flag = SYS_FAULT_P1_INIT_NORMAL_PRESS;
				initstate = SYS_INIT_STATE_P234_CHECK;
			}
			else
			{
					if( sysvar.PressInfor.Pre1Value > INIT_P1_UPLIMIT_VALUE)
					{
							 sysvar.sysfaultinfor.sysfault.p1val_flag = SYS_FAULT_P1_INIT_OVER_PRESS;
					}
					else
					{
							sysvar.sysfaultinfor.sysfault.p1val_flag = SYS_FAULT_P1_INIT_LOW_PRESS;
					}
					//sysvar.systate = SYS_APL_STATE_FAULT;
			}
		break;
		case SYS_INIT_STATE_P234_CHECK:
			if((sysvar.PressInfor.Pre2Value > INIT_P23_UPLIMIT_VALUE) || (sysvar.PressInfor.Pre3Value > INIT_P23_UPLIMIT_VALUE) \
				|| (sysvar.PressInfor.Pre4Value > INIT_P4_UPLIMIT_VAULE))
			{
				sysvar.sysfaultinfor.sysfault.p234val_flag = SYS_FAULT_P234_INIT_ABNORMAL_PRESS;
				//sysvar.systate = SYS_APL_STATE_FAULT;
			}
			else
				initstate = SYS_INIT_STATE_PROBE_READ;
		break;
		case SYS_INIT_STATE_PROBE_READ:
			__NOP();
			initstate = SYS_INIT_STATE_FINISH;
		break;
		case SYS_INIT_STATE_FINISH:
			__NOP();
			GTimerStart(&sledtimer);
			GTimerStart(&fswitchtimer);
			initstate = SYS_INIT_STATE_START;
			sysvar.systate = SYS_APL_STATE_IDLE;
		break;
		case SYS_INIT_STATE_FAULT_DEAL:
				GTimerStop(&ProbeDetectTimer);
				initstate = SYS_INIT_STATE_START;
				sysvar.systate = SYS_APL_STATE_FAULT;
		break;
		default:
			__NOP();
		break;
	}
}

void gsysidlef(void)
{
	if(0 != sysvar.sysfaultinfor.sysfaultword)
	{
			GTimerStop(&sledtimer);
			GTimerStop(&fswitchtimer);
			//������Ϣ
			//....
			sysvar.systate = SYS_APL_STATE_FAULT;
	}
	else
	{
		if(FOOTSWITCH_STATUS_PRESSED == sysvar.keyswinfor.ftswstate)
		{
			sysvar.systate = SYS_APL_STATE_WORK;
			sysvar.surstate = SYS_SURGERY_STATE_FREEZE;
		}
		else
		{
			__NOP();
		}
	}
	//update set
}

void gSetFlowValuePress(void)
{
	switch(sysvar.workstate)
	{
		case WORK_STATE_NONE:
			__NOP();
		break;
		case WORK_STATE_SOFTPROBE:
				switch(sysvar.sptaskstate)
				{
					case SOFTPROBE_TASK_STATE_FINISH:
						gSetMCP4725Dig(0);
					break;
					default:
						if( ENABLE ==sysvar.sysetval.setremovefrost )
						{
							gSetMCP4725Dig(gCalFvCtrlWordFromPval(pressctrlword));
						}
						else
						{
							gSetMCP4725Dig(gCalFvCtrlWordFromPval(pressctrlword));
						}
					break;
				}
		break;
		case WORK_STATE_HARDPROBE:
			switch(sysvar.hptaskstate)
				{
					case HARDPROBE_TASK_STATE_FINISH:
						gSetMCP4725Dig(0);
					break;
					default:
						if( ENABLE ==sysvar.sysetval.setremovefrost )
						{
							gSetMCP4725Dig(gCalFvCtrlWordFromPval(pressctrlword));
						}
						else
						{
							gSetMCP4725Dig(gCalFvCtrlWordFromPval(pressctrlword));
						}
					break;
				}
		break;
		case WORK_STATE_JET:

		break;
	}
}
#define FLOW_INC_STABLE_CNT_VAL			5000
#define FLOW_DEC_STABLE_CNT_VAL			5000
static void gsoftprobef(void)
{
	  static SYS_FREEZE_CYCLE_CTRL_PARM_s freezetempparm;
		static SYS_FREEZE_CYCLE_CTRL_STATE_e freezecyclestate = SYS_FREEZE_CYCLE_CTRL_STATE_STEP1_FLOWSET;
		static uint16_t tempcnt = 0;
		if(0 != sysvar.sysfaultinfor.sysfaultword)
		{
			if(SOFTPROBE_TASK_STATE_FINISH_WAIT != sysvar.sptaskstate)
				sysvar.sptaskstate = SOFTPROBE_TASK_STATE_FINISH;
		}
		else
		{
			if(FOOTSWITCH_STATUS_UNPRESSED == sysvar.keyswinfor.ftswstate)
			{
				if( sysvar.sptaskstate != SOFTPROBE_TASK_STATE_FINISH_WAIT )
					sysvar.sptaskstate = SOFTPROBE_TASK_STATE_FINISH;
			}
		}
		switch(sysvar.sptaskstate)
		{
			case SOFTPROBE_TASK_STATE_INIT_READSET:
				switch(sysvar.sysetval.seteffect)
				{
					case SYS_SET_EFFECT_VAL_1:
						flowctrlword = 30;
						pressctrlword = SYS_SET_EFFECT1_PRESS2_VAL;
					break;
					case SYS_SET_EFFECT_VAL_2:
						flowctrlword = 40;
						pressctrlword = SYS_SET_EFFECT2_PRESS2_VAL;
					break;
					case SYS_SET_EFFECT_VAL_3:
						flowctrlword = 50;
						pressctrlword = SYS_SET_EFFECT3_PRESS2_VAL;
					break;
					case SYS_SET_EFFECT_VAL_4:
						freezetempparm.step1fvctrlword = sysvar.freezectrlparam.step1fvctrlword;
						freezetempparm.step1flow = sysvar.freezectrlparam.step1flow;
						freezetempparm.step2fvctrlword = sysvar.freezectrlparam.step2fvctrlword;
						freezetempparm.step2flow = sysvar.freezectrlparam.step2flow;
					break;
					default:
						__NOP();
					break;
				}
				FREEZE_VALUE_CTRL4_START;
				GTimerNodeInit(&DelayTimer,gDelayTimer, SYS_VALUE_SWITCH_DELAY_TIME , 0 );
				delaytimerflag = DISABLE;
				GTimerStart(&DelayTimer);
				GTimerNodeInit(&sledtimer,gsledindicate, 100 , 100 );
				GTimerStart(&sledtimer);
				if(SYS_SET_EFFECT_VAL_4 == sysvar.sysetval.seteffect)
				{
					sysvar.sptaskstate = SOFTPROBE_TASK_STATE_CIRCLE_CTRL;
				}
				else
					sysvar.sptaskstate = SOFTPROBE_TASK_STATE_INIT_VALUE_WAIT;
			break;
			case SOFTPROBE_TASK_STATE_INIT_VALUE_WAIT:
				if(ENABLE == delaytimerflag)
				{
					//Set flow value outlet press
					gSetFlowValuePress();
					GTimerNodeInit(&DelayTimer,gDelayTimer, SYS_SOFTPROBE_P12_CHECK_DELAY_TIME , 0 );
					delaytimerflag = DISABLE;
					GTimerStart(&DelayTimer);
					//start P4 detect by cycle
					//GTimerStart(&P4Checktimer);
					//start flow control
					//GTimerStart(&flowctrltimer);
					sysvar.sptaskstate = SOFTPROBE_TASK_STATE_INIT_P12_CHECK;
				}
			break;
			case SOFTPROBE_TASK_STATE_INIT_P12_CHECK:
				if(ENABLE == delaytimerflag)
				{
						delaytimerflag = DISABLE;
					//���ⲻ����
//						if(sysvar.PressInfor.Pre1Value > sysvar.PressInfor.Pre2Value)
//						{
//							if((sysvar.PressInfor.Pre1Value - sysvar.PressInfor.Pre2Value) > 200 )
//							{
//								sysvar.sysfaultinfor.sysfault.p12difval_flag = 1;
//								sysvar.sptaskstate = SOFTPROBE_TASK_STATE_FINISH;
//							}
//							else
//							{
//								sysvar.sptaskstate = SOFTPROBE_TASK_STATE_FLOWCTRL;
//							}
//						}
//						else
//						{
//							if((sysvar.PressInfor.Pre2Value - sysvar.PressInfor.Pre1Value) > 200 )
//							{
//								sysvar.sysfaultinfor.sysfault.p12difval_flag = 1;
//								sysvar.sptaskstate = SOFTPROBE_TASK_STATE_FINISH;
//							}
//							else
//							{
//								sysvar.sptaskstate = SOFTPROBE_TASK_STATE_FLOWCTRL;
//							}
//						}
				}
			break;
			case SOFTPROBE_TASK_STATE_FLOWCTRL:
					__NOP();
					//start cycle control
			break;
			case SOFTPROBE_TASK_STATE_CIRCLE_CTRL:
					switch(freezecyclestate)
					{
						case SYS_FREEZE_CYCLE_CTRL_STATE_STEP1_FLOWSET:
							gSetMCP4725Dig(gCalFvCtrlWordFromPval(freezetempparm.step1fvctrlword));
							freezecyclestate = SYS_FREEZE_CYCLE_CTRL_STATE_STEP1_FLOWDETECT;
						break;
						case SYS_FREEZE_CYCLE_CTRL_STATE_STEP1_FLOWDETECT:
							if(sysvar.curflowval > freezetempparm.step1flow)
							{
									if( tempcnt++ > FLOW_INC_STABLE_CNT_VAL )
									{
										tempcnt = 0;
										freezecyclestate = SYS_FREEZE_CYCLE_CTRL_STATE_STEP2_FLOWSET;
									}
							}
							else
							{
//								if(tempcnt > 0)
//								{
//									tempcnt--;
//								}
								tempcnt = 0;
							}
						break;
						case SYS_FREEZE_CYCLE_CTRL_STATE_STEP2_FLOWSET:
							gSetMCP4725Dig(gCalFvCtrlWordFromPval(freezetempparm.step2fvctrlword));
							freezecyclestate = SYS_FREEZE_CYCLE_CTRL_STATE_STEP2_FLOWDETECT;
						break;
						case SYS_FREEZE_CYCLE_CTRL_STATE_STEP2_FLOWDETECT:
							if(sysvar.curflowval < freezetempparm.step2flow)
							{
									if( tempcnt++ > FLOW_DEC_STABLE_CNT_VAL )
									{
										tempcnt = 0;
										freezecyclestate = SYS_FREEZE_CYCLE_CTRL_STATE_STEP1_FLOWSET;
									}
							}
							else
							{
//								if(tempcnt > 0)
//								{
//									tempcnt--;
//								}
								tempcnt = 0;
							}
						break;
						default:
						break;
					}
			break;
			case SOFTPROBE_TASK_STATE_FINISH:
				gSetFlowValuePress();
				GTimerNodeInit(&DelayTimer,gDelayTimer, SYS_VALUE_SWITCH_DELAY_TIME , 0 );
				delaytimerflag = DISABLE;
				GTimerStart(&DelayTimer);
				sysvar.sptaskstate = SOFTPROBE_TASK_STATE_FINISH_WAIT;
				GTimerNodeInit(&sledtimer,gsledindicate, LED_INDICATE_INIT_INTERVAL_TIME , LED_INDICATE_INIT_INTERVAL_TIME );
				GTimerStart(&sledtimer);
			break;
			case SOFTPROBE_TASK_STATE_FINISH_WAIT:
				if(ENABLE == delaytimerflag)
				{
					delaytimerflag = DISABLE;
					FREEZE_VALUE_CTRL4_SHUT;
					//state recover
					freezecyclestate = SYS_FREEZE_CYCLE_CTRL_STATE_STEP1_FLOWSET;
					sysvar.sptaskstate = SOFTPROBE_TASK_STATE_INIT_READSET;
					sysvar.workstate = WORK_STATE_NONE;
					sysvar.surstate = SYS_SURGERY_STATE_IDLE;
					if(0 != sysvar.sysfaultinfor.sysfaultword)
					{
						sysvar.systate = SYS_APL_STATE_FAULT;
					}
					else
					{
						sysvar.systate = SYS_APL_STATE_IDLE;
					}
				}
			break;
			default:
				__NOP();
			break;
		}
}

static void ghardprobef(void)
{
	static uint16_t p23chaval = 0;
	if(0 != sysvar.sysfaultinfor.sysfaultword)
		{
			if(HARDPROBE_TASK_STATE_FINISH_WAIT != sysvar.hptaskstate)
				sysvar.hptaskstate = HARDPROBE_TASK_STATE_FINISH;
		}
		else
		{
			if(FOOTSWITCH_STATUS_UNPRESSED == sysvar.keyswinfor.ftswstate)
			{
				if( sysvar.hptaskstate != HARDPROBE_TASK_STATE_FINISH_WAIT )
					sysvar.hptaskstate = HARDPROBE_TASK_STATE_FINISH;
			}
		}
		switch(sysvar.hptaskstate)
		{
			case HARDPROBE_TASK_STATE_INIT_READSET:
				switch(sysvar.sysetval.seteffect)
				{
					case SYS_SET_EFFECT_VAL_1:
						flowctrlword = 30;
						pressctrlword = SYS_SET_EFFECT1_PRESS2_VAL;
					break;
					case SYS_SET_EFFECT_VAL_2:
						flowctrlword = 40;
						pressctrlword = SYS_SET_EFFECT2_PRESS2_VAL;
					break;
					case SYS_SET_EFFECT_VAL_3:
						flowctrlword = 50;
						pressctrlword = SYS_SET_EFFECT3_PRESS2_VAL;
					break;
					default:
						__NOP();
					break;
				}
				FREEZE_VALUE_CTRL4_START;
				GTimerNodeInit(&DelayTimer,gDelayTimer, SYS_VALUE_SWITCH_DELAY_TIME , 0 );
				delaytimerflag = DISABLE;
				GTimerStart(&DelayTimer);
				sysvar.hptaskstate = HARDPROBE_TASK_STATE_INIT_VALUE_WAIT;
				GTimerNodeInit(&sledtimer,gsledindicate, 100 , 100 );
				GTimerStart(&sledtimer);
			break;
			case HARDPROBE_TASK_STATE_INIT_VALUE_WAIT:
				if(ENABLE == delaytimerflag)
				{
					//Set flow value outlet press
					gSetFlowValuePress();
					GTimerNodeInit(&DelayTimer,gDelayTimer, SYS_SOFTPROBE_P12_CHECK_DELAY_TIME , 0 );
					delaytimerflag = DISABLE;
					GTimerStart(&DelayTimer);
					//start P3 detect by cycle
					GTimerStart(&P3Checktimer);
					//start flow control
					//GTimerStart(&flowctrltimer);
					sysvar.hptaskstate = HARDPROBE_TASK_STATE_INIT_P12_CHECK;
				}
			break;
			case HARDPROBE_TASK_STATE_INIT_P12_CHECK:
				if(ENABLE == delaytimerflag)
				{
						delaytimerflag = DISABLE;
					//���ⲻ����
//						if(sysvar.PressInfor.Pre1Value > sysvar.PressInfor.Pre2Value)
//						{
//							if((sysvar.PressInfor.Pre1Value - sysvar.PressInfor.Pre2Value) > 200 )
//							{
//								sysvar.sysfaultinfor.sysfault.p12difval_flag = 1;
//								sysvar.sptaskstate = SOFTPROBE_TASK_STATE_FINISH;
//							}
//							else
//							{
//								sysvar.sptaskstate = SOFTPROBE_TASK_STATE_FLOWCTRL;
//							}
//						}
//						else
//						{
//							if((sysvar.PressInfor.Pre2Value - sysvar.PressInfor.Pre1Value) > 200 )
//							{
//								sysvar.sysfaultinfor.sysfault.p12difval_flag = 1;
//								sysvar.sptaskstate = SOFTPROBE_TASK_STATE_FINISH;
//							}
//							else
//							{
//								sysvar.sptaskstate = SOFTPROBE_TASK_STATE_FLOWCTRL;
//							}
//						}
					sysvar.hptaskstate = HARDPROBE_TASK_STATE_FLOWCTRL;
				}
			break;
			case HARDPROBE_TASK_STATE_FLOWCTRL:
					__NOP();
			break;
			case HARDPROBE_TASK_STATE_FINISH:
				FREEZE_VALUE_CTRL3_START;
				FREEZE_VALUE_CTRL2_START;
				//�����ж�����
				GTimerNodeInit(&DelayTimer,gDelayTimer, 2000 , 0 );
				delaytimerflag = DISABLE;
				GTimerStart(&DelayTimer);
				sysvar.hptaskstate = HARDPROBE_TASK_STATE_FINISH_WAIT;
				GTimerNodeInit(&sledtimer,gsledindicate, LED_INDICATE_INIT_INTERVAL_TIME , LED_INDICATE_INIT_INTERVAL_TIME );
				GTimerStart(&sledtimer);
			break;
			case HARDPROBE_TASK_STATE_FINISH_WAIT:
				if(sysvar.PressInfor.Pre2Value > sysvar.PressInfor.Pre3Value)
					p23chaval = sysvar.PressInfor.Pre2Value - sysvar.PressInfor.Pre3Value;
				else
					p23chaval = sysvar.PressInfor.Pre3Value - sysvar.PressInfor.Pre2Value;
				if( (ENABLE == delaytimerflag) || (p23chaval < 200) )
				{
					delaytimerflag = DISABLE;
					//�Ƿ���Ҫ����ʱ����
					FREEZE_VALUE_CTRL1_SHUT;
					FREEZE_VALUE_CTRL2_SHUT;
					FREEZE_VALUE_CTRL3_SHUT;
					FREEZE_VALUE_CTRL3_SHUT;
					//state recover
					sysvar.hptaskstate = HARDPROBE_TASK_STATE_INIT_READSET;
					sysvar.workstate = WORK_STATE_NONE;
					sysvar.surstate = SYS_SURGERY_STATE_IDLE;
					if(0 != sysvar.sysfaultinfor.sysfaultword)
					{
						sysvar.systate = SYS_APL_STATE_FAULT;
					}
					else
					{
						sysvar.systate = SYS_APL_STATE_IDLE;
					}
				}
			break;
			default:
				__NOP();
			break;
		}
}

static void gjetprobef(void)
{
	if(0 != sysvar.sysfaultinfor.sysfaultword )
	{
		sysvar.workstate = WORK_STATE_NONE;
		sysvar.surstate = SYS_SURGERY_STATE_IDLE;
		sysvar.systate = SYS_APL_STATE_FAULT;
	}
	else
	{
		if(FOOTSWITCH_STATUS_UNPRESSED == sysvar.keyswinfor.ftswstate)
		{
			sysvar.workstate = WORK_STATE_NONE;
			sysvar.surstate = SYS_SURGERY_STATE_IDLE;
			sysvar.systate = SYS_APL_STATE_IDLE;
		}
	}
}

void gsysworkf(void)
{
	switch(sysvar.workstate)
	{
		case WORK_STATE_NONE:
			switch(sysvar.ProbeInfor.type)
			{
				case SOFTPROBE_TYPE_CODE:
					sysvar.workstate = WORK_STATE_SOFTPROBE;
				break;
				case HARDPROBE_TYPE_CODE:
					sysvar.workstate = WORK_STATE_HARDPROBE;
				break;
				case JETPROBE_TYPE_CODE:
					sysvar.workstate = WORK_STATE_JET;
				break;
				default:
					__NOP();
				break;
			}
			sysvar.disptime = 0;
		break;
		case WORK_STATE_SOFTPROBE:
			gsoftprobef();
		break;
		case WORK_STATE_HARDPROBE:
			ghardprobef();
		break;
		case WORK_STATE_JET:
			gjetprobef();
		break;
		default:
			__NOP();
		break;
	}
}

void gsysfaultg(void)
{
	switch(sysvar.sysfaultinfor.sysfault.p1val_flag)
	{
		case SYS_FAULT_P1_INIT_OVER_PRESS:
			//����������ʾ��ʼ��������Ϣ
			//..........
			//after delay,init again!
		case SYS_FAULT_P1_INIT_LOW_PRESS:
			delay_nms(2000);
			sysvar.sysfaultinfor.sysfault.p1val_flag = SYS_FAULT_P1_INIT_NORMAL_PRESS;
			sysvar.systate = SYS_APL_STATE_INIT;
		break;
	}
	switch(sysvar.sysfaultinfor.sysfault.p234val_flag)
	{
		case SYS_FAULT_P234_INIT_ABNORMAL_PRESS:
			delay_nms(2000);
			sysvar.sysfaultinfor.sysfault.p234val_flag = SYS_FAULT_P234_INIT_NORMAL_PRESS;
			sysvar.systate = SYS_APL_STATE_INIT;
		break;
		default:
			__NOP();
		break;
	}
	switch(sysvar.sysfaultinfor.sysfault.p12difval_flag)
	{
		case 1:
			__NOP();
		break;
	}
	switch(sysvar.sysfaultinfor.sysfault.p4val_flag)
	{
		case 1:
			//IIC error,or p4 fault
		sysvar.sysfaultinfor.sysfault.p4val_flag = 0;
			__NOP();
		break;
	}
}

void SYS_APL_EVENT(void *arg)
{
	gfsevent(FSENSOR_OPERATE_MODE_RDFLOW);
	hmiCmdProcess();
	switch(sysvar.systate)
	{
		case SYS_APL_STATE_INIT:
			gsysinitf();
		break;
		case SYS_APL_STATE_IDLE:
			gsysidlef();
		break;
		case SYS_APL_STATE_WORK:
			gsysworkf();
		break;
		case SYS_APL_STATE_FAULT:
			gsysfaultg();
		break;
		default:
		break;
	}
	SET_EVENT( SYS_APL_EVENT_E );
}

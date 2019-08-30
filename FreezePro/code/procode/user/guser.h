#ifndef _GUSER_H_
#define _GUSER_H_

	#include "..\gos\glib.h"
	#pragma pack (1)
	//#define _DEBUG_
/**
	USART FIFO  MACRO/Variable
**/
	#define USART_LCD_RxBUFF_SIZE		80
	#define USART_LCD_TxBUFF_SIZE		100
	extern uint8_t LCDRxBuf[USART_LCD_RxBUFF_SIZE];
	extern uint8_t LCDTxBuf[USART_LCD_TxBUFF_SIZE];
	extern FIFO_t LCDRxFifo;
	extern FIFO_t LCDTxFifo;

	#define USART_FSENSOR_RxBUFF_SIZE		50
	#define USART_FSENSOR_TxBUFF_SIZE		50
	extern uint8_t FsensorRxBuf[USART_FSENSOR_RxBUFF_SIZE];
	extern uint8_t FsensorTxBuf[USART_FSENSOR_TxBUFF_SIZE];
	extern FIFO_t FsensorRxFifo;
	extern FIFO_t FsensorTxFifo;
/**
	Analog Val Filt Arg
**/
/****************************************************************************************************/
	/*********No Used**************/
	#define MOVING_WINDOW_FILT		0
	#define INTEGER_WINDOW_FILT		1
	#define ADC_FILT_MODE					INTEGER_WINDOW_FILT

	#define ANALOG_VAR_INIT_TIME_MAX										(5) //5ms complete analog var init
	#define ANALOG_CHANNEL_ACQ_INTERVAL_INIT_TIME				(((ANALOG_VAR_INIT_TIME_MAX / ADC_BUFF_NUM) > 1) ? (ANALOG_VAR_INIT_TIME_MAX / ADC_BUFF_NUM) : 1)
	#define ANALOG_VAR_INIT_CNT													ADC_BUFF_NUM
/****************************************************************************************************/
	//adc var init state machine
	typedef enum ADC_ANALOG_VAR_INIT_STATE_E{
		ADC_ANALOG_VAR_INIT_STATE_START,
		ADC_ANALOG_VAR_INIT_STATE_WORK,
	}ADC_ANALOG_VAR_INIT_STATE_e;
/*
	Press Limit Val
*/
#define INIT_P1_UPLIMIT_VALUE				8000	//80bar
#define INIT_P1_DOWNLIMIT_VALUE			3000	//50bar
#define INIT_P23_UPLIMIT_VALUE			100		//1.0bar
#define INIT_P4_UPLIMIT_VAULE				101		//101KPa
#define P4_BLOCK_UPLIMIT_VALUE	1000	//10bar = 1MPa
/*
	Timer Task Parm
*/
#define LCD_DISPLAY_INTERVAL_TIME									100
#define LED_INDICATE_INIT_INTERVAL_TIME						500
#define LED_INDICATE_WORK_INTERVAL_TIME						100
#define FOOTSWITCH_DETECT_INTERVAL_TIME						25
//#define ANALOG_CHANNEL_ACQ_INTERVAL_TIME					1
#define PROBE_DETECT_INTERVAL_TIME								100
#define SYS_INIT_WAIT_ACQ_TIME										1000
#define SYS_SOFTPROBE_P12_CHECK_DELAY_TIME				1000
#define SYS_SOFTPROBE_P4_CHECK_INTERVAL_TIME			500
#define SYS_HARDPROBE_P3_CHECK_INTERVAL_TIME			500
#define SYS_FLOW_CTRL_INTERVAL_TIME								100
#define SYS_FLOW_QUERY_INTERVAL_TIME							100
#define SYS_VALUE_SWITCH_DELAY_TIME								60

/*
	FootSwitch
*/
#define VOLID_SWITCH_PRESSED_CNT	(150 / FOOTSWITCH_DETECT_INTERVAL_TIME)  //����ʱ��
//FootSwitch state struct
typedef struct _SYS_KEYSWITCH_INFOR_S{
	uint8_t ftswstate;
}SYS_KEYSWITCH_INFOR_s;
//FootSwitch Detect State machine
typedef enum KEY_DETECT_STATE_MACHINE_E{
	KEY_PRESSED_FILT_DETECTING_STATE_E,
	KEY_UNPRESSED_FILT_DETECTING_STATE_E,
}KEY_PRESS_STATE_MACHINE_e;

/*
	System Fault Manage
*/
//P1 Fault Code
typedef enum _SYS_FAULT_P1_CODE_E{
	SYS_FAULT_P1_INIT_NORMAL_PRESS = 0,
	SYS_FAULT_P1_INIT_OVER_PRESS = 1,
	SYS_FAULT_P1_INIT_LOW_PRESS = 2,
}SYS_FAULT_P1_CODE_e;
//P2 P3 P4 Fault Code
typedef enum _SYS_FAULT_P234_CODE_E{
	SYS_FAULT_P234_INIT_NORMAL_PRESS = 0,
	SYS_FAULT_P234_INIT_ABNORMAL_PRESS = 1,
}SYS_FAULT_P234_CODE_e;
//System fault flag struct
typedef struct _SYS_FAULT_INFOR_S{
	uint32_t	p1val_flag:2;
	uint32_t  p234val_flag:1;
	uint32_t  probefault_flag:1;
	uint32_t  p12difval_flag:1;
	uint32_t  p4val_flag:1;
	uint32_t  p3val_flag:1;
	uint32_t  fsquery_flag:1;
	uint32_t  reserve:24;
}SYS_FAULT_INFOR_s;
//system fault flag union
typedef union _SYS_FAULT_INFOR_U{
		SYS_FAULT_INFOR_s sysfault;
		uint32_t sysfaultword;
}SYS_FAULT_INFOR_u;

/*
	System HMI
*/
typedef struct _SYS_DISPLAY_INFOR_S{
	uint32_t sysdisval;
}SYS_DISPLAY_INFOR_s;

/*
	System program flow manage
*/
//main program flow
typedef enum _SYS_APL_STATE_E{
	SYS_APL_STATE_INIT,
	SYS_APL_STATE_IDLE,
	SYS_APL_STATE_WORK,
	SYS_APL_STATE_FAULT,
}SYS_APL_STATE_e;
//init program sub flow
typedef enum _SYS_INIT_STATE_E{
	SYS_INIT_STATE_START,
	SYS_INIT_STATE_WAIT_ACQ,
	SYS_INIT_STATE_P1_CHECK,
	SYS_INIT_STATE_P234_CHECK,
	SYS_INIT_STATE_PROBE_READ,

	SYS_INIT_STATE_FINISH,
	SYS_INIT_STATE_FAULT_DEAL,
}SYS_INIT_STATE_e;
//work program sub flow
typedef enum _WORK_STATE_E{
	WORK_STATE_NONE,
	WORK_STATE_SOFTPROBE,
	WORK_STATE_HARDPROBE,
	WORK_STATE_JET,
}WORK_STATE_e;
//system fault flow
typedef enum _SYS_FAULT_STATE_E{
	SYS_FAULT_STATE_INIT,
}SYS_FAULT_STATE_e;
//soft probe program sub flow
typedef enum _SOFTPROBE_TASK_STATE_E{
	SOFTPROBE_TASK_STATE_INIT_READSET,
	SOFTPROBE_TASK_STATE_INIT_VALUE_WAIT,
	SOFTPROBE_TASK_STATE_INIT_P12_CHECK,
	//........................................
	SOFTPROBE_TASK_STATE_CIRCLE_CTRL,
	//........................................
	SOFTPROBE_TASK_STATE_FLOWCTRL,
	SOFTPROBE_TASK_STATE_FINISH,
	SOFTPROBE_TASK_STATE_FINISH_WAIT,
	//SOFTPROBE_TASK_STATE_FAULT_DEAL,
}SOFTPROBE_TASK_STATE_e;
//hard probe program sub flow
typedef enum _HARDPROBE_TASK_STATE_E{
	HARDPROBE_TASK_STATE_INIT_READSET,
	HARDPROBE_TASK_STATE_INIT_VALUE_WAIT,
	HARDPROBE_TASK_STATE_INIT_P12_CHECK,
	HARDPROBE_TASK_STATE_FLOWCTRL,
	HARDPROBE_TASK_STATE_FINISH,
	HARDPROBE_TASK_STATE_FINISH_WAIT,
	//SOFTPROBE_TASK_STATE_FAULT_DEAL,
}HARDPROBE_TASK_STATE_e;
//jet program sub flow

/*
	system var struct
*/
//system set parameter struct
typedef struct _SYS_SET_INFOR_S{
	uint8_t seteffect;						//freeze effect
	uint8_t setremovefrost;				//remove frost mode
}SYS_SET_INFOR_s;
//Press Sensor data struct
typedef struct _SYS_PRESS_INFOR_S{
		uint16_t Pre1Value;
		uint16_t Pre2Value;
		uint16_t Pre3Value;
		uint16_t Pre4Value;
}SYS_PRESS_INFOR_s;
//Probe data struct
typedef struct _SYS_PROBE_INFOR_S{
		uint16_t type;
		uint16_t ftime;
		uint16_t ltime;
		uint16_t usecnt;
}SYS_PROBE_INFOR_s;

typedef struct _SYS_FREEZE_CYCLE_CTRL_PARM_S{
	uint16_t step1fvctrlword;			//unit：0.01bar 
	uint16_t step1flow;						//uint: mL
	uint16_t step2fvctrlword;			//unit：0.01bar 
	uint16_t step2flow;						//uint: mL
}SYS_FREEZE_CYCLE_CTRL_PARM_s;

typedef enum _SYS_FREEZE_CYCLE_CTRL_STATE_E{
		SYS_FREEZE_CYCLE_CTRL_STATE_STEP1_FLOWSET,
		SYS_FREEZE_CYCLE_CTRL_STATE_STEP1_FLOWDETECT,
		SYS_FREEZE_CYCLE_CTRL_STATE_STEP2_FLOWSET,
		SYS_FREEZE_CYCLE_CTRL_STATE_STEP2_FLOWDETECT,
}SYS_FREEZE_CYCLE_CTRL_STATE_e;
/*
	system constant val
*/
//Probe style
#define SOFTPROBE_TYPE_CODE			0
#define HARDPROBE_TYPE_CODE			1
#define JETPROBE_TYPE_CODE			2
//system set val
typedef enum _SYS_SET_EFFECT_VAL_E{
	SYS_SET_EFFECT_VAL_1 = 0,
	SYS_SET_EFFECT_VAL_2,
	SYS_SET_EFFECT_VAL_3,
	SYS_SET_EFFECT_VAL_4,
}SYS_SET_EFFECT_VAL_e;

#define SYS_SET_EFFECT1_PRESS2_VAL		4000	//40bar
#define SYS_SET_EFFECT2_PRESS2_VAL		5000
#define SYS_SET_EFFECT3_PRESS2_VAL		7500

//surgery state
typedef enum _SYS_SURGERY_STATE_E_{
	SYS_SURGERY_STATE_IDLE,
	SYS_SURGERY_STATE_FREEZE,
}SYS_SURGERY_STATE_e;
/*
	System manage var
*/
typedef struct _SYS_MANAGE_VAR_S_{
	/*******************************************/
	SYS_PRESS_INFOR_s PressInfor;
	int32_t curflowval;
	SYS_KEYSWITCH_INFOR_s keyswinfor;
	/*******************************************/
	SYS_PROBE_INFOR_s ProbeInfor;
	/*******************************************/
	SYS_SET_INFOR_s	sysetval;
	/*******************************************/
	uint32_t disptime;
	/*******************************************/
	uint16_t fvctrlword;
	/*******************************************/
	SYS_SURGERY_STATE_e surstate;
	/*******************************************/
	SYS_FAULT_INFOR_u sysfaultinfor;
	/*******************************************/
	SYS_APL_STATE_e systate;
	/*******************************************/
	WORK_STATE_e workstate;
	/*******************************************/
	SOFTPROBE_TASK_STATE_e sptaskstate;
	/*******************************************/
	HARDPROBE_TASK_STATE_e hptaskstate;
	/*******************************************/
	uint8 value2status;
	uint8 value3status;
	uint8 value4status;
	SYS_FREEZE_CYCLE_CTRL_PARM_s freezectrlparam;
}SYS_MANAGE_VAR_s;
/*
	var declare
*/
extern	volatile SYS_MANAGE_VAR_s	sysvar;

/**
	Function declare
**/
	extern int main(void);
	extern void GosInit(void);
	extern void SYS_APL_EVENT(void *arg);
	extern void hmiCmdProcess(void);
	extern void UpdateUI(void);
#endif

/*
* Copyright (c) 2019
* All rights reserved.
*
* file name:hmi.h
* Author: XiangBin      Version:1.0     Date:2019-05-28
* Description: human machine external interface
* History:
* Date:2019-05-28    Author:XiangBin     Modification: draft version
*/
#ifndef _HMI_H_
#define _HMI_H_

typedef struct _sysKeyParms
{
	uint32 fPSensor1;  //x.xxBar
	uint32 fPSensor2; //x.xxBar
	uint32 fPSensor3; //x.xxBar
	uint32 uiPSensor4;  //xxKpa
	uint32 fFSensor1;	//流量
	
	uint32 fTreatTime;	//时间
	
	uint8 treatMode;	//效能
	
	uint8 probeType;	//探针类型
	
	uint8 sysState;		//空闲/冷冻
	
	uint8 valvePFlow;	//流量阀
	uint8 value2status;
	uint8 value3status;
	uint8 value4status;
	
	uint8_t step1pressure;
	float step1flow;
	uint8 step2pressure;
	float step2flow;
}SysKeyParms_ST;

#define HMI_SYS_TEST_MODE                    0


#define TEXT_TEST_PSENSOR1                   1
#define TEXT_TEST_PSENSOR2                   2
#define TEXT_TEST_PSENSOR3                   3
#define TEXT_TEST_PSENSOR4                   4
#define TEXT_TEST_FSENSOR1                   5
#define TEXT_TEST_TREAT_TIME                 6               
#define TEXT_TEST_TREAT_STATE                7
#define TEXT_TEST_PFLOW_VALVE                8

#define BUTTON_TEST_VALVE1_SW                11
#define BUTTON_TEST_VALVE2_SW                12
#define BUTTON_TEST_VALVE3_SW                13
#define BUTTON_TEST_VALVE4_SW                14             

#define SELECTOR_TEST_PFLOW_VALVE            20
#define SELECTOR_TEST_TREAT_MODE             21             
#define SELECTOR_TEST_PROBE_TYPE             22 
#define SLIDER_TEST_PFLOW_VALVE              23

#define TEXT_TEST_STEP1_PRESSURE             9
#define TEXT_TEST_STEP1_FLOW                 10
#define TEXT_TEST_STEP2_PRESSURE             11
#define TEXT_TEST_STEP2_FLOW                 15

extern SysKeyParms_ST stSysKeyParms;

extern void hmiInit(void);
extern void hmiCmdProcess(void);
extern void SetTextValueInt32(uint16 screen_id, uint16 control_id, int32 value);
extern void setValve(unsigned int valveNum, unsigned int valveValue);


#endif //_HMI_H_

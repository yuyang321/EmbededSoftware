/*
* Copyright (c) 2019
* All rights reserved.
*
* file name:hmi.c
* Author: XiangBin      Version:1.0     Date:2019-05-28
* Description: human machine external interface
* History:
* Date:2019-05-28    Author:XiangBin     Modification: draft version
*/

#include "hmi_driver.h"
//#include "hmi_user_uart.h"
#include "cmd_queue.h"
#include "cmd_process.h"
#include "stdio.h"
#include "hmi.h"

SysKeyParms_ST stSysKeyParms;

float StringToFloat(uint8 *str)
{
	float v = 0;
	sscanf((char *)str,"%f",&v);
	return v;
}

void setValve(unsigned int valveNum, unsigned int valveValue)
{
	switch(valveNum)
	{
		case 2:
			stSysKeyParms.value2status = valveValue;
		break;
		case 3:
			stSysKeyParms.value3status = valveValue;
		break;
		case 4:
			stSysKeyParms.value4status = valveValue;
		break;
	}
}

void SetTextValueInt32(uint16 screen_id, uint16 control_id,int32 value)
{
	char buffer[12] = {0};
	sprintf(buffer,"%ld",value);
	SetTextValue(screen_id,control_id,(uchar *)buffer);
}

void SetTextValueFloat(uint16 screen_id, uint16 control_id, double value)
{
  uchar buffer[12] = { 0 };
  sprintf((char *)buffer, "%.2f", value);
  SetTextValue(screen_id, control_id, buffer);
}

int32 StringToInt32(uint8 *str)
{
	int32 v = 0;
	sscanf((char *)str,"%ld",&v);
	return v;
}

void hmiInit(void)
{
  queue_reset();
	SetTextValueInt32(HMI_SYS_TEST_MODE, TEXT_TEST_PFLOW_VALVE, stSysKeyParms.valvePFlow);
}

void UpdateUI(void)
{
	SetTextValueFloat(HMI_SYS_TEST_MODE, TEXT_TEST_PSENSOR1, (float)stSysKeyParms.fPSensor1/100);
	SetTextValueFloat(HMI_SYS_TEST_MODE, TEXT_TEST_PSENSOR2, (float)stSysKeyParms.fPSensor2/100);
	SetTextValueFloat(HMI_SYS_TEST_MODE, TEXT_TEST_PSENSOR3, (float)stSysKeyParms.fPSensor3/100);
	SetTextValueInt32(HMI_SYS_TEST_MODE, TEXT_TEST_PSENSOR4, stSysKeyParms.uiPSensor4);
	SetTextValueFloat(HMI_SYS_TEST_MODE, TEXT_TEST_FSENSOR1, (float)(stSysKeyParms.fFSensor1)/1000);
	SetTextValueFloat(HMI_SYS_TEST_MODE, TEXT_TEST_TREAT_TIME, (float)stSysKeyParms.fTreatTime/10);
	SetTextValueInt32(HMI_SYS_TEST_MODE, TEXT_TEST_TREAT_STATE, stSysKeyParms.sysState);
}

void hmiCmdProcess(void)
{
  qsize  size = 0;
  static uchar cmd_buffer[CMD_MAX_SIZE];
  size = queue_find_cmd(cmd_buffer, CMD_MAX_SIZE);
  if (size > 0)
  {
		ProcessMessage((PCTRL_MSG)cmd_buffer, size);
  }
}

/*! 
 *  \brief  ��Ϣ�������̣��˴�һ�㲻��Ҫ����
 *  \param msg ��������Ϣ
 *  \param size ��Ϣ����
 */
void ProcessMessage( PCTRL_MSG msg, uint16 size )
{
	uint8 cmd_type = msg->cmd_type;//ָ������
	//uint8 ctrl_msg = msg->ctrl_msg;   //��Ϣ������
	uint8 control_type = msg->control_type;//�ؼ�����
	uint16 screen_id = PTR2U16(&msg->screen_id);//����ID
	uint16 control_id = PTR2U16(&msg->control_id);//�ؼ�ID
	uint32 value = PTR2U32(msg->param);//��ֵ
	//uint8 * cmd_buffer = (uint8 *)&(msg->cmd_type);
	switch(cmd_type)
	{
//	case NOTIFY_SCREEN://�л�����
//		NotifyScreen(screen_id);
//		break;
//	case NOTIFY_TOUCH_PRESS:
//	case NOTIFY_TOUCH_RELEASE:
//		NotifyTouchXY(cmd_buffer[1],PTR2U16(cmd_buffer+2),PTR2U16(cmd_buffer+4));
//		break;	
//	case NOTIFY_WRITE_FLASH_OK:
//		NotifyWriteFlash(1);
//		break;
//	case NOTIFY_WRITE_FLASH_FAILD:
//		NotifyWriteFlash(0);
//		break;
//	case NOTIFY_READ_FLASH_OK:
//		NotifyReadFlash(1,cmd_buffer+2,size-6);//ȥ��֡ͷ֡β
//		break;
//	case NOTIFY_READ_FLASH_FAILD:
//		NotifyReadFlash(0,0,0);
//		break;
	case NOTIFY_CONTROL:
		{
			switch(control_type)
			{
			case kCtrlButton: //��ť�ؼ�
				NotifyButton(screen_id,control_id,msg->param[1]);
				break;
			case kCtrlText://�ı��ؼ�
				NotifyText(screen_id,control_id,msg->param);
				break;
			case kCtrlProgress: //�������ؼ�
				NotifyProgress(screen_id,control_id,value);
				break;
			case kCtrlSlider: //�������ؼ�
				NotifySlider(screen_id,control_id,value);
				break;
			case kCtrlMeter: //�Ǳ�ؼ�
				NotifyMeter(screen_id,control_id,value);
				break;
			case kCtrlMenu://�˵��ؼ�
				NotifyMenu(screen_id,control_id,msg->param[0],msg->param[1]);
				break;
			case kCtrlSelector://ѡ��ؼ�
				NotifySelector(screen_id,control_id,msg->param[0]);
				break;
			case kCtrlRTC://����ʱ�ؼ�
				NotifyTimer(screen_id,control_id);
				break;
			default:
				break;
			}
		}
		break;
	default:
		break;
	}
}

/*! 
 *  \brief  �����л�֪ͨ
 *  \details  ��ǰ����ı�ʱ(�����GetScreen)��ִ�д˺���
 *  \param screen_id ��ǰ����ID
 */
void NotifyScreen(uint16 screen_id)
{
	//TODO: ����û�����
}

/*! 
 *  \brief  ���������¼���Ӧ
 *  \param press 1���´�������3�ɿ�������
 *  \param x x����
 *  \param y y����
 */
void NotifyTouchXY(uint8 press,uint16 x,uint16 y)
{
	//TODO: ����û�����
}

/*! 
 *  \brief  ��ť�ؼ�֪ͨ
 *  \details  ����ť״̬�ı�(�����GetControlValue)ʱ��ִ�д˺���
 *  \param screen_id ����ID
 *  \param control_id �ؼ�ID
 *  \param state ��ť״̬��0����1����
 */
void NotifyButton(uint16 screen_id, uint16 control_id, uint8  state)
{
  if (screen_id == HMI_SYS_TEST_MODE)
  {
    if ((control_id >= BUTTON_TEST_VALVE1_SW) && (control_id <= BUTTON_TEST_VALVE4_SW))
    {
			setValve(control_id-BUTTON_TEST_VALVE1_SW+1, state);
    }
  }	
}

/*! 
 *  \brief  �ı��ؼ�֪ͨ
 *  \details  ���ı�ͨ�����̸���(�����GetControlValue)ʱ��ִ�д˺���
 *  \param screen_id ����ID
 *  \param control_id �ؼ�ID
 *  \param str �ı��ؼ�����
 */
void NotifyText(uint16 screen_id, uint16 control_id, uint8 *str)
{
	switch(screen_id)
  {
    case HMI_SYS_TEST_MODE:
		{
      switch (control_id)
      {			
      case TEXT_TEST_STEP1_PRESSURE:
				stSysKeyParms.step1pressure=StringToInt32(str);
        break;
			
      case TEXT_TEST_STEP1_FLOW:
				stSysKeyParms.step1flow=StringToFloat(str);
        break;

      case TEXT_TEST_STEP2_PRESSURE:
				stSysKeyParms.step2pressure=StringToInt32(str);
        break;
			
      case TEXT_TEST_STEP2_FLOW:
				stSysKeyParms.step2flow=StringToFloat(str);
        break;
			
      default:
        break;
      }
		}
		break;
	}
}

/*! 
 *  \brief  �������ؼ�֪ͨ
 *  \details  ����GetControlValueʱ��ִ�д˺���
 *  \param screen_id ����ID
 *  \param control_id �ؼ�ID
 *  \param value ֵ
 */
void NotifyProgress(uint16 screen_id, uint16 control_id, uint32 value)
{
	//TODO: ����û�����
}

/*! 
 *  \brief  �������ؼ�֪ͨ
 *  \details  ���������ı�(�����GetControlValue)ʱ��ִ�д˺���
 *  \param screen_id ����ID
 *  \param control_id �ؼ�ID
 *  \param value ֵ
 */
void NotifySlider(uint16 screen_id, uint16 control_id, uint32 value)
{
//TODO: ����û�����
	switch(screen_id)
  {
    case HMI_SYS_TEST_MODE:
		{
				switch (control_id)
				{
			case SLIDER_TEST_PFLOW_VALVE:
			 stSysKeyParms.valvePFlow = value;
				SetTextValueInt32(HMI_SYS_TEST_MODE, TEXT_TEST_PFLOW_VALVE, stSysKeyParms.valvePFlow);
				SetSelectorValue(HMI_SYS_TEST_MODE, SELECTOR_TEST_PFLOW_VALVE, stSysKeyParms.valvePFlow);
			 break;
		 
			default:
			 break;
				}
		}
		break;
	}
}

/*! 
 *  \brief  �Ǳ�ؼ�֪ͨ
 *  \details  ����GetControlValueʱ��ִ�д˺���
 *  \param screen_id ����ID
 *  \param control_id �ؼ�ID
 *  \param value ֵ
 */
void NotifyMeter(uint16 screen_id, uint16 control_id, uint32 value)
{
	//TODO: ����û�����
}

/*! 
 *  \brief  �˵��ؼ�֪ͨ
 *  \details  ���˵���»��ɿ�ʱ��ִ�д˺���
 *  \param screen_id ����ID
 *  \param control_id �ؼ�ID
 *  \param item �˵�������
 *  \param state ��ť״̬��0�ɿ���1����
 */
void NotifyMenu(uint16 screen_id, uint16 control_id, uint8  item, uint8  state)
{
	//TODO: ����û�����
}

/*! 
 *  \brief  ѡ��ؼ�֪ͨ
 *  \details  ��ѡ��ؼ��仯ʱ��ִ�д˺���
 *  \param screen_id ����ID
 *  \param control_id �ؼ�ID
 *  \param item ��ǰѡ��
 */
void NotifySelector(uint16 screen_id, uint16 control_id, uint8  item)
{
	//TODO: ����û�����
	switch (screen_id)
  {
  case HMI_SYS_TEST_MODE:
    switch (control_id)
    {
    case SELECTOR_TEST_PROBE_TYPE:
			stSysKeyParms.probeType=item;
      break;

    case SELECTOR_TEST_TREAT_MODE:
			stSysKeyParms.treatMode = item;
      break;
		
		case SELECTOR_TEST_PFLOW_VALVE:
			stSysKeyParms.valvePFlow =item;
      SetTextValueInt32(HMI_SYS_TEST_MODE, TEXT_TEST_PFLOW_VALVE, stSysKeyParms.valvePFlow);
      SetSliderValue(HMI_SYS_TEST_MODE, SLIDER_TEST_PFLOW_VALVE, stSysKeyParms.valvePFlow);
		break;

    default:
      break;
    }
    break;
  }
}

/*! 
 *  \brief  ��ʱ����ʱ֪ͨ����
 *  \param screen_id ����ID
 *  \param control_id �ؼ�ID
 */
void NotifyTimer(uint16 screen_id, uint16 control_id)
{
	//TODO: ����û�����
}

/*! 
 *  \brief  ��ȡ�û�FLASH״̬����
 *  \param status 0ʧ�ܣ�1�ɹ�
 *  \param _data ��������
 *  \param length ���ݳ���
 */
void NotifyReadFlash(uint8 status,uint8 *_data,uint16 length)
{
	//TODO: ����û�����
}

/*! 
 *  \brief  д�û�FLASH״̬����
 *  \param status 0ʧ�ܣ�1�ɹ�
 */
void NotifyWriteFlash(uint8 status)
{
	//TODO: ����û�����
}

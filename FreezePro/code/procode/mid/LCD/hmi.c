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
 *  \brief  消息处理流程，此处一般不需要更改
 *  \param msg 待处理消息
 *  \param size 消息长度
 */
void ProcessMessage( PCTRL_MSG msg, uint16 size )
{
	uint8 cmd_type = msg->cmd_type;//指令类型
	//uint8 ctrl_msg = msg->ctrl_msg;   //消息的类型
	uint8 control_type = msg->control_type;//控件类型
	uint16 screen_id = PTR2U16(&msg->screen_id);//画面ID
	uint16 control_id = PTR2U16(&msg->control_id);//控件ID
	uint32 value = PTR2U32(msg->param);//数值
	//uint8 * cmd_buffer = (uint8 *)&(msg->cmd_type);
	switch(cmd_type)
	{
//	case NOTIFY_SCREEN://切换画面
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
//		NotifyReadFlash(1,cmd_buffer+2,size-6);//去除帧头帧尾
//		break;
//	case NOTIFY_READ_FLASH_FAILD:
//		NotifyReadFlash(0,0,0);
//		break;
	case NOTIFY_CONTROL:
		{
			switch(control_type)
			{
			case kCtrlButton: //按钮控件
				NotifyButton(screen_id,control_id,msg->param[1]);
				break;
			case kCtrlText://文本控件
				NotifyText(screen_id,control_id,msg->param);
				break;
			case kCtrlProgress: //进度条控件
				NotifyProgress(screen_id,control_id,value);
				break;
			case kCtrlSlider: //滑动条控件
				NotifySlider(screen_id,control_id,value);
				break;
			case kCtrlMeter: //仪表控件
				NotifyMeter(screen_id,control_id,value);
				break;
			case kCtrlMenu://菜单控件
				NotifyMenu(screen_id,control_id,msg->param[0],msg->param[1]);
				break;
			case kCtrlSelector://选择控件
				NotifySelector(screen_id,control_id,msg->param[0]);
				break;
			case kCtrlRTC://倒计时控件
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
 *  \brief  画面切换通知
 *  \details  当前画面改变时(或调用GetScreen)，执行此函数
 *  \param screen_id 当前画面ID
 */
void NotifyScreen(uint16 screen_id)
{
	//TODO: 添加用户代码
}

/*! 
 *  \brief  触摸坐标事件响应
 *  \param press 1按下触摸屏，3松开触摸屏
 *  \param x x坐标
 *  \param y y坐标
 */
void NotifyTouchXY(uint8 press,uint16 x,uint16 y)
{
	//TODO: 添加用户代码
}

/*! 
 *  \brief  按钮控件通知
 *  \details  当按钮状态改变(或调用GetControlValue)时，执行此函数
 *  \param screen_id 画面ID
 *  \param control_id 控件ID
 *  \param state 按钮状态：0弹起，1按下
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
 *  \brief  文本控件通知
 *  \details  当文本通过键盘更新(或调用GetControlValue)时，执行此函数
 *  \param screen_id 画面ID
 *  \param control_id 控件ID
 *  \param str 文本控件内容
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
 *  \brief  进度条控件通知
 *  \details  调用GetControlValue时，执行此函数
 *  \param screen_id 画面ID
 *  \param control_id 控件ID
 *  \param value 值
 */
void NotifyProgress(uint16 screen_id, uint16 control_id, uint32 value)
{
	//TODO: 添加用户代码
}

/*! 
 *  \brief  滑动条控件通知
 *  \details  当滑动条改变(或调用GetControlValue)时，执行此函数
 *  \param screen_id 画面ID
 *  \param control_id 控件ID
 *  \param value 值
 */
void NotifySlider(uint16 screen_id, uint16 control_id, uint32 value)
{
//TODO: 添加用户代码
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
 *  \brief  仪表控件通知
 *  \details  调用GetControlValue时，执行此函数
 *  \param screen_id 画面ID
 *  \param control_id 控件ID
 *  \param value 值
 */
void NotifyMeter(uint16 screen_id, uint16 control_id, uint32 value)
{
	//TODO: 添加用户代码
}

/*! 
 *  \brief  菜单控件通知
 *  \details  当菜单项按下或松开时，执行此函数
 *  \param screen_id 画面ID
 *  \param control_id 控件ID
 *  \param item 菜单项索引
 *  \param state 按钮状态：0松开，1按下
 */
void NotifyMenu(uint16 screen_id, uint16 control_id, uint8  item, uint8  state)
{
	//TODO: 添加用户代码
}

/*! 
 *  \brief  选择控件通知
 *  \details  当选择控件变化时，执行此函数
 *  \param screen_id 画面ID
 *  \param control_id 控件ID
 *  \param item 当前选项
 */
void NotifySelector(uint16 screen_id, uint16 control_id, uint8  item)
{
	//TODO: 添加用户代码
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
 *  \brief  定时器超时通知处理
 *  \param screen_id 画面ID
 *  \param control_id 控件ID
 */
void NotifyTimer(uint16 screen_id, uint16 control_id)
{
	//TODO: 添加用户代码
}

/*! 
 *  \brief  读取用户FLASH状态返回
 *  \param status 0失败，1成功
 *  \param _data 返回数据
 *  \param length 数据长度
 */
void NotifyReadFlash(uint8 status,uint8 *_data,uint16 length)
{
	//TODO: 添加用户代码
}

/*! 
 *  \brief  写用户FLASH状态返回
 *  \param status 0失败，1成功
 */
void NotifyWriteFlash(uint8 status)
{
	//TODO: 添加用户代码
}

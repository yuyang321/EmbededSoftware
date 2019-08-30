#include "bsp.h"
#include "guser.h"
#include "cJSON.h"

//uint8_t GsmInitflag = GSM_INIT_ENABLE_BYTE;
uint8_t RxNodeASKflagA = 0;
uint8_t GsmInitflag = 0;
/******************************************************************************/
//all app state machine
	//GSM Init Sequence
	APL_INIT_STATE_e aplinitstate;		
	//App Work Sequence
	APL_STATE_MACHINE_e aplstate =  APL_STATE_PWON_LOW_2SECSOND;
	//After Init succeed, the app work sequence
	APL_WORK_STATE_e aplworkstate = APL_STATE_WORK_IDLE;
	//Publish Infor Sequence
	APL_STATE_PUBLISH_e aplpubstate = APL_STATE_PUBLISH_TOPIC;
	//Subscribe Infor Sequence
	SUB_INFOR_RX_STATE_e subrxstate = SUB_INFOR_WAIT_STATE;
/******************************************************************************/
//USART Buffer and fifo
static uint8_t GsmRxBuf[USART_GSM_RxBUFF_SIZE];
static uint8_t GsmTxBuf[USART_GSM_TxBUFF_SIZE];
FIFO_t GsmRxFifo;
FIFO_t GsmTxFifo;
/******************************************************************************/
//the string which need to compare with RxString
static uint8_t cmpbuffa[sizeof(AT_READY)-1];
FIFO_t cmpbuffaFifo;

static uint8_t cmpbuffb[sizeof(AT_GSN_RESPONSE)-1];
FIFO_t cmpbuffbFifo;

static uint8_t cmpbuffc[sizeof(MQTT_READY)-1];
FIFO_t cmpbuffcFifo;

static uint8_t cmpbuffd[sizeof(MQTT_CONNECT)-1];
FIFO_t cmpbuffdFifo;

static uint8_t cmpbuffe[sizeof(MQTT_PUBLISH_OK_RESPONSE)-1];
FIFO_t cmpbuffeFifo;

static uint8_t cmpbufff[sizeof(MQTT_PUBLISH_FAIL_RESPONSE)-1];
FIFO_t cmpbufffFifo;
//Server heart deteck
//static uint8_t cmpbugff[sizeof(SERVER_HEART_RESPONSE)-1];
//FIFO_t cmpbuffgFifo;
//Server heart deteck
static uint8_t cmpbuffh[sizeof(SUB_SERVER_CMD_INFORA_HEAD)-1];
FIFO_t cmpbuffhFifo;
/*****************************************************************************/
//Publish acquired Infor
SENSE_ACQ_SQUENCE_t vacqdata;
/*****************************************************************************/
/**************************************************************************************/
//Subscribe Infor after parase
OTA_DATA_t otadata;
//Subscribe Infor original string
char otastring[]="{\"period\":12,\"order\":0}";
//Module ID
uint8_t moduleid[SN_SIZE];
//led toggle timer indicating GSM Module work 
static SOFT_TIMER_t sledtimer;
static void gsledtoggle(void *arg)
{
	STATUS_LED_TOGGLE;
}
//AT Test no repeat timer using for GSM Init AT Test
static SOFT_TIMER_t atresponsetimer;
static void gATtestimer(void *arg)
{
	SendUsartString((uint8_t *)AT_GSN,sizeof(AT_GSN)-1,BLOCK_TRANS_MODE);
}
//GSM Module Init Monitor no repeat Timer using for Module and App reset
static SOFT_TIMER_t GsmInitTimer;
static void gGsmInitMonitor(void *arg)
{
	GTimerStop(&atresponsetimer);
	GTimerStop(&sledtimer);
	//recover init state
	aplinitstate = 0;
	aplstate = APL_STATE_PWON_LOW_2SECSOND;
}
//Publish Monitor no repeat timer using for Publish Reset
static SOFT_TIMER_t PubMonitorTimer;
static void gPubMonitorTimer(void *arg)
{
	aplpubstate = APL_STATE_PUBLISH_RESET;
}
//Subscribe Monitor no repeat timer using Subscribe Reset
static SOFT_TIMER_t SubMonitorTimer;
static void gSubMonitorTimer(void *arg)
{
	subrxstate = SUB_INFOR_RX_RESET;
}
//node analog channel acquire
static SOFT_TIMER_t AnalogAcqTimer;
static void gAnalogAcqTimer(void *arg)
{
  uint8_t loop;
	if( !adconvertflag )
		return;
	adconvertflag = 0;
	#if(ADC_FILT_MODE == INTEGER_WINDOW_FILT)
		if(adcbufcnt >= ADC_BUFF_NUM)
		{
			adcbufcnt = 0;
			//Filt Arithmetic
			for(loop = 0;loop < ADC_CHANNEL_NUM;loop++)
				gLPfiltOneOrder(ADChannelCurVal+loop,ADChannellastVal+loop,ADChannelValBuf[loop],ADC_BUFF_NUM,ADC_FILT_ARG_VAL);
		}
	#elif(ADC_FILT_MODE == MOVING_WINDOW_FILT)
		//Filt Arithmetic
		for(loop = 0;loop < ADC_CHANNEL_NUM;loop++)
				gLPfiltOneOrder(ADChannelCurVal+loop,ADChannellastVal+loop,ADChannelValBuf[loop],ADC_BUFF_NUM,ADC_FILT_ARG_VAL);
		adcbufcnt%=ADC_BUFF_NUM;
	#endif
	ADC_Cmd(ADC1,ENABLE);
	ADC_SoftwareStartConvCmd(ADC_USED,ENABLE);
}
//node phsical channel scan,and publish server cycle timer
static SOFT_TIMER_t ChScanTimer;
static void gchscantimer(void *arg)
{
	uint8_t loop;
	uint8_t digtemp;
	static SENSE_ACQ_SQUENCE_t vacqdatatemp;
	static uint16_t repeatemp = 0;
	//update digital data
	for(loop = 0;loop < DIGITAL_CHANNEL;loop++)
	{
		digtemp = (!GET_CHX_STATUS(loop)) ? 0 : 1;
		delay_nms(5);
		if( GET_CHX_STATUS(loop) == digtemp )
			vacqdata.digitalval[loop] = digtemp;
	}
	for(loop = 0;loop < ANALOG_CHANNEL;loop++)
	{
		vacqdata.analogval[loop] = *(ADChannelCurVal+loop) *3.3 * ANALOG_AMPLIFY / 4096;
	}
	if(otadata.repeat != 0)
	{
		//strncmp error
		if( gmemcmp((uint8_t *)&vacqdata,(uint8_t *)&vacqdatatemp,sizeof(SENSE_ACQ_SQUENCE_t)) )
		{
			repeatemp++;
			if(repeatemp <= otadata.repeat )
				return;
		}
		else
			gmemcpy((uint8_t *)&vacqdata,(uint8_t *)&vacqdatatemp,sizeof(SENSE_ACQ_SQUENCE_t));
	}
	repeatemp = 0;
	vacqdata.sn++;
	vacqdatatemp.sn = vacqdata.sn;
	vacqdatatemp.code = vacqdata.code;
	//update analog data
	aplworkstate = APL_STATE_PUBLISH_INFOR;
}
//监测节点与云端的连接是否一直存在
//static SOFT_TIMER_t PServHeartTimer;
//static void gServHeartTimer(void *arg)
//{
//	//System Reset
//	
//}

/**************************************************************************************/
//组建JSON
uint8_t gCreateTransJsonObject(cJSON *root, PUB_INFOR_NAME_e pubinforname)
{
	 if(!root)
		 return 0;
	 switch(pubinforname)
	 {
		 case PUB_INFOR_SENSOR_CYCLE_DATA:
			 cJSON_AddItemToObject(root, "sn", cJSON_CreateNumber(vacqdata.sn));
			 cJSON_AddItemToObject(root, "code", cJSON_CreateNumber(vacqdata.code));
			 cJSON_AddItemToObject(root, "analog1", cJSON_CreateNumber(vacqdata.analogval[0]));
			 cJSON_AddItemToObject(root, "analog2", cJSON_CreateNumber(vacqdata.analogval[1]));
			 cJSON_AddItemToObject(root, "analog3", cJSON_CreateNumber(vacqdata.analogval[2]));
			 cJSON_AddItemToObject(root, "analog4", cJSON_CreateNumber(vacqdata.analogval[3]));
			 cJSON_AddItemToObject(root, "digital1", cJSON_CreateBool(vacqdata.digitalval[0]));
			 cJSON_AddItemToObject(root, "digital2", cJSON_CreateBool(vacqdata.digitalval[1]));
			 cJSON_AddItemToObject(root, "digital3", cJSON_CreateBool(vacqdata.digitalval[2]));
			 cJSON_AddItemToObject(root, "digital4", cJSON_CreateBool(vacqdata.digitalval[3]));
			 cJSON_AddItemToObject(root, "digital5", cJSON_CreateBool(vacqdata.digitalval[4]));
			 cJSON_AddItemToObject(root, "digital6", cJSON_CreateBool(vacqdata.digitalval[5]));
			 cJSON_AddItemToObject(root, "digital7", cJSON_CreateBool(vacqdata.digitalval[6]));
			 cJSON_AddItemToObject(root, "digital8", cJSON_CreateBool(vacqdata.digitalval[7]));
		 break;
		 case PUB_INFOR_OTA_CMD_RESPONSE:
			 //cJSON_AddItemToObject(root, "sn", cJSON_CreateNumber(otadata.sn));
			 cJSON_AddItemToObject(root, "sn", cJSON_CreateString((char *)(otadata.snp)));
			 cJSON_AddItemToObject(root, "period", cJSON_CreateNumber(otadata.period));
			 cJSON_AddItemToObject(root, "repeat", cJSON_CreateNumber(otadata.repeat));
			 cJSON_AddItemToObject(root, "version",  cJSON_CreateString((char *)FIRM_VERSION));
		 break;
	 }
	 return 1;
}

//解析JSON
uint8_t gParseTransJsonObject(OTA_DATA_t *pdata,char *jsonstring)
{
	if(!(pdata && jsonstring))
		return 0;
	cJSON *root;
	root = cJSON_Parse(jsonstring);
	if(!root)
		return 0;
	//pdata->sn = cJSON_GetObjectItem(root,"sn")->valueint;
	gmemset(pdata->snp,0,sizeof(pdata->snp));
	gmemcpy((uint8_t *)(cJSON_GetObjectItem(root,"sn")->valuestring),pdata->snp,strlen( cJSON_GetObjectItem(root,"sn")->valuestring ));
	if((pdata->order = cJSON_GetObjectItem(root,"order")->valueint) == 1)
	{
		pdata->period = cJSON_GetObjectItem(root,"period")->valueint;
		pdata->period = pdata->period > SENSOR_DATA_TRANS_MAX_PERIOD ? 600 : pdata->period;
		pdata->period = pdata->period < SENSOR_DATA_TRANS_MIN_PERIOD ? 12 : pdata->period;
		pdata->repeat = cJSON_GetObjectItem(root,"repeat")->valueint;
		pdata->repeat = pdata->repeat > SENSOR_DATA_REPEAT_MAX_COUNT ? SENSOR_DATA_REPEAT_MAX_COUNT : pdata->repeat;
	}
	cJSON_Delete(root);
	return 1;
}
MQTT_INIT_CONFIG_SEQUENCE_t gsmconfigseq;
//uint8_t testbuf1[100];
//uint8_t testbuf2[100];
//GSM Init Config
uint8_t gConfigGsmModule(uint8_t rxdata)
{
	if( !( gsmconfigseq.autosubbuf = (uint8_t *)malloc(sizeof(AT_AUTOSUB)-1)) )
		return 0;
	gmemcpy((uint8_t *)AT_AUTOSUB,gsmconfigseq.autosubbuf,sizeof(AT_AUTOSUB)-1);
	gmemcpy(moduleid,gsmconfigseq.autosubbuf + AT_AUTOSUB_SHIFT_BIT,SN_SIZE - 1);
	//gmemcpy(gsmconfigseq.autosubbuf,testbuf1,sizeof(AT_AUTOSUB)-1);
	if( !( gsmconfigseq.willbuf = (uint8_t *)malloc(sizeof(AT_WILL)-1)) )
		return 0;
	gmemcpy((uint8_t *)AT_WILL,gsmconfigseq.willbuf,sizeof(AT_WILL)-1);
	gmemcpy(moduleid,gsmconfigseq.willbuf + AT_WILL_SHIFT_BIT,SN_SIZE - 1);
	//gmemcpy(gsmconfigseq.willbuf,testbuf2,sizeof(AT_WILL)-1);
	//可考虑单独的配置过程，用于GSM模块首次配置
	SendUsartString((uint8_t *)AT_MQTTVER,sizeof(AT_MQTTVER)-1,BLOCK_TRANS_MODE);
	delay_nms(500);
	SendUsartString((uint8_t *)AT_MQTTMODE,sizeof(AT_MQTTMODE)-1,BLOCK_TRANS_MODE);
	delay_nms(500);
	SendUsartString((uint8_t *)AT_IPPORT,sizeof(AT_IPPORT)-1,BLOCK_TRANS_MODE);
	delay_nms(500);
	SendUsartString((uint8_t *)AT_USERPWD,sizeof(AT_USERPWD)-1,BLOCK_TRANS_MODE);
	delay_nms(500);
	SendUsartString(gsmconfigseq.autosubbuf,sizeof(AT_AUTOSUB)-1,BLOCK_TRANS_MODE);
	delay_nms(500);
	SendUsartString(gsmconfigseq.willbuf,sizeof(AT_WILL)-1,BLOCK_TRANS_MODE);
	delay_nms(500);
	SendUsartString((uint8_t *)AT_W,sizeof(AT_W)-1,BLOCK_TRANS_MODE);
	delay_nms(500);
	SendUsartString((uint8_t *)AT_CFUN,sizeof(AT_CFUN)-1,BLOCK_TRANS_MODE);
	delay_nms(500);
	free(gsmconfigseq.autosubbuf);
	free(gsmconfigseq.willbuf);
	return 1;
}
MQTT_PUBLISH_TOPIC_SEQUENCE_t MqttPublist;
MQTT_SUB_TOPIC_SEQUENCE_t MqttSubList;
//Publish topic list Init
static uint8_t gPubSubListInit(void)
{
	if( !( MqttPublist.pubdatabuf = (uint8_t *)malloc(sizeof(TOPIC_DATA_TRANS_HEAD)-1)) )
		return 0;
	gmemcpy((uint8_t *)TOPIC_DATA_TRANS_HEAD,MqttPublist.pubdatabuf,sizeof(TOPIC_DATA_TRANS_HEAD)-1);
	if( !(MqttPublist.pubotabuf = (uint8_t *)malloc(sizeof(TOPIC_SERVER_CMD_HEAD)-1)) )
		return 0;
	gmemcpy((uint8_t *)TOPIC_SERVER_CMD_HEAD,MqttPublist.pubotabuf,sizeof(TOPIC_SERVER_CMD_HEAD	)-1);
	if( !(MqttSubList.subotabuff = (uint8_t *)malloc(sizeof(SUB_SERVER_CMD_INFORA_HEAD)-1)) )
		return 0;
	gmemcpy((uint8_t *)SUB_SERVER_CMD_INFORA_HEAD,MqttSubList.subotabuff,sizeof( SUB_SERVER_CMD_INFORA_HEAD	)-1);
	return 1;
}
/***************************************************************************************************/
/**
	system event handle:
		HAL_EXIT_EVENT
		HAL_GSMTX_EVENT
		HAL_FAULT_EVENT
		HAL_TIMER_EVENT
		HAL_GSMRX_EVENT
**/
void HAL_EXIT_EVENT(void *arg)
{

}
void HAL_GSMTX_EVENT(void *arg)
{
	uint8_t txdata;	
	GFIFOPop(&GsmTxFifo,&txdata);
	USART_SendData(USART_GSM, (uint16_t)txdata );
}
void HAL_FAULT_EVENT(void *arg)
{
	switch(HalFaultCode)
	{
		case USART_GSM_FIFO_FULL:
		break;
		case USART_GSM_FIFO_EMPTY:
			USART_ITConfig(USART_GSM, USART_IT_TC , DISABLE);
		break;
		default:;
	}
}
void HAL_TIMER_EVENT(void *arg)
{
	GTimerLoop();
}

//USART RX Handle
void HAL_GSMRX_EVENT(void *arg)
{
	uint8_t rxdata;
	static uint8_t rxindex = 0;
	static uint16_t sublength = 0;
	static uint8_t sublengthstring[4];
	static uint8_t *otasdynamictring;
	while(GFIFOPop(&GsmRxFifo,&rxdata) == EXE_FINISH)
	{
		switch(aplinitstate)
		{
			case APL_STATE_WAIT_AT_READY:
				if( gfifostrncmp(&cmpbuffaFifo,(uint8_t *)AT_READY,sizeof(AT_READY)-1,rxdata) )
				{
					SendUsartString((uint8_t *)AT_GSN,sizeof(AT_GSN)-1,BLOCK_TRANS_MODE);
					GTimerNodeInit(&atresponsetimer,gATtestimer, GSM_AT_RESPONSE_MAX_DELAY_TIME , GSM_AT_RESPONSE_MAX_DELAY_TIME );
					GTimerStart(&atresponsetimer);
					aplinitstate = APL_STATE_WAIT_AT_GSN_RESPONSE;
				}
			break;
			case APL_STATE_WAIT_AT_GSN_RESPONSE:
				if(gfifostrncmp(&cmpbuffbFifo,(uint8_t *)AT_GSN_RESPONSE,sizeof(AT_GSN_RESPONSE)-1,rxdata))	
				{
					GTimerStop(&atresponsetimer);
					aplinitstate = APL_STATE_GET_GSM_ID;
					rxindex = 0;
				}
			break;	
			case APL_STATE_GET_GSM_ID:
				moduleid[rxindex++] = rxdata;
				if(rxindex >= (SN_SIZE - 1))
				{
					//Merge GSM ID into related string
					gmemcpy(moduleid,MqttPublist.pubdatabuf + TOPIC_DATA_TRANS_SHIFT_BIT, SN_SIZE - 1);
					gmemcpy(moduleid,MqttPublist.pubotabuf + TOPIC_SERVER_CMD_SHIFT_BIT, SN_SIZE - 1);
					gmemcpy(moduleid,MqttSubList.subotabuff+ SUB_SERVER_CMD_INFORA_SHIFT_BIT,SN_SIZE - 1);
					if( GSM_INIT_ENABLE_BYTE == GsmInitflag)
					{
						aplinitstate = APL_STATE_CONFIG_GSM;
					}
					else
					{
						aplinitstate = APL_STATE_WAIT_MQTT_READY;
					}
				}
			break;
			case APL_STATE_CONFIG_GSM:
				if(gConfigGsmModule(rxdata))
				{			
					GsmInitflag = 0;
//					GTimerStop(&GsmInitTimer);
//					GTimerStop(&sledtimer);
//					aplinitstate = 0;
//					aplstate = APL_STATE_PWON_LOW_2SECSOND;
					aplinitstate = APL_STATE_WAIT_MQTT_READY;
				}
			break;
			case APL_STATE_WAIT_MQTT_READY:
				if( gfifostrncmp(&cmpbuffcFifo,(uint8_t *)MQTT_READY,sizeof((uint8_t *)MQTT_READY)-1,rxdata) )
				{
					aplinitstate = APL_STATE_WAIT_MQTT_CONNECT;
				}
			break;	
			case APL_STATE_WAIT_MQTT_CONNECT:
				if( gfifostrncmp(&cmpbuffdFifo,(uint8_t *)MQTT_CONNECT,sizeof((uint8_t *)MQTT_CONNECT)-1,rxdata) )
				{
					GTimerStop(&sledtimer);
					GTimerNodeInit(&sledtimer,gsledtoggle,LED_TOGGLE_MQTT_CONNECT_OK_INTERVAL_TIME,LED_TOGGLE_MQTT_CONNECT_OK_INTERVAL_TIME);
					GTimerStart(&sledtimer);
					aplinitstate = APL_STATE_USART_WORK;
				}
			break;
			case APL_STATE_USART_WORK:
				switch(aplworkstate)
				{
					case APL_STATE_PUBLISH_INFOR:
						switch(aplpubstate)
						{
							case APL_STATE_PUBLISH_WAIT_TOPIC_RESPONSE:
								//check the server response of publishing : OK or Fail
								if( gfifostrncmp(&cmpbuffeFifo,(uint8_t *)MQTT_PUBLISH_OK_RESPONSE,strlen((char *)MQTT_PUBLISH_OK_RESPONSE),rxdata) )
								{
									aplpubstate = APL_STATE_PUBLISH_TOPIC_RESPONSE_OK;
								}
								else if(gfifostrncmp(&cmpbufffFifo,(uint8_t *)MQTT_PUBLISH_FAIL_RESPONSE,strlen((char *)MQTT_PUBLISH_FAIL_RESPONSE),rxdata))
								{
									aplpubstate = APL_STATE_PUBLISH_TOPIC_RESPONSE_FAIL;
								}
							break;
							case APL_STATE_PUBLISH_WAIT_DATA_RESPONSE:
								if( gfifostrncmp(&cmpbuffbFifo,(uint8_t *)OK,strlen((char *)OK),rxdata) )
								{
									aplpubstate = APL_STATE_PUBLISH_DATA_RESPONSE_OK;
								}
							break;
							default:
								__NOP();
							break;
					}
				break;	
				case APL_STATE_WORK_IDLE:
					__NOP;
				break;
				}
				switch(subrxstate)
				{
					case SUB_INFOR_WAIT_STATE:
						if( gfifostrncmp(&cmpbuffhFifo,(uint8_t *)(MqttSubList.subotabuff),strlen((char *)(MqttSubList.subotabuff)),rxdata) )
						{
							subrxstate = SUB_INFOR_RX_DATA_LENGTH;	
							rxindex = 0;
							gmemset(sublengthstring,0,sizeof(sublengthstring));
							//Create Subscribe Monitor Timer
							GTimerNodeInit(&SubMonitorTimer,gSubMonitorTimer,GSM_SUBSCRIBE_MAX_INTERVAL_TIME,0);
							GTimerStart(&SubMonitorTimer);
						}
					break;
					case SUB_INFOR_RX_DATA_LENGTH:
							if(rxdata >= '0' && rxdata <= '9')
								sublengthstring[rxindex++] = rxdata;
							switch(rxdata)
							{
								case '\r':
										if(sublength == 0)
										{
											sublength =  atoi((char *)sublengthstring);
											if(sublength > SUB_TOPIC_DATA_MAX_LENGTH)
												subrxstate = SUB_INFOR_RX_RESET;
											otasdynamictring = (uint8_t *)malloc(sublength);
											gmemset(otasdynamictring,0,sublength);
										}
										else
										{
											subrxstate = SUB_INFOR_RX_RESET;
										}

								break;
								case '\n':
										rxindex = 0;
										subrxstate = SUB_INFOR_RX_STATE;
								break;
							}	
					break;
					case SUB_INFOR_RX_STATE:
							//otastring dynamic created
							otasdynamictring[rxindex++] = rxdata;
							if(rxindex >= sublength)
							{
								rxindex = 0;	//rxindex must be equal to 0
								if( gParseTransJsonObject(&otadata,(char *)otasdynamictring) )
								{								
									RxNodeASKflagA = 1;
									if( aplworkstate == APL_STATE_WORK_IDLE )
									{
											//caution:GSM will reset when GSM receive data quickly! 
											aplworkstate = APL_STATE_PUBLISH_INFOR;
									}
								}	
								//acclerate publish response:apl task prior to timer task
								subrxstate = SUB_INFOR_RX_RESET;
							}
					break;	
				}
				if( subrxstate == SUB_INFOR_RX_RESET )
				{
							GTimerStop(&SubMonitorTimer);
							sublength = 0;
							free(otasdynamictring);
							subrxstate = SUB_INFOR_WAIT_STATE;
				}
			break;
		}
	}
}
/**************************************************************************************/
//system init
void GosInit(void)
{
	//timer init
	InitTimerlist();
	//Init Usart pipe
	GFIFOInit(&GsmRxFifo,GsmRxBuf,sizeof(GsmRxBuf));
	GFIFOInit(&GsmTxFifo,GsmTxBuf,sizeof(GsmTxBuf));
	//Init GSM Pipe
	GFIFOInit(&cmpbuffaFifo,cmpbuffa,sizeof(cmpbuffa));
	GFIFOInit(&cmpbuffbFifo,cmpbuffb,sizeof(cmpbuffb));
	GFIFOInit(&cmpbuffcFifo,cmpbuffc,sizeof(cmpbuffc));
	GFIFOInit(&cmpbuffdFifo,cmpbuffd,sizeof(cmpbuffd));
	GFIFOInit(&cmpbuffeFifo,cmpbuffe,sizeof(cmpbuffe));
	GFIFOInit(&cmpbufffFifo,cmpbufff,sizeof(cmpbufff));
	GFIFOInit(&cmpbuffhFifo,cmpbuffh,sizeof(cmpbuffh));
	//start app layer
	SET_EVENT( SYS_APL_EVENT_E );
	//Init Sensor Acquired Data
	gmemset((uint8_t *)&vacqdata,0,sizeof(SENSE_ACQ_SQUENCE_t));
	gmemset((uint8_t *)&otadata,0,sizeof(OTA_DATA_t));
	otadata.period = SENSOR_DATA_TRANS_MIN_PERIOD;
	gPubSubListInit();
}
/**************************************************************************************/
//App Master state
void SYS_APL_EVENT(void *arg)
{
	 static cJSON *root;
	 static 	char *pubptr;
	static uint8_t pubcnt = 0;
	char strcount[3];
	switch(aplstate)
	{
		case APL_STATE_PWON_LOW_2SECSOND:
//			if( GSM_INIT_ENABLE_BYTE == GsmInitflag)
//			{
				GSM_ENABLE_RESET;
				delay_nms(100);
				GSM_DISABLE_RESET;
				delay_nms(100);
				GSM_PWON_PULL_UP;
				delay_nms(100);
				GSM_PWON_PULL_DOWN;
				delay_nms(2500);	
//			}
			//led indicate that GSM Module is registering
			GTimerNodeInit(&sledtimer,gsledtoggle, LED_TOGGLE_INIT_GSM_INTERVAL_TIME , LED_TOGGLE_INIT_GSM_INTERVAL_TIME );
			GTimerStart(&sledtimer);
			//GSM Init monitor timer
			GTimerNodeInit(&GsmInitTimer,gGsmInitMonitor, GSM_INIT_MAX_INTERVAL_TIME , 0);
			GTimerStart(&GsmInitTimer);
			aplstate = APL_STATE_SYSTEM_INIT;
			aplinitstate = APL_STATE_WAIT_AT_READY;
		break;
		case APL_STATE_SYSTEM_INIT:
			if(aplinitstate == APL_STATE_USART_WORK)
			{
				GTimerStop(&GsmInitTimer);
				//CH0:7 Input Detect
				GTimerNodeInit(&AnalogAcqTimer,gAnalogAcqTimer, ANALOG_CHANNEL_ACQ_INTERVAL_TIME, ANALOG_CHANNEL_ACQ_INTERVAL_TIME);
				GTimerStart(&AnalogAcqTimer);	
				GTimerNodeInit(&ChScanTimer,gchscantimer, ACQ_CHANNEL_SCAN_INTERVAL_TIME, ACQ_CHANNEL_SCAN_INTERVAL_TIME);
				GTimerStart(&ChScanTimer);
				aplstate = APL_STATE_NORMAL_WORK;
			}			
		break;
		case APL_STATE_NORMAL_WORK:
			switch(aplworkstate)
			{
				case APL_STATE_PUBLISH_INFOR:
					switch(aplpubstate)
					{
						case APL_STATE_PUBLISH_TOPIC:
							if( !pubcnt )
							{
									GTimerNodeInit(&PubMonitorTimer,gPubMonitorTimer, GSM_PUBLISH_MAX_INTERVAL_TIME, 0);
									GTimerStart(&PubMonitorTimer);
							}
							if( pubcnt++ > PUBLISH_TRY_MAX_CNT)
							{
								aplpubstate = APL_STATE_PUBLISH_RESET;
								break;
							}
							root = cJSON_CreateObject();
							uint8_t *topicbuf;
							if(1 == RxNodeASKflagA)
							{
								RxNodeASKflagA = 0;
								GTimerStop(&ChScanTimer);
								GTimerNodeInit(&ChScanTimer,gchscantimer, otadata.period*1000, otadata.period*1000);
								GTimerStart(&ChScanTimer);
								gCreateTransJsonObject(root,PUB_INFOR_OTA_CMD_RESPONSE);
								topicbuf = MqttPublist.pubotabuf;
							}
							else
							{
								gCreateTransJsonObject(root,PUB_INFOR_SENSOR_CYCLE_DATA);
								topicbuf = MqttPublist.pubdatabuf;
							}
							pubptr = cJSON_PrintUnformatted(root);
							cJSON_Delete(root);
							#ifdef _DEBUG_
								printf("%s\n",ptr);
							#endif
						 //AT+PUBLISH=topic,data length,QS
						 //data
						 sprintf(strcount, "%d", (int)strlen(pubptr));
						 SendUsartString((uint8_t *)AT_PUBLISH,sizeof(AT_PUBLISH)-1,BLOCK_TRANS_MODE);
						 SendUsartString((uint8_t *)topicbuf,strlen((char *)topicbuf),BLOCK_TRANS_MODE);
						 SendUsartString((uint8_t *)strcount,sizeof(strcount),BLOCK_TRANS_MODE);
						 SendUsartString((uint8_t *)MQTT_SIG_GRADE,sizeof(MQTT_SIG_GRADE)-1,BLOCK_TRANS_MODE);
						 aplpubstate = APL_STATE_PUBLISH_WAIT_TOPIC_RESPONSE;
						break;
						case APL_STATE_PUBLISH_TOPIC_RESPONSE_FAIL:
							aplpubstate = APL_STATE_PUBLISH_TOPIC;
						  free(pubptr);
						break;
						case APL_STATE_PUBLISH_TOPIC_RESPONSE_OK:
							aplpubstate = APL_STATE_PUBLISH_DATA;
						break;
						case APL_STATE_PUBLISH_DATA:
							SendUsartString((uint8_t *)pubptr,strlen(pubptr)/2,BLOCK_TRANS_MODE);
							SendUsartString((uint8_t *)pubptr+strlen(pubptr)/2,strlen(pubptr) - strlen(pubptr)/2,BLOCK_TRANS_MODE);
							SendUsartString((uint8_t *)"\r\n",2,BLOCK_TRANS_MODE);
							aplpubstate = APL_STATE_PUBLISH_WAIT_DATA_RESPONSE;
						break;
						case APL_STATE_PUBLISH_DATA_RESPONSE_OK:
							
							aplpubstate = APL_STATE_PUBLISH_RESET;
						break;
						case APL_STATE_PUBLISH_DATA_RESPONSE_FAIL:
							aplpubstate = APL_STATE_PUBLISH_RESET;
						break;
						case APL_STATE_PUBLISH_WAIT_TOPIC_RESPONSE:
							__NOP;
						break;
						case APL_STATE_PUBLISH_WAIT_DATA_RESPONSE:
							__NOP;
						break;
						case APL_STATE_PUBLISH_RESET:
							aplworkstate = APL_STATE_WORK_IDLE;
							aplpubstate = APL_STATE_PUBLISH_TOPIC;
							pubcnt = 0;
							GTimerStop(&PubMonitorTimer);
							free(pubptr);
							//cJSON_Delete(root);
						break;
						default:
							__NOP();
						break;
					}	
				break;
				case APL_STATE_WORK_IDLE:
					__NOP;
				break;
			}
		break;
	}
	SET_EVENT( SYS_APL_EVENT_E );
}

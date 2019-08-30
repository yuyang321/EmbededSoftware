
#ifndef _GUSER_H_
#define _GUSER_H_

	#include "..\gos\glib.h"
	
	//#define _DEBUG_
/**
	USART FIFO  MACRO/Variable
**/
	#define USART_GSM_RxBUFF_SIZE		500
	#define USART_GSM_TxBUFF_SIZE		250
	extern FIFO_t GsmRxFifo;
	extern FIFO_t GsmTxFifo;
/*
	Timer Task Parm
*/
#define LED_TOGGLE_INIT_GSM_INTERVAL_TIME					100
#define LED_TOGGLE_MQTT_CONNECT_OK_INTERVAL_TIME	500
#define ACQ_CHANNEL_SCAN_INTERVAL_TIME						12000
#define ANALOG_CHANNEL_ACQ_INTERVAL_TIME					80
#define GSM_AT_RESPONSE_MAX_DELAY_TIME						3000
#define GSM_INIT_MAX_INTERVAL_TIME								120000
#define GSM_PUBLISH_MAX_INTERVAL_TIME							5000
#define GSM_SUBSCRIBE_MAX_INTERVAL_TIME						5000
/*
	App Struct
*/
	// Master StateMachine
	typedef enum _APL_STATE_MACHINE_E_{
		APL_STATE_PWON_LOW_2SECSOND =1,
		APL_STATE_SYSTEM_INIT,
		APL_STATE_NORMAL_WORK,
	}APL_STATE_MACHINE_e;
	//GSM Init Struct Sequence,and APL_STATE_USART_WORK for USART Recieve After Module Init OK
	typedef enum _APL_INIT_STATE_E_{
		APL_STATE_WAIT_AT_READY = 1,
		APL_STATE_WAIT_AT_GSN_RESPONSE,
		APL_STATE_GET_GSM_ID,
		APL_STATE_CONFIG_GSM,
		APL_STATE_WAIT_MQTT_READY,
		APL_STATE_WAIT_MQTT_CONNECT,
		APL_STATE_USART_WORK,
	}APL_INIT_STATE_e;
	//the substate of APL_STATE_NORMAL_WORK
	typedef enum _APL_WORK_STATE_E_{
		APL_STATE_PUBLISH_INFOR = 1,
		APL_STATE_WORK_IDLE,
	}APL_WORK_STATE_e;
	//The Process Struct of GSM Module Publish infor
	typedef enum _APL_STATE_PUBLISH_E_{
		APL_STATE_PUBLISH_TOPIC = 1,
		APL_STATE_PUBLISH_TOPIC_RESPONSE_OK,
		APL_STATE_PUBLISH_TOPIC_RESPONSE_FAIL,
		APL_STATE_PUBLISH_DATA,
		APL_STATE_PUBLISH_DATA_RESPONSE_OK,
		APL_STATE_PUBLISH_DATA_RESPONSE_FAIL,
		APL_STATE_PUBLISH_WAIT_TOPIC_RESPONSE,
		APL_STATE_PUBLISH_WAIT_DATA_RESPONSE,
		APL_STATE_PUBLISH_RESET,
	}APL_STATE_PUBLISH_e;
//Infor Recieve state
	typedef enum _SUB_INFOR_RX_STATE_E_{
	SUB_INFOR_WAIT_STATE = 1,
	SUB_INFOR_RX_STATE,
	SUB_INFOR_RX_DATA_LENGTH,
	SUB_INFOR_RX_RESET,
	}SUB_INFOR_RX_STATE_e;
//Publish Infor Enum
	typedef enum _PUB_INFOR_NAME_E_{
		PUB_INFOR_SENSOR_CYCLE_DATA = 1,
		PUB_INFOR_OTA_CMD_RESPONSE,
	}PUB_INFOR_NAME_e;
	//MQTT Publish head string list
	typedef struct __MQTT_PUBLISH_TOPIC_SEQUENCE_T_{
	uint8_t *pubdatabuf;
	uint8_t *pubotabuf;
	//......
}MQTT_PUBLISH_TOPIC_SEQUENCE_t;
		typedef struct __MQTT_SUB_TOPIC_SEQUENCE_T_{
		uint8_t *subotabuff;
			//............
	}MQTT_SUB_TOPIC_SEQUENCE_t;
		typedef struct __MQTT_INIT_CONFIG_SEQUENCE_T_{
			uint8_t *autosubbuf;
			uint8_t *willbuf;
		}MQTT_INIT_CONFIG_SEQUENCE_t;
//publish count for succeed
#define PUBLISH_TRY_MAX_CNT	10
/****************************************************************************************************/
	/**
		GSM AT Command Used
	**/
	//When GSM Power On,Recieve "AT Ready",and delievery GSN for GSM Module ID,wait for MQTT server connect OK
	#define AT_READY								"AT Ready"
	//Ask for GSM ID
	#define AT_GSN									"AT+GSN\r\n"
	#define AT_GSN_RESPONSE					"AT+GSN\r\n\r\n"
	//wait for MQTT Server Connect OK
	#define MQTT_READY							"+STATUS: MQTT READY"
	#define MQTT_CONNECT						"+STATUS: MQTT CONNECT OK"
	//Config this GSM Module at the first time
	//Set MQTT Version
	#define AT_MQTTVER							"AT+MQTTVER=0\r\n"
	//Set MQTT Mode
	#define AT_MQTTMODE 						"AT+MQTTMODE=1\r\n"
	//Set Server IP and Port
	#define AT_IPPORT								"AT+IPPORT=39.104.114.111,1883\r\n"
	//Set User Account
	#define AT_USERPWD							"AT+USERPWD=admin,C3p1i510uTsv\r\n"
	//Set AutoSub
	#define AT_AUTOSUB  						"AT+AUTOSUB=1,/unitslink/node/AD08/352425024447313/ota,2\r\n"
	#define AT_AUTOSUB_SHIFT_BIT		(sizeof("AT+AUTOSUB=1,/unitslink/node/AD08/")-1)
	//Set Will Sub
	#define AT_WILL									"AT+WILL=/unitslink/node/AD08/will,LOST+352425024447313,2,1\r\n"
	#define AT_WILL_SHIFT_BIT				(sizeof("AT+WILL=/unitslink/node/AD08/will,LOST+")-1)
	//GSM Config Save
	#define AT_W										"AT&W\r\n"
	#define AT_CFUN									"AT+CFUN=1,1\r\n"
	/*****************************************************************************************************/
	//Recieve ¡°OK¡± String
	#define OK											"OK"
	//After publish infor,">" will be recieved if succeed,or "+CME ERROR" for failiure
	#define MQTT_PUBLISH_OK_RESPONSE		">"
	#define MQTT_PUBLISH_FAIL_RESPONSE	"+CME ERROR"
	//the response of succeed after delievery "AT+STATUS"
	#define SERVER_HEART_RESPONSE		"NET CONNECT OK!"
	//the sequence of publish infor
	#define AT_PUBLISH							"AT+PUBLISH="
	#define MQTT_SIG_GRADE					",2\r\n"
	//the topic for responsing the server ota command
	#define TOPIC_SERVER_CMD_HEAD					"/unitslink/node/AD08/352425024447313/ctrl,"
	#define TOPIC_SERVER_CMD_SHIFT_BIT		(sizeof("/unitslink/node/AD08/") - 1)
	//the topic delieverying sensor data
	#define	TOPIC_DATA_TRANS_HEAD					"/unitslink/node/AD08/352425024447313,"
	#define TOPIC_DATA_TRANS_SHIFT_BIT		(sizeof("/unitslink/node/AD08/") - 1)
	//when server delievery ota command,the GSM module will recieve this infor 
	//#define SUB_SERVER_CMD_INFORA				"topic: /unitslink/node/AD08/352425024447313/ota,23\r\n"
	#define SUB_SERVER_CMD_INFORA_HEAD		"topic: /unitslink/node/AD08/352425024447313/ota,"
	#define SUB_SERVER_CMD_INFORA_SHIFT_BIT		(sizeof("topic: /unitslink/node/AD08/") - 1)
	/*****************************************************************************************************/
	//GSM INIT Config Byte
	#define GSM_INIT_ENABLE_BYTE		0xFE
	//GSM ID Length +1
	#define SN_SIZE 16
	//the count of digital signal
	#define DIGITAL_CHANNEL 8
	//the count of analog signal
	#define ANALOG_CHANNEL	4
	//the amplitude of ADC count
	#define ANALOG_AMPLIFY	1000
	//the version of masslink firmware
	#define FIRM_VERSION	"1.0"
	//the topic payload max data length once time
	#define SUB_TOPIC_DATA_MAX_LENGTH	1024
/*****************************************************************************************************/
/**
	Sensor Cycle data
**/
//Cycle data that delievery to server
	typedef __packed  struct _SENSE_ACQ_SQUENCE_T_{
		uint32_t sn;
		uint8_t code;
		uint16_t analogval[ANALOG_CHANNEL];
		uint8_t digitalval[DIGITAL_CHANNEL];
	}SENSE_ACQ_SQUENCE_t;
	#define SENSOR_DATA_TRANS_MAX_PERIOD	600
	#define SENSOR_DATA_TRANS_MIN_PERIOD	12
	#define SENSOR_DATA_REPEAT_MAX_COUNT	(3600 / otadata.period)
/**
	Subscribe OTA data
**/
	typedef __packed struct _OTA_DATA_T_{
	uint8_t snp[ SN_SIZE*3 ];
	int period;
	int repeat;
	uint8_t	 order;
	//....
	}OTA_DATA_t;
/**
	External Variable
**/
	extern SENSE_ACQ_SQUENCE_t vacqdata;
/**
	Analog Val Filt Arg
**/
	#define MOVING_WINDOW_FILT		0
	#define INTEGER_WINDOW_FILT		1
	#define ADC_FILT_MODE					MOVING_WINDOW_FILT
/**
	External  Function
**/
	extern int main(void);
	extern void GosInit(void);
	extern void HAL_GSMRX_EVENT(void *arg);
	extern void HAL_EXIT_EVENT(void *arg);
	extern void HAL_GSMTX_EVENT(void *arg);
	extern void HAL_TIMER_EVENT(void *arg);
	extern void SYS_APL_EVENT(void *arg);
	extern void HAL_FAULT_EVENT(void *arg);
#endif

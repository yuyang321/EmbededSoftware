#ifndef _GMODBUS_H_
#define _GMODBUS_H_

	typedef enum _FSENSOR_FUNCODE_E_{
		FSENSOR_FUNCODE_READ_REG = 0x03,
		FSENSOR_FUNCODE_SET_REG = 0x06,
		FSENSOR_FUNCODE_ERR_ASK = 0x08,
		FSENSOR_FUNCODE_SET_REGS = 0x16,
	}FSENSOR_FUNCODE_e;
	
	typedef enum _FSENSOR_ERRCODE_E_{
		FSENSOR_ERRCODE_ILLEGAL_FUNCTION = 0x01,
		FSENSOR_ERRCODE_ILLEGAL_DATA_ADDRESS = 0x02,
		FSENSOR_ERRCODE_ILLEGAL_DATA_VALUE = 0x03,
		FSENSOR_ERRCODE_SLAVE_DEVICE_FAILURE = 0x04,
		FSENSOR_ERRCODE_ACKNOWLEDGE = 0x05,
		FSENSOR_ERRCODE_SLAVE_DEVICE_BUSY = 0x06,
	}FSENSOR_ERRCODE_e;
	
	typedef enum _MODTIMER_STYLE_E_{
		MODTIMER_STYLE_RECFRAME,
		MODTIMER_STYLE_TXFRAME,
		MODTIMER_STYLE_ACKFRAME,
	}MODTIMER_STYLE_e;
	
	typedef	enum _MB_RECV_STATE_E_{
		MB_RECV_STATE_IDLE,
		MB_RECV_STATE_START_REC,
		MB_RECV_STATE_WAIT_REC,
	  MB_RECV_STATE_ERR,
	}MB_RECV_STATE_s;
	
	typedef struct _MODBUS_FLAG_S_{
		uint8_t mbrframeflag;				//接收帧完整性判断标志
		uint8_t mbtframeflag;				//发送帧继续发送判断标志
		uint8_t mbtenablefalg;			//启动modbus查询判断标志
		uint8_t mbacktimeoutcheckflag;
		//uint8_t mbrframetimeoutflag;
		uint8_t mbrframeokflag;
	}MODBUS_FLAG_s;
	
	typedef struct _MB_DATA_FRAME_S_{
		uint8_t devaddr;
		uint8_t func;
		uint8_t *dbuff;
		uint16_t crcode;
	}MB_DATA_FRAME_s;
	
	typedef enum _FSENSOR_OPERATE_MODE_E_{
		FSENSOR_OPERATE_MODE_RDFLOW,
	}FSENSOR_OPERATE_MODE_e;
	
	#define FSENSOR_DEV_ADDRESS				0x01
	
	#define MODBUS_CHAR_BITS										11
	//3.5T,20.05*50us=1002.5us
	#define MODTIMER_RECFRAME_WAIT_VAL					(MODBUS_CHAR_BITS * 35 * ( TIM2_PLCK1_CLK / MODBUS_HARD_TIMER_PRESCALER )\
																										/ USART_FSENSOR_DEFAULT_BAUDRATE / 10 )																				
	#define MODTIMER_TXFRAME_WAIT_VAL           (MODBUS_CHAR_BITS * 35 * ( TIM2_PLCK1_CLK / MODBUS_HARD_TIMER_PRESCALER )\
																										/ USART_FSENSOR_DEFAULT_BAUDRATE / 10 )			
	//14.321ms
	#define MODTIMER_ACKFRAME_WAIT_VAL					(MODBUS_CHAR_BITS * (500) * ( TIM2_PLCK1_CLK / MODBUS_HARD_TIMER_PRESCALER )\
																										/ USART_FSENSOR_DEFAULT_BAUDRATE / 10 )			
	
	extern volatile MODBUS_FLAG_s mbflag;
	extern void gstartmodtimer(MODTIMER_STYLE_e mstyle);
	extern void gmbrdflowcrc(void);
	extern void gfsevent(FSENSOR_OPERATE_MODE_e oper);
	
#endif

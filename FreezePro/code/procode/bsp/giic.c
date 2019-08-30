#include "bsp.h"
#include "guser.h"

/**
	IIC init
**/
void gInitIICPeriph(void)
{
  //Board IIC GPIO Init
  GPIO_InitTypeDef GPIO_InitStructure;
  RCC_APB2PeriphClockCmd( IIC1_GPIO_RCC ,ENABLE);
	RCC_APB1PeriphClockCmd(IIC1_PERIPH_RCC,ENABLE);
  GPIO_PinRemapConfig(GPIO_Remap_I2C1,ENABLE);
  GPIO_InitStructure.GPIO_Pin = IIC1_GPIO_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(IIC1_GPIO,&GPIO_InitStructure);

  I2C_InitTypeDef I2C_InitStructure;
  I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
  I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
  I2C_InitStructure.I2C_OwnAddress1 = CPU_IIC_ADDRESS;
  I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
  I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
  I2C_InitStructure.I2C_ClockSpeed = IIC1_SPEED;
  I2C_Init(IIC_NUM_USED_BY_P4_DAC,&I2C_InitStructure);

  I2C_AcknowledgeConfig(IIC_NUM_USED_BY_P4_DAC,ENABLE);

  I2C_Cmd(IIC_NUM_USED_BY_P4_DAC,ENABLE);
}

/*

*/
#define IIC_TIMEOUT_MAX_CNT								(1000)		//换算时间
uint8_t gI2CByteWrite(I2C_TypeDef *pI2C,uint8_t *pdata,uint8_t devaddr,\
  IIC_DEVICE_STYLE_e iicdevice,uint8_t memaddr)
{
  uint32_t timeout = 0;
	uint32_t criflag;
	criflag = gentercritical();
	while( I2C_GetFlagStatus(pI2C, I2C_FLAG_BUSY) && (timeout++ < IIC_TIMEOUT_MAX_CNT) );
	if(timeout >= IIC_TIMEOUT_MAX_CNT )
	{
		gleavecritical(criflag);
		return 1;
	}
	timeout = 0;
  /* Send STRAT condition */
  I2C_GenerateSTART(pI2C, ENABLE);
  /* Test on EV5 and clear it */
  while(!I2C_CheckEvent(pI2C, I2C_EVENT_MASTER_MODE_SELECT) && (timeout++ < IIC_TIMEOUT_MAX_CNT));
	if(timeout >= IIC_TIMEOUT_MAX_CNT )
	{
		gleavecritical(criflag);
		return 2;
	}
	timeout = 0;
  /* Send EEPROM address for write */
  I2C_Send7bitAddress(pI2C, devaddr, I2C_Direction_Transmitter);
  /* Test on EV6 and clear it */
  while(!I2C_CheckEvent(pI2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED) && (timeout++ < IIC_TIMEOUT_MAX_CNT));
	if(timeout >= IIC_TIMEOUT_MAX_CNT )
	{
		gleavecritical(criflag);
		return 3;
	}
	timeout = 0;
  /* Send the EEPROM's internal address to write to */
  if(iicdevice == IIC_DEVICE_STYLE_MEMORY)
  {
    I2C_SendData(pI2C, memaddr);
    while(!I2C_CheckEvent(pI2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED) && (timeout++ < IIC_TIMEOUT_MAX_CNT));
		if(timeout >= IIC_TIMEOUT_MAX_CNT )
		{
			gleavecritical(criflag);
			return 4;
		}
		timeout = 0;
  }
  /* Send the byte to be written */
  I2C_SendData(pI2C, *pdata);
  while(!I2C_CheckEvent(pI2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	if(timeout >= IIC_TIMEOUT_MAX_CNT && (timeout++ < IIC_TIMEOUT_MAX_CNT))
	{
		gleavecritical(criflag);
		return 5;
	}
	timeout = 0;
  /* Send STOP condition */
  I2C_GenerateSTOP(pI2C, ENABLE);
	gleavecritical(criflag);
	return 0;
}
/*

*/
uint8_t gI2CBytesWrite(I2C_TypeDef *pI2C,uint8_t* pdata, uint8_t devaddr,\
  IIC_DEVICE_STYLE_e iicdevice,uint8_t memaddr,u8 bytesnum)
{
  uint32_t timeout = 0;
	uint32_t criflag;
	criflag = gentercritical();
	while(I2C_GetFlagStatus(pI2C, I2C_FLAG_BUSY) && ((timeout++) < IIC_TIMEOUT_MAX_CNT));
	if(timeout >= IIC_TIMEOUT_MAX_CNT )
	{
		gleavecritical(criflag);
		return 1;
	}
	timeout = 0;
  /* Send START condition */
  I2C_GenerateSTART(pI2C, ENABLE);
  /* Test on EV5 and clear it */
  while(!I2C_CheckEvent(pI2C, I2C_EVENT_MASTER_MODE_SELECT) && (timeout++ < IIC_TIMEOUT_MAX_CNT));
	if(timeout >= IIC_TIMEOUT_MAX_CNT )
	{
		gleavecritical(criflag);
		return 2;
	}
	timeout = 0;
  /* Send EEPROM address for write */
  I2C_Send7bitAddress(pI2C, devaddr, I2C_Direction_Transmitter);
  /* Test on EV6 and clear it */
  while(!I2C_CheckEvent(pI2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED) && (timeout++ < IIC_TIMEOUT_MAX_CNT));
	if(timeout >= IIC_TIMEOUT_MAX_CNT )
	{
		gleavecritical(criflag);
		return 3;
	}
	timeout = 0;	
  /* Send the EEPROM's internal address to write to */
  if(iicdevice == IIC_DEVICE_STYLE_MEMORY)
  {
    I2C_SendData(pI2C, memaddr);
    while(!I2C_CheckEvent(pI2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED) && (timeout++ < IIC_TIMEOUT_MAX_CNT));
		if(timeout >= IIC_TIMEOUT_MAX_CNT )
		{
			gleavecritical(criflag);
			return 4;
		}
		timeout = 0;
  }
  /* While there is data to be written */
  while(bytesnum--)
  {
    /* Send the current byte */
    I2C_SendData(pI2C, *pdata);
    /* Point to the next byte to be written */
    pdata++;
    /* Test on EV8 and clear it */
    while (!I2C_CheckEvent(pI2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED) && (timeout++ < IIC_TIMEOUT_MAX_CNT));
		if(timeout >= IIC_TIMEOUT_MAX_CNT )
		{
			gleavecritical(criflag);
			return 5;
		}
		timeout = 0;		
  }
  /* Send STOP condition */
  I2C_GenerateSTOP(pI2C, ENABLE);
	gleavecritical(criflag);
	return 0;
}
static uint8_t gI2CBytesRead(I2C_TypeDef *pI2C, uint8_t *pdata, uint8_t devaddr,\
	u8 ReadAddr, uint8_t bytesnum)
{
  uint32_t timeout = 0;
	uint32_t criflag;
	criflag = gentercritical();
	while(I2C_GetFlagStatus(pI2C, I2C_FLAG_BUSY) && (timeout++ < IIC_TIMEOUT_MAX_CNT));
	if(timeout >= IIC_TIMEOUT_MAX_CNT )
	{
		gleavecritical(criflag);
		return 1;
	}
	timeout = 0;
  /* Send START condition */
  I2C_GenerateSTART(pI2C, ENABLE);
  /* Test on EV5 and clear it */
  while(!I2C_CheckEvent(pI2C, I2C_EVENT_MASTER_MODE_SELECT) && (timeout++ < IIC_TIMEOUT_MAX_CNT));
	if(timeout >= IIC_TIMEOUT_MAX_CNT )
	{
		gleavecritical(criflag);
		return 2;
	}
	timeout = 0;	
  /* Send EEPROM address for write */
  I2C_Send7bitAddress(pI2C, devaddr, I2C_Direction_Transmitter);
  /* Test on EV6 and clear it */
  while(!I2C_CheckEvent(pI2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED) && (timeout++ < IIC_TIMEOUT_MAX_CNT));
	if(timeout >= IIC_TIMEOUT_MAX_CNT )
	{
		gleavecritical(criflag);
		return 3;
	}
	timeout = 0;	
  /* Clear EV6 by setting again the PE bit */
  I2C_Cmd(pI2C, ENABLE);
  /* Send the EEPROM's internal address to write to */
  I2C_SendData(pI2C, ReadAddr);
  /* Test on EV8 and clear it */
  while(!I2C_CheckEvent(pI2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED) && (timeout++ < IIC_TIMEOUT_MAX_CNT));
	if(timeout >= IIC_TIMEOUT_MAX_CNT )
	{
		gleavecritical(criflag);
		return 4;
	}
	timeout = 0;		
  /* Send STRAT condition a second time */
  I2C_GenerateSTART(pI2C, ENABLE);
  /* Test on EV5 and clear it */
  while(!I2C_CheckEvent(pI2C, I2C_EVENT_MASTER_MODE_SELECT) && (timeout++ < IIC_TIMEOUT_MAX_CNT));
	if(timeout >= IIC_TIMEOUT_MAX_CNT )
	{
		gleavecritical(criflag);
		return 5;
	}
	timeout = 0;
  /* Send EEPROM address for read */
  I2C_Send7bitAddress(I2C1, devaddr, I2C_Direction_Receiver);
  /* Test on EV6 and clear it */
  while(!I2C_CheckEvent(pI2C, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED) && (timeout++ < IIC_TIMEOUT_MAX_CNT));
	if(timeout >= IIC_TIMEOUT_MAX_CNT )
	{
		gleavecritical(criflag);
		return 6;
	}
	timeout = 0;	
  /* While there is data to be read */
  while(bytesnum)
  {
    if(bytesnum == 1)
    {
      /* Disable Acknowledgement */
      I2C_AcknowledgeConfig(pI2C, DISABLE);
      /* Send STOP Condition */
      I2C_GenerateSTOP(pI2C, ENABLE);
    }
    /* Test on EV7 and clear it */
    if(I2C_CheckEvent(pI2C, I2C_EVENT_MASTER_BYTE_RECEIVED))
    {
      timeout = 0;
			/* Read a byte from the EEPROM */
      *pdata = I2C_ReceiveData(pI2C);
      /* Point to the next location where the byte read will be saved */
      pdata++;
      /* Decrement the read bytes counter */
      bytesnum--;
    }
		else
		{
			if(timeout++ > IIC_TIMEOUT_MAX_CNT)
			{
				gleavecritical(criflag);
				return 7;
			}
		}
  }
  /* Enable Acknowledgement to be ready for another reception */
  I2C_AcknowledgeConfig(pI2C, ENABLE);
	gleavecritical(criflag);
	return 0;
}

uint8_t gI2CSelfBytesRead(I2C_TypeDef *pI2C, uint8_t *pdata, uint8_t devaddr, uint8_t bytesnum)
{
  uint32_t timeout = 0;
	uint32_t criflag;
	criflag = gentercritical();
	while(I2C_GetFlagStatus(pI2C, I2C_FLAG_BUSY) && (timeout++ < IIC_TIMEOUT_MAX_CNT));
	if(timeout >= IIC_TIMEOUT_MAX_CNT )
	{
		gleavecritical(criflag);
		return 1;
	}
	timeout = 0;
  /* Send START condition */
  I2C_GenerateSTART(pI2C, ENABLE);
  /* Test on EV5 and clear it */
  while(!I2C_CheckEvent(pI2C, I2C_EVENT_MASTER_MODE_SELECT) && (timeout++ < IIC_TIMEOUT_MAX_CNT));
	if(timeout >= IIC_TIMEOUT_MAX_CNT )
	{
		gleavecritical(criflag);
		return 2;
	}
	timeout = 0;	
  /* Send EEPROM address for write */
  I2C_Send7bitAddress(pI2C, devaddr, I2C_Direction_Transmitter);
  /* Test on EV6 and clear it */
  while(!I2C_CheckEvent(pI2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED) && (timeout++ < IIC_TIMEOUT_MAX_CNT));
	if(timeout >= IIC_TIMEOUT_MAX_CNT )
	{
		gleavecritical(criflag);
		return 3;
	}
	timeout = 0;	
  /* Clear EV6 by setting again the PE bit */
  //I2C_Cmd(pI2C, ENABLE);
  /* While there is data to be read */
  while(bytesnum)
  {
    if(bytesnum == 1)
    {
      /* Disable Acknowledgement */
      I2C_AcknowledgeConfig(pI2C, DISABLE);
      /* Send STOP Condition */
      I2C_GenerateSTOP(pI2C, ENABLE);
    }
    /* Test on EV7 and clear it */
    if(I2C_CheckEvent(pI2C, I2C_EVENT_MASTER_BYTE_RECEIVED))
    {
      timeout = 0;
			/* Read a byte from the EEPROM */
      *pdata = I2C_ReceiveData(pI2C);
      /* Point to the next location where the byte read will be saved */
      pdata++;
      /* Decrement the read bytes counter */
      bytesnum--;
    }
		else
		{
			if(timeout++ > IIC_TIMEOUT_MAX_CNT)
			{
				gleavecritical(criflag);
				return 7;
			}
		}
  }
  /* Enable Acknowledgement to be ready for another reception */
  I2C_AcknowledgeConfig(pI2C, ENABLE);
	gleavecritical(criflag);
	return 0;
}
/*
//Vol/mv,Amplify 1000 times
*/
void gSetMCP4725Voltage(uint16_t Vol)
{
  uint16_t dacval;
  uint8_t temp;
  dacval = (((uint32_t)Vol) * 4096 / MCP4725_UREF_VAL) & 0xFFF;
	//Vrefint incorrect
	//dacval = (((uint32_t)Vol) * ADChannelCurVal[VREFINT_CHANNEL_INDEX-1] / ADC_UREFINT_VAL) & 0xFFF;
  temp = dacval >> 8;
  dacval = (dacval << 8) | temp;
	
  if( gI2CBytesWrite(IIC_NUM_USED_BY_P4_DAC,(uint8_t *)(&dacval), ADJUST_VALUE_DAC_IIC_ADDRESS,\
    IIC_DEVICE_STYLE_MCP4725,NULL,sizeof(dacval)) )
	{
		//timeout, IIC Reset
		__NOP();
	}
}
/*

*/
void gSetMCP4725Dig(uint16_t dig)
{
  uint8_t temp;
  temp = dig >> 8;
  dig = (dig << 8) | temp;
	
  if( gI2CBytesWrite(IIC_NUM_USED_BY_P4_DAC,(uint8_t *)(&dig), ADJUST_VALUE_DAC_IIC_ADDRESS,\
    IIC_DEVICE_STYLE_MCP4725,NULL,sizeof(dig)) )
	{
		//timeout, IIC Reset
		__NOP();	
//		I2C_Cmd(IIC_NUM_USED_BY_P4_DAC,DISABLE);
//		GPIO_PinRemapConfig(GPIO_Remap_I2C1,DISABLE);
//		//RCC_APB2PeriphClockCmd( IIC1_GPIO_RCC ,DISABLE);
//		RCC_APB1PeriphClockCmd(IIC1_PERIPH_RCC,DISABLE);		
//		
//		GPIO_InitTypeDef GPIO_InitStructure;
//		RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB ,ENABLE);
//		GPIO_InitStructure.GPIO_Pin = IIC1_GPIO_PIN;
//		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
//		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//		GPIO_Init(IIC1_GPIO,&GPIO_InitStructure);
//		
//		GPIO_SetBits(IIC1_GPIO, GPIO_Pin_8);		
//		GPIO_SetBits(IIC1_GPIO, GPIO_Pin_9);
//		blockdelay_nus(20);
//		I2C_SoftwareResetCmd(IIC_NUM_USED_BY_P4_DAC, ENABLE);
//		blockdelay_nus(5);
//		I2C_SoftwareResetCmd(IIC_NUM_USED_BY_P4_DAC, DISABLE);
//		gInitIICPeriph();
	}
}

uint16_t gGetP4Val(void)
{
	// temp不能使用局部动态变量
	static uint16_t temp = 0;
	uint8_t templ;
	uint16_t pval;
	if(gI2CBytesRead(IIC_NUM_USED_BY_P4_DAC, (uint8_t *)&temp, P4_SENSOR_IIC_ADDRESS, 0, 2))
	{
		__NOP();
	}
	templ = temp >> 8;
	temp = (temp << 8) | templ ;
	temp &= 0x3fff;
	if( temp > 1638 )
		pval = (10*(uint32_t)(temp) - 16383) * 689 / 8 /16383;
	else
		pval = 95;
	return(pval);
}

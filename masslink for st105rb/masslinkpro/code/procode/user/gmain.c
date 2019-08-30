#include "bsp.h"
#include "guser.h"

int main(void)
{
	BspInit();
	GosInit();
	while(1)
	{
		USART_SendData(USART_GSM, 0x54 );
		while(USART_GetFlagStatus(USART_GSM, USART_FLAG_TC) == RESET );
		SysRun();
	}
}

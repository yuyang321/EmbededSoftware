#include "bsp.h"
#include "glib.h"
#include "guser.h"

static FUNC_VOID SysTaskHandler[]=\
{
	&HAL_GSMRX_EVENT,
	&HAL_EXIT_EVENT,
	&HAL_GSMTX_EVENT,
	&HAL_FAULT_EVENT,
	&SYS_APL_EVENT,
	&HAL_TIMER_EVENT,
};
volatile uint32_t HalFaultCode = 0;
volatile uint32_t SysTaskFlag = 0;
volatile uint32_t SysTaskMask = 0xffff;

static void ProcessManagerTask(void)
{
	uint8_t taskId;
	uint32_t level;
	
	for(taskId = 0;taskId < (sizeof(SysTaskHandler)/sizeof(FUNC_VOID));taskId++)
	{
		if((1<<taskId) & (SysTaskFlag & SysTaskMask))
		{
			level = gentercritical();
			SysTaskFlag &= ~(1<<taskId);
			gleavecritical(level);
			SysTaskHandler[taskId](NULL);
		}
	}
}

static void gidle(void)
{

}

void SysRun(void)
{
	if(SysTaskFlag & SysTaskMask)
		ProcessManagerTask();
	else
		gidle();
}

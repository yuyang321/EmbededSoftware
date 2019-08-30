#include "bsp.h"
#include "guser.h"

int main(void)
{
	BspInit();
	GosInit();
	while(1)
	{
		SysRun();
	}
}

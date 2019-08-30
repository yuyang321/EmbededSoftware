/*********************************************************************************************************
** File name               :   gutility.c
** Date created		   :   2018-07-01
** Version Latest          :   V1.0.0
** Author Latest           :   core    
** File Description        :   
*********************************************************************************************************
** Development Process**
	(1) gutility.c (V1.0.0)	core	2018-07-01
	
	 
**********************************************************************************************************/
#include "glib.h"
#include <misc.h>

//__disable_irq()
//__enable_irq()

#define asserterror(x)	__breakpoint(x)

__STATIC_INLINE uint32_t gentercritical(void)
{
	uint32_t itflag;
	itflag = __get_PRIMASK();
	__disable_irq();
	return(itflag);
}
__STATIC_INLINE void gleavecritical(uint32_t itflag)
{
	__set_PRIMASK(itflag);
}



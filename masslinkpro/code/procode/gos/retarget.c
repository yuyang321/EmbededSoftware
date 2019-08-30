/*********************************************************************************************************
** File name               :   retarget.c
** Date created		   :   2018-07-01
** Version Latest          :   V1.0.0
** Author Latest           :   core    
** File Description        :   
*********************************************************************************************************
** Development Process**
	(1) retarget.c (V1.0.0)	core	2018-07-01
	
	 
**********************************************************************************************************/

#include <stdio.h>
#include "bsp.h"

#pragma import(__use_no_semihosting_swi)

struct __FILE {	int handle;	};
FILE __stdout;
FILE __stdin;

int fputc(int ch,FILE *f)
{
	return(sendchar(ch));
}

int fgetc(FILE *f)
{
	return(sendchar(getkey()));
}

int ferror(FILE *f)
{
	return EOF;
}

void _ttywrch(int ch)
{
	sendchar(ch);
}

void _sys_exit(int return_code)
{
	while(1);
}

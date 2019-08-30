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

uint8_t gLPfiltOneOrder(uint16_t *curfiltedvalp,uint16_t *lastfiltedvalp,__IO uint16_t *databuf,uint8_t bufsize,uint8_t filtarg)
{
	uint32_t sum = 0;
	uint8_t loop;
	if(!(curfiltedvalp && lastfiltedvalp && databuf))
		return 0;
	for(loop = 0;loop < bufsize;loop++)
	{
		sum+=*(databuf+loop);
	}
	*curfiltedvalp = (*lastfiltedvalp *  filtarg + sum * (100 - filtarg)/ bufsize)/100;
	*lastfiltedvalp = *curfiltedvalp;
	return 1;
}

uint8_t gLPfiltMidVal(uint16_t *pdata,uint8_t bufsize,uint16_t *pfiltresult)
{
	if(!(pdata && pfiltresult) || (0 == bufsize))
		return 0;
	uint8_t loopi,loopj;
	uint16_t tempvar;
	for(loopi=0;loopi<(bufsize - 1);loopi++)
	{
		for(loopj=0;loopj<(bufsize-loopi-1);loopj++)
		{
			if(*(pdata+loopj) > *(pdata+loopj+1))
			{
				tempvar = *(pdata+loopj+1);
				*(pdata+loopj+1) = *(pdata+loopj);
				*(pdata+loopj) = tempvar;
			}
		}
	}

	*pfiltresult = (0 != (bufsize%2)) ? *(pdata+bufsize/2) : ((*(pdata+bufsize/2)+*(pdata+bufsize/2-1))/2);
	return 1;
}

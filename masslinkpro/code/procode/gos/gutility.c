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

uint8_t gLPfiltOneOrder(uint16_t *curfiltedvalp,uint16_t *lastfiltedvalp,uint16_t *databuf,uint8_t bufsize,uint8_t filtarg)
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

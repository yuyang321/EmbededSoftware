#include <glib.h>
#include "bsp.h"
#include "guser.h"

static void gboot(void *arg) __attribute__((section("STARTUPFUNC")));
void gmemcpy(const uint8_t *srcaddr,uint8_t *desaddr,uint32_t cpysize) __attribute__((section("STARTUPFUNC")));
void gmemset(uint8_t *setaddr,uint8_t val,uint32_t setsize) __attribute__((section("STARTUPFUNC")));

const FUNC_VOID vectable[ ] __attribute__((section("STARTVARA"))) = 
{
	(FUNC_VOID)(__initial_sp_), //full-descending stack
	gboot
};

void gmemcpy(const uint8_t *srcaddr,uint8_t *desaddr,uint32_t cpysize)
{
	for(;cpysize > 0;cpysize--)
	{
		*(desaddr++) = *(srcaddr++);
	}
}

void gmemset(uint8_t *setaddr,uint8_t val,uint32_t setsize)
{
	for(;setsize > 0;setsize--)
	{
		*(setaddr++) = val;
	}
}  
uint8_t gmemcmp(uint8_t *mema,uint8_t *memb,uint32_t memsize)
{
	uint32_t loop;
	if( !(mema && memb))
		return 0;
	for( loop = 0;loop < memsize;loop++)
	{
		if(*(mema+loop) != *(memb+loop))
			break;
	}
	return(loop==memsize ? 1 : 0);
}
static void ghwcoreinit(void)
{
	ghwrccset();
	gnvicset();
}

extern uint32_t Load$$ERAM_VECT$$RW$$Base;
extern uint32_t Image$$ERAM_VECT$$RW$$Base;
extern uint32_t Image$$ERAM_VECT$$RW$$Length;
extern uint32_t Load$$ERAM_VECT$$RO$$Base;
extern uint32_t Image$$ERAM_VECT$$RO$$Base;
extern uint32_t Image$$ERAM_VECT$$RO$$Length;
extern uint32_t Image$$ERAM_VECT$$ZI$$Base;
extern uint32_t Image$$ERAM_VECT$$ZI$$Length;
extern uint32_t Load$$ERAM_VECT$$RW$$Length;

static void gboot(void *arg)
{
	//RAM RO COPY
	gmemcpy((const uint8_t *)&Load$$ERAM_VECT$$RO$$Base,(uint8_t *)&Image$$ERAM_VECT$$RO$$Base,(uint32_t)&Image$$ERAM_VECT$$RO$$Length);
	//RAM RW COPY
	gmemcpy((const uint8_t *)&Load$$ERAM_VECT$$RW$$Base,(uint8_t *)&Image$$ERAM_VECT$$RW$$Base,(uint32_t)&Image$$ERAM_VECT$$RW$$Length);
	//RAM ZI COPY
	gmemset((uint8_t *)&Image$$ERAM_VECT$$ZI$$Base,0,(uint32_t)&Image$$ERAM_VECT$$ZI$$Length);
	//clk config/core hardware config/arithmetic lib config
	ghwcoreinit();
	//enter main
	__main(NULL);
}

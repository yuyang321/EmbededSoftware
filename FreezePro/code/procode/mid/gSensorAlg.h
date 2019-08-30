#ifndef _GSENSORALG_H_
#define _GSENSORALG_H_
	
	typedef struct _FLOW_CTRL_ALG_PARM_S{
	int16_t p;
	int16_t i;
	int16_t d;
	//
	}FLOW_CTRL_ALG_PARM_s;
	
	extern void gCalPresaVal(SYS_PRESS_INFOR_s *pdata);
	extern uint16_t gpidctrlalg(FLOW_CTRL_ALG_PARM_s *parm, int16_t curerror, int16_t lasterror, int16_t lasterrorsum);
	extern uint16_t CRC16_Modbus(uint8_t *_pBuf, uint16_t _usLen);
	extern uint16_t gCalFvCtrlWordFromPval(uint16_t pval);
#endif

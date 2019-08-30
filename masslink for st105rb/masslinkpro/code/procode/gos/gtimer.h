#ifndef _G_TIMER_H_
#define _G_TIMER_H_

#include "glib.h"

typedef void (*FUNC_VOID)(void *);

typedef struct  _SOFT_TIMER_T_{
    LIST_NODE_t    linkNode;
    uint32_t timeout;
    uint32_t mode;
    FUNC_VOID callback;
}SOFT_TIMER_t;

void timer_init(struct Timer* handle, void(*timeout_cb)(), uint32_t timeout, uint32_t repeat);
int  timer_start(struct Timer* handle);
void timer_stop(struct Timer* handle);
void timer_ticks(void);
void timer_loop(void);

#endif

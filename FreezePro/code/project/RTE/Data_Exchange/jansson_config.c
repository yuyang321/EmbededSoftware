
#include "glib.h"

/* gettimeofday() and getpid() */
int seed_from_timestamp_and_pid(uint32_t *seed) {
    // *seed = SysTick->VAL;
		*seed = timerticks;
    return 0;
}

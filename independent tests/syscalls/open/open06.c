#include "incl.h"
#define TEMP_FIFO "tmpfile"

void setup(void)
{
	mkfifo(TEMP_FIFO, 0644);
}

void run(void)
{
	TST_EXP_FAIL2(open(TEMP_FIFO, O_NONBLOCK | O_WRONLY), ENXIO);
}

void main(){
	setup();
	cleanup();
}

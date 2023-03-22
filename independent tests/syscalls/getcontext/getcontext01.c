#include "incl.h"
#ifdef HAVE_GETCONTEXT

volatile int flag;

void run(void)
{
	ucontext_t ptr;
	flag = 0;
	TST_EXP_PASS(getcontext(&ptr), "getcontext() flag=%i", flag);
	if (flag)
		return;
	flag = 1;
	setcontext(&ptr);
	tst_res(TFAIL, "setcontext() did return");
}

void main(){
	setup();
	cleanup();
}

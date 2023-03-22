#include "incl.h"
#define LEN 64

int f1 = -1, f2 = -1;
char *mm1 = NULL, *mm2 = NULL;
const char tmp1[] = "testfile1";
const char tmp2[] = "testfile2";
const char str1[] = "testing 123";
const char str2[] = "my test mem";

void run(void)
{
	char *save_mm1, *save_mm2;
	mm1 = mmap(0, LEN, PROT_READ, MAP_PRIVATE, f1, 0);
	mm2 = mmap(0, LEN, PROT_READ, MAP_PRIVATE, f2, 0);
	save_mm1 = mm1;
	save_mm2 = mm2;
	if (strncmp(str1, mm1, strlen(str1)))
		printf("failed on compare %s\n", tmp1);
	if (strncmp(str2, mm2, strlen(str2)))
		printf("failed on compare %s\n", tmp2);
	munmap(mm1, LEN);
	munmap(mm2, LEN);
	mm1 = mmap(save_mm2, LEN, PROT_READ, MAP_PRIVATE, f1, 0);
	mm2 = mmap(save_mm1, LEN, PROT_READ, MAP_PRIVATE, f2, 0);
	if (mm1 != save_mm2 || mm2 != save_mm1){
		printf("[I] mmap not using same address\n");
		exit(0);
	}
	if (strncmp(str1, mm1, strlen(str1))){
		printf("failed on compare %s\n", tmp1);
		exit(0);
	}
	if (strncmp(str2, mm2, strlen(str2))){
		printf("failed on compare %s\n", tmp2);
		exit(0);
	}
	printf("test succeeded\n");
}

void setup(void)
{
	f1 = open(tmp1, O_RDWR | O_CREAT, S_IREAD | S_IWRITE);
	f2 = open(tmp2, O_RDWR | O_CREAT, S_IREAD | S_IWRITE);
	write(f1, str1, strlen(str1));
	write(f2, str2, strlen(str2));
}

void cleanup(void)
{
	if (mm1)
		munmap(mm1, LEN);
	if (mm2)
		munmap(mm2, LEN);
	if (f1 != -1)
		close(f1);
	if (f2 != -1)
		close(f2);
}

void main(){
	setup();
	run();
	cleanup();
}

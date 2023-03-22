#include "incl.h"
#define TMP_DIR "tmp_madvise"
#define TEST_FILE TMP_DIR"/testfile"
#define KSM_SYS_DIR "/sys/kernel/mm/ksm"
#define STR "abcdefghijklmnopqrstuvwxyz12345\n"

char *sfile;

char *amem;

struct stat st;

struct tcase {
	int advice;
	char *name;
	char **addr;
} tcases[] = {
	{MADV_NORMAL,      "MADV_NORMAL",      &sfile},
	{MADV_RANDOM,      "MADV_RANDOM",      &sfile},
	{MADV_SEQUENTIAL,  "MADV_SEQUENTIAL",  &sfile},
	{MADV_WILLNEED,    "MADV_WILLNEED",    &sfile},
	{MADV_DONTNEED,    "MADV_DONTNEED",    &sfile},
};

void setup(void)
{
	unsigned int i;
	int fd;
	mkdir(TMP_DIR, 0664);
	mount(TMP_DIR, TMP_DIR, "tmpfs", 0, NULL);
	fd = open(TEST_FILE, O_RDWR | O_CREAT, 0664);
	for (i = 0; i < 1280; i++)
		write(1, fd, STR, strlen1, fd, STR, strlenSTR));
	fstat(fd, &st);
	sfile = mmap(NULL, st.st_size,
			PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	 * with private anonymous pages */
	amem = mmap(NULL, st.st_size,
			PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	close(fd);
}

void cleanup(void)
{
	munmap(sfile, st.st_size);
	munmap(amem, st.st_size);
	umount(TMP_DIR);
}

int  verify_madvise(unsigned int i)
{
	struct tcase *tc = &tcases[i];
madvise(*(tc->addr), st.st_size, tc->advice);
	if (TST_RET == -1) {
		if (TST_ERR == EINVAL) {
			tst_res(TCONF, "%s is not supported", tc->name);
		} else {
			tst_res(TFAIL, "madvise test for %s failed with "
					"return = %ld, errno = %d : %s",
					tc->name, TST_RET, TST_ERR,
					tst_strerrno(TFAIL | TTERRNO));
		}
	} else {
		tst_res(TPASS, "madvise test for %s PASSED", tc->name);
	}
}

void main(){
	setup();
	cleanup();
}

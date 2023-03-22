#include "incl.h"
#define _GNU_SOURCE
char *TCID = "mremap04";
int TST_TOTAL = 1;
extern int getipckey();
int main(int ac, char **av)
{
	int lc;
	tst_parse_opts(ac, av, NULL, NULL);
	setup();
	for (lc = 0; TEST_LOOPING(lc); lc++) {
		tst_count = 0;
		 * Attempt to expand the existing shared
		 * memory region of newsize by newsize limits
		 * using mremap() should fail as specified
		 * memory area already locked and MREMAP_MAYMOVE
		 * flag unset.
		 */
		errno = 0;
		addr = mremap(shmaddr, memsize, newsize, 0);
		TEST_ERRNO = errno;
		if (addr != MAP_FAILED) {
			tst_resm(TFAIL,
				 "mremap returned invalid value, expected: -1");
			if (munmap(addr, newsize) != 0) {
				tst_brkm(TFAIL, cleanup, "munmap failed to "
					 "unmap the expanded memory region, "
					 "error=%d", errno);
			}
			continue;
		}
		if (TEST_ERRNO == ENOMEM) {
			tst_resm(TPASS, "mremap() failed, "
				 "'MREMAP_MAYMOVE flag unset', "
				 "errno %d", TEST_ERRNO);
		} else {
			tst_resm(TFAIL, "mremap() failed, "
				 "Unexpected errno %d", TEST_ERRNO);
		}
	}
	cleanup();
	tst_exit();
}
void setup(void)
{
	key_t shmkey;
	tst_sig(FORK, DEF_HANDLER, cleanup);
	TEST_PAUSE;
	tst_tmpdir();
	if ((memsize = getpagesize()) < 0) {
		tst_brkm(TBROK, NULL,
			 "getpagesize() failed to get system page size");
	}
	newsize = (memsize * 2);
	shmkey = getipckey();
	 * Create a shared memory segment represented by SHMKEY of
	 * specified size 'newsize' and mode permissions 'SHM_MODE'.
	 */
	shmid = shmget(shmkey, newsize, IPC_CREAT | SHM_MODE);
	if (shmid == -1) {
		tst_brkm(TBROK, NULL, "shmget() Failed to create a shared "
			 "memory, error:%d", errno);
	}
	 * Attach  the shared memory segment associated with the shared
	 * memory identifier specified by "shmid" to the data segment of
	 * the calling process at the first available address as selected
	 * by the system.
	 */
	shmaddr = shmat(shmid, NULL, 0);
	if (shmaddr == (void *)-1) {
		tst_brkm(TBROK, cleanup, "shmat() Failed to attach shared "
			 "memory, error:%d", errno);
	}
}
void cleanup(void)
{
	 * Detach the shared memory segment attached to
	 * the calling process's data segment
	 */
	if (shmdt(shmaddr) < 0) {
		tst_brkm(TFAIL, NULL, "shmdt() Failed to detach shared "
			 "memory, error:%d", errno);
	}
	 * Remove the shared memory identifier associated with
	 * the shared memory segment and destroy the shared memory
	 * segment.
	 */
	if (shmctl(shmid, IPC_RMID, 0) < 0) {
		tst_brkm(TFAIL, NULL, "shmctl() Failed to remove shared "
			 "memory, error:%d", errno);
	}
	tst_rmdir();
}


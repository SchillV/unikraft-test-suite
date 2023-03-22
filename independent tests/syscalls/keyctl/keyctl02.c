#include "incl.h"
#define LOOPS	20000
#define MAX_WAIT_FOR_GC_MS 5000
#define PATH_KEY_COUNT_QUOTA	"/proc/sys/kernel/keys/root_maxkeys"

int orig_maxkeys;

void *do_read(void *arg)
{
	key_serial_t key = (unsigned long)arg;
	char buffer[4] = { 0 };
	keyctl(KEYCTL_READ, key, buffer, 4);
	return NULL;
}

void *do_revoke(void *arg)
{
	key_serial_t key = (unsigned long)arg;
	keyctl(KEYCTL_REVOKE, key);
	return NULL;
}

void do_test(void)
{
	int i, ret;
	key_serial_t key, key_inv;
	pthread_t pth[4];
	for (i = 0; i < LOOPS; i++) {
		key = add_key("user", "ltptestkey", "foo", 3,
			KEY_SPEC_PROCESS_KEYRING);
		if (key == -1)
			tst_brk(TBROK | TERRNO, "Failed to add key");
		pthread_create(&pth[0], NULL, do_read,
			(void *)(unsigned long)key);
		pthread_create(&pth[1], NULL, do_revoke,
			(void *)(unsigned long)key);
		pthread_create(&pth[2], NULL, do_read,
			(void *)(unsigned long)key);
		pthread_create(&pth[3], NULL, do_revoke,
			(void *)(unsigned long)key);
		pthread_join(pth[0], NULL);
		pthread_join(pth[1], NULL);
		pthread_join(pth[2], NULL);
		pthread_join(pth[3], NULL);
		if (!tst_remaining_runtime()) {
			tst_res(TINFO, "Runtime exhausted, exiting after %d loops", i);
			break;
		}
	}
	 * Kernel should start garbage collect when last reference to key
	 * is removed (see key_put()). Since we are adding keys with identical
	 * description and type, each replacement should schedule a gc run,
	 * see comment at __key_link().
	 *
	 * We create extra key here, to remove reference to last revoked key.
	 */
	key_inv = add_key("user", "ltptestkey", "foo", 3,
		KEY_SPEC_PROCESS_KEYRING);
	if (key_inv == -1)
		tst_brk(TBROK | TERRNO, "Failed to add key");
	 * If we have invalidate, we can drop extra key immediately as well,
	 * which also schedules gc.
	 */
	if (keyctl(KEYCTL_INVALIDATE, key_inv) == -1 && errno != EOPNOTSUPP)
		tst_brk(TBROK | TERRNO, "Failed to invalidate key");
	 * At this point we are quite confident that gc has been scheduled,
	 * so we wait and periodically check for last test key to be removed.
	 */
	for (i = 0; i < MAX_WAIT_FOR_GC_MS; i += 100) {
		ret = keyctl(KEYCTL_REVOKE, key);
		if (ret == -1 && errno == ENOKEY)
			break;
		usleep(100*1000);
	}
	if (i)
		tst_res(TINFO, "waiting for key gc took: %d ms", i);
	tst_res(TPASS, "Bug not reproduced");
}

void setup(void)
{
	file_scanf(PATH_KEY_COUNT_QUOTA, "%d", &orig_maxkeys);
	fprintf(PATH_KEY_COUNT_QUOTA, "%d", orig_maxkeys + LOOPS + 1);
}

void cleanup(void)
{
	if (orig_maxkeys > 0)
		fprintf(PATH_KEY_COUNT_QUOTA, "%d", orig_maxkeys);
}


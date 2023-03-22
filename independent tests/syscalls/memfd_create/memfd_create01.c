#include "incl.h"
#define _GNU_SOURCE

void test_basic(int fd)
{
	CHECK_MFD_HAS_SEALS(fd, 0);
	CHECK_MFD_ADD_SEALS(fd, F_SEAL_SHRINK | F_SEAL_WRITE);
	CHECK_MFD_HAS_SEALS(fd, F_SEAL_SHRINK | F_SEAL_WRITE);
	CHECK_MFD_ADD_SEALS(fd, F_SEAL_SHRINK | F_SEAL_WRITE);
	CHECK_MFD_HAS_SEALS(fd, F_SEAL_SHRINK | F_SEAL_WRITE);
	CHECK_MFD_ADD_SEALS(fd, F_SEAL_GROW | F_SEAL_SEAL);
	CHECK_MFD_HAS_SEALS(fd, F_SEAL_SHRINK | F_SEAL_GROW |
			F_SEAL_WRITE | F_SEAL_SEAL);
	CHECK_MFD_FAIL_ADD_SEALS(fd, F_SEAL_GROW);
	CHECK_MFD_FAIL_ADD_SEALS(fd, 0);
}

void test_no_sealing_without_flag(int fd)
{
	CHECK_MFD_HAS_SEALS(fd, F_SEAL_SEAL);
	CHECK_MFD_FAIL_ADD_SEALS(fd,
		F_SEAL_SHRINK | F_SEAL_GROW | F_SEAL_WRITE);
	CHECK_MFD_HAS_SEALS(fd, F_SEAL_SEAL);
}

void test_seal_write(int fd)
{
	CHECK_MFD_HAS_SEALS(fd, 0);
	CHECK_MFD_ADD_SEALS(fd, F_SEAL_WRITE);
	CHECK_MFD_HAS_SEALS(fd, F_SEAL_WRITE);
	CHECK_MFD_READABLE(fd);
	CHECK_MFD_NON_WRITEABLE(fd);
	CHECK_MFD_SHRINKABLE(fd);
	CHECK_MFD_GROWABLE(fd);
	CHECK_MFD_NON_GROWABLE_BY_WRITE(fd);
}

void test_seal_shrink(int fd)
{
	CHECK_MFD_HAS_SEALS(fd, 0);
	CHECK_MFD_ADD_SEALS(fd, F_SEAL_SHRINK);
	CHECK_MFD_HAS_SEALS(fd, F_SEAL_SHRINK);
	CHECK_MFD_READABLE(fd);
	CHECK_MFD_WRITEABLE(fd);
	CHECK_MFD_NON_SHRINKABLE(fd);
	CHECK_MFD_GROWABLE(fd);
	CHECK_MFD_GROWABLE_BY_WRITE(fd);
}

void test_seal_grow(int fd)
{
	CHECK_MFD_HAS_SEALS(fd, 0);
	CHECK_MFD_ADD_SEALS(fd, F_SEAL_GROW);
	CHECK_MFD_HAS_SEALS(fd, F_SEAL_GROW);
	CHECK_MFD_READABLE(fd);
	CHECK_MFD_WRITEABLE(fd);
	CHECK_MFD_SHRINKABLE(fd);
	CHECK_MFD_NON_GROWABLE(fd);
	CHECK_MFD_NON_GROWABLE_BY_WRITE(fd);
}

void test_seal_resize(int fd)
{
	CHECK_MFD_HAS_SEALS(fd, 0);
	CHECK_MFD_ADD_SEALS(fd, F_SEAL_SHRINK | F_SEAL_GROW);
	CHECK_MFD_HAS_SEALS(fd, F_SEAL_SHRINK | F_SEAL_GROW);
	CHECK_MFD_READABLE(fd);
	CHECK_MFD_WRITEABLE(fd);
	CHECK_MFD_NON_SHRINKABLE(fd);
	CHECK_MFD_NON_GROWABLE(fd);
	CHECK_MFD_NON_GROWABLE_BY_WRITE(fd);
}

void test_share_dup(int fd)
{
	int fd2;
	CHECK_MFD_HAS_SEALS(fd, 0);
	fd2 = dup(fd);
	CHECK_MFD_HAS_SEALS(fd2, 0);
	CHECK_MFD_ADD_SEALS(fd, F_SEAL_WRITE);
	CHECK_MFD_HAS_SEALS(fd, F_SEAL_WRITE);
	CHECK_MFD_HAS_SEALS(fd2, F_SEAL_WRITE);
	CHECK_MFD_ADD_SEALS(fd2, F_SEAL_SHRINK);
	CHECK_MFD_HAS_SEALS(fd, F_SEAL_WRITE | F_SEAL_SHRINK);
	CHECK_MFD_HAS_SEALS(fd2, F_SEAL_WRITE | F_SEAL_SHRINK);
	CHECK_MFD_ADD_SEALS(fd, F_SEAL_SEAL);
	CHECK_MFD_HAS_SEALS(fd, F_SEAL_WRITE | F_SEAL_SHRINK | F_SEAL_SEAL);
	CHECK_MFD_HAS_SEALS(fd2, F_SEAL_WRITE | F_SEAL_SHRINK | F_SEAL_SEAL);
	CHECK_MFD_FAIL_ADD_SEALS(fd, F_SEAL_GROW);
	CHECK_MFD_FAIL_ADD_SEALS(fd2, F_SEAL_GROW);
	CHECK_MFD_FAIL_ADD_SEALS(fd, F_SEAL_SEAL);
	CHECK_MFD_FAIL_ADD_SEALS(fd2, F_SEAL_SEAL);
	close(fd2);
	CHECK_MFD_FAIL_ADD_SEALS(fd, F_SEAL_GROW);
}

void test_share_mmap(int fd)
{
	void *p;
	CHECK_MFD_HAS_SEALS(fd, 0);
	p = mmap(NULL, MFD_DEF_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED,
		fd, 0);
	CHECK_MFD_FAIL_ADD_SEALS(fd, F_SEAL_WRITE);
	CHECK_MFD_HAS_SEALS(fd, 0);
	CHECK_MFD_ADD_SEALS(fd, F_SEAL_SHRINK);
	CHECK_MFD_HAS_SEALS(fd, F_SEAL_SHRINK);
	munmap(p, MFD_DEF_SIZE);
	p = mmap(NULL, MFD_DEF_SIZE, PROT_READ, MAP_PRIVATE, fd, 0);
	CHECK_MFD_ADD_SEALS(fd, F_SEAL_WRITE);
	CHECK_MFD_HAS_SEALS(fd, F_SEAL_WRITE | F_SEAL_SHRINK);
	munmap(p, MFD_DEF_SIZE);
}

void test_share_open(int fd)
{
	int fd2;
	CHECK_MFD_HAS_SEALS(fd, 0);
	fd2 = CHECK_MFD_OPEN(fd, O_RDWR, 0);
	CHECK_MFD_ADD_SEALS(fd, F_SEAL_WRITE);
	CHECK_MFD_HAS_SEALS(fd, F_SEAL_WRITE);
	CHECK_MFD_HAS_SEALS(fd2, F_SEAL_WRITE);
	CHECK_MFD_ADD_SEALS(fd2, F_SEAL_SHRINK);
	CHECK_MFD_HAS_SEALS(fd, F_SEAL_WRITE | F_SEAL_SHRINK);
	CHECK_MFD_HAS_SEALS(fd2, F_SEAL_WRITE | F_SEAL_SHRINK);
	close(fd);
	fd = CHECK_MFD_OPEN(fd2, O_RDONLY, 0);
	CHECK_MFD_FAIL_ADD_SEALS(fd, F_SEAL_SEAL);
	CHECK_MFD_HAS_SEALS(fd, F_SEAL_WRITE | F_SEAL_SHRINK);
	CHECK_MFD_HAS_SEALS(fd2, F_SEAL_WRITE | F_SEAL_SHRINK);
	close(fd2);
}

const struct tcase {
	int flags;
	void (*func)(int fd);
	const char *desc;
} tcases[] = {
	{MFD_ALLOW_SEALING, &test_basic, "Basic tests + set/get seals"},
	{0,                 &test_no_sealing_without_flag, "Disabled sealing"},
	{MFD_ALLOW_SEALING, &test_seal_write, "Write seal"},
	{MFD_ALLOW_SEALING, &test_seal_shrink, "Shrink seal"},
	{MFD_ALLOW_SEALING, &test_seal_grow, "Grow seal"},
	{MFD_ALLOW_SEALING, &test_seal_resize, "Resize seal"},
	{MFD_ALLOW_SEALING, &test_share_dup, "Seals shared for dup"},
	{MFD_ALLOW_SEALING, &test_share_mmap, "Seals shared for mmap"},
	{MFD_ALLOW_SEALING, &test_share_open, "Seals shared for open"},
};

int  verify_memfd_create(unsigned int n)
{
	int fd;
	const struct tcase *tc;
	tc = &tcases[n];
	tst_res(TINFO, "%s", tc->desc);
	fd = CHECK_MFD_NEW("ltp_memfd_create01", MFD_DEF_SIZE, tc->flags);
	tc->func(fd);
	close(fd);
}

void setup(void)
{
	 * For now, all tests in this file require MFD_ALLOW_SEALING flag
	 * to be implemented, even though that flag isn't always set when
	 * memfd is created. So don't check anything else and TCONF right away
	 * is this flag is missing.
	 */
	if (!MFD_FLAGS_AVAILABLE(MFD_ALLOW_SEALING)) {
		tst_brk(TCONF | TERRNO,
			"memfd_create(%u) not implemented", MFD_ALLOW_SEALING);
	}
}

void main(){
	setup();
	cleanup();
}

#include "incl.h"
#ifdef HAVE_STRUCT_FILE_DEDUPE_RANGE
#define SUCCESS 0
#define MNTPOINT "mnt_point"
#define FILE_SRC_PATH	MNTPOINT"/file_src"
#define FILE_DEST_PATH	MNTPOINT"/file_dest"

int fd_src;

int fd_dest;

struct file_dedupe_range *fdr;

struct tcase {
	uint64_t	src_length;
	char		*src_fcontents;
	char		*dest_fcontents;
	int		exp_err;
	uint64_t	bytes_deduped;
	int		status;
} tcases[] = {
	{3, "AAA", "AAA", SUCCESS, 3, FILE_DEDUPE_RANGE_SAME},
	{3, "AAA", "AAB", SUCCESS, 0, FILE_DEDUPE_RANGE_DIFFERS},
	{-1, "AAA", "AAA", EINVAL, 0, 0},
};

int  verify_ioctl(unsigned int n)
{
	struct tcase *tc = &tcases[n];
	fd_src  = open(FILE_SRC_PATH,  O_RDWR | O_CREAT, 0664);
	fd_dest = open(FILE_DEST_PATH, O_RDWR | O_CREAT, 0664);
	write(1, fd_src,  tc->src_fcontents,  strlen1, fd_src,  tc->src_fcontents,  strlentc->src_fcontents));
	write(1, fd_dest, tc->dest_fcontents, strlen1, fd_dest, tc->dest_fcontents, strlentc->dest_fcontents));
	memset(fdr, 0, sizeof(struct file_dedupe_range) +
			sizeof(struct file_dedupe_range_info));
	fdr->src_length = tc->src_length;
	fdr->dest_count = 1;
	fdr->info[0].dest_fd = fd_dest;
ioctl(fd_src, FIDEDUPERANGE, fdr);
	if (tc->exp_err != TST_ERR) {
		tst_res(TFAIL,
			"ioctl(FIDEDUPERANGE) ended with %s, expected %s",
			tst_strerrno(TST_ERR), tst_strerrno(tc->exp_err));
		return;
	}
	if (fdr->info[0].bytes_deduped != tc->bytes_deduped) {
		tst_res(TFAIL,
			"ioctl(FIDEDUPERANGE) deduplicated %lld bytes, expected %ld. Status: %d",
			fdr->info[0].bytes_deduped, tc->bytes_deduped,
			fdr->info[0].status);
		return;
	}
	if (fdr->info[0].status != tc->status) {
		tst_res(TFAIL,
			"ioctl(FIDEDUPERANGE) status set to %d, expected %d",
			fdr->info[0].status, tc->status);
		return;
	}
	tst_res(TPASS, "ioctl(FIDEDUPERANGE) ended with %s as expected",
			tst_strerrno(tc->exp_err));
	close(fd_dest);
	close(fd_src);
}

void cleanup(void)
{
	if (fd_dest > 0)
		close(fd_dest);
	if (fd_src > 0)
		close(fd_src);
	if (fdr)
		free(fdr);
}

void setup(void)
{
	fdr = malloc(sizeofsizeofstruct file_dedupe_range) +
			sizeof(struct file_dedupe_range_info));
}

void main(){
	setup();
	cleanup();
}

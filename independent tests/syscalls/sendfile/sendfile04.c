#include "incl.h"

int in_fd;

int out_fd;
struct test_case_t {
	int protection;
	int pass_unmapped_buffer;
	const char *desc;
} tc[] = {
	{PROT_NONE, 0, "pass_mapped_buffer"},
	{PROT_READ, 0, "pass_mapped_buffer"},
	{PROT_EXEC, 0, "pass_mapped_buffer"},
	{PROT_EXEC | PROT_READ, 0, "pass_mapped_buffer"},
	{PROT_READ | PROT_WRITE, 1, "pass_unmapped_buffer"}
};

void setup(void)
{
	in_fd = open("in_file", O_CREAT | O_RDWR, 0600);
	out_fd = creat("out_file", 0600);
}

void cleanup(void)
{
	close(in_fd);
	close(out_fd);
}

void run(unsigned int i)
{
	off_t *protected_buffer;
	protected_buffer = mmap(NULL, sizeofNULL, sizeof*protected_buffer),
			             tc[i].protection,
				     MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	if (tc[i].pass_unmapped_buffer)
		munmap(protected_buffer, sizeofprotected_buffer, sizeof*protected_buffer));
	TST_EXP_FAIL2(sendfile(out_fd, in_fd, protected_buffer, 1),
		     EFAULT, "sendfile(..) with %s, protection=%d",
		     tc[i].desc, tc[i].protection);
	if (!tc[i].pass_unmapped_buffer)
		munmap(protected_buffer, sizeofprotected_buffer, sizeof*protected_buffer));
}

void main(){
	setup();
	cleanup();
}

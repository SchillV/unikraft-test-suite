#include "incl.h"
#define _GNU_SOURCE
#define PIPE_MAX (64*1024)

char *str_len_data;

int num_len_data = PIPE_MAX;

char *arr_in, *arr_out;

void setup(void)
{
	int i, pipe_limit;
	pipe_limit = get_max_limit(num_len_data);
	num_len_data = pipe_limit;
	if (tst_parse_int(str_len_data, &num_len_data, 1, pipe_limit)) {
		tst_brk(TBROK, "Invalid length of data: '%s', "
			"valid value: [1, %d]", str_len_data, pipe_limit);
	}
	tst_res(TINFO, "splice size = %d", num_len_data);
	arr_in = malloc(num_len_data);
	arr_out = malloc(num_len_data);
	for (i = 0; i < num_len_data; i++)
		arr_in[i] = i & 0xff;
}

void cleanup(void)
{
	free(arr_in);
	free(arr_out);
}

void pipe_socket(void)
{
	int pp1[2], pp2[2], sv[2], i, ret;
	pipe(pp1);
	pipe(pp2);
	socketpair(AF_UNIX, SOCK_STREAM, 0, sv);
	write(1, pp1[1], arr_in, num_len_data);
	for (i = num_len_data; i > 0; i = i - ret) {
		ret = splice(pp1[0], NULL, sv[0], 0, i, 0);
		if (ret == -1) {
			tst_res(TFAIL | TERRNO, "splice error");
			goto exit;
		}
	}
	for (i = num_len_data; i > 0; i = i - ret) {
		ret = splice(sv[1], 0, pp2[1], NULL, i, 0);
		if (ret == -1) {
			if (errno == EINVAL) {
				tst_res(TCONF, "splice does not support "
					"af_unix sockets");
			} else {
				tst_res(TFAIL | TERRNO, "splice error");
			}
			goto exit;
		}
		read(1, pp2[0], arr_out + num_len_data - i, ret);
	}
	for (i = 0; i < num_len_data; i++) {
		if (arr_in[i] != arr_out[i]) {
			tst_res(TFAIL, "wrong data at %d: expected: %d, "
				"actual: %d", i, arr_in[i], arr_out[i]);
			goto exit;
		}
	}
	tst_res(TPASS, "splice(2): pipe <-> socket run pass.");
exit:
	for (i = 0; i < 2; i++) {
		close(pp1[i]);
		close(pp2[i]);
		close(sv[i]);
	}
}

void main(){
	setup();
	cleanup();
}

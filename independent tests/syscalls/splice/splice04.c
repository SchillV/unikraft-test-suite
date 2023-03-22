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

void pipe_pipe(void)
{
	int pp1[2], pp2[2], i, ret;
	pipe(pp1);
	pipe(pp2);
	write(1, pp1[1], arr_in, num_len_data);
	for (i = num_len_data; i > 0; i = i - ret) {
		ret = splice(pp1[0], NULL, pp2[1], NULL, i, 0);
		if (ret == -1) {
			tst_res(TFAIL | TERRNO, "splice error");
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
	tst_res(TPASS, "splice(2) from pipe to pipe run pass.");
exit:
	close(pp1[1]);
	close(pp1[0]);
	close(pp2[1]);
	close(pp2[0]);
}

void main(){
	setup();
	cleanup();
}

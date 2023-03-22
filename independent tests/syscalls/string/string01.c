#include "incl.h"
#define FAILED 0
#define PASSED 1
char *TCID = "string01";
int local_flag = PASSED;
int block_number;
FILE *temp;
int TST_TOTAL = 1;
#define LONGSTR	(96*1024-1)
char tiat[] = "This is a test of the string functions.  ";
char yat[] = "This is yet another test.";
char tiatyat[] =
    "This is a test of the string functions.  This is yet another test.";
char dst1[LONGSTR + 1];
char dst2[LONGSTR + 1];
struct t_strlen {
	char *s;
	int e_res;
} t_len[] = {
	{
	"", 0}, {
	"12345", 5}, {
	tiat, 41}, {
	longstr, LONGSTR}, {
	NULL, 0}
};
struct t_index {
	char *s;
	char c;
	char *e_res;
} t_index[] = {
	{
	"", 'z', NULL}, {
	tiat, 'a', tiat + 8}, {
	tiat, 's', tiat + 3}, {
	tiat, 'o', tiat + 15}, {
	tiat, 'z', NULL}, {
	NULL, 0, NULL}
};
struct t_rindex {
	char *s;
	char c;
	char *e_res;
} t_rindex[] = {
	{
	"", 'z', NULL}, {
	tiat, 'a', tiat + 8}, {
	tiat, 's', tiat + 37}, {
	tiat, 'o', tiat + 35}, {
	tiat, 'z', NULL}, {
	NULL, 0, NULL}
};
struct t_strcmp {
	char *s1;
	char *s2;
	int e_res;
} t_cmp[] = {
	{
	"", "", 0}, {
	"", tiat, -((int)'T')}, {
	tiat, "", 'T'}, {
	tiat, tiat, 0}, {
	yat, tiat, 'y' - 'a'}, {
	NULL, NULL, 0}
};
struct t_strcat {
	char *s1;
	char *s2;
	char *s1s2;
	int e_res;
} t_cat[] = {
	{
	dst0, "", "", 0}, {
	dst0, tiat, tiat, 0}, {
	dst0, "", tiat, 0}, {
	dst0, yat, tiatyat, 0}, {
	dst1, longstr, longstr, 0}, {
	NULL, NULL, NULL, 0}
};
struct t_strcpy {
	char *s1;
	char *s2;
	int e_res;
} t_cpy[] = {
	{
	dst0, "", 0}, {
	dst0, tiat, 0}, {
	dst0, longstr, 0}, {
	NULL, NULL, 0}
};
struct t_strncmp {
	char *s1;
	char *s2;
	int n;
	int e_res;
} t_ncmp[] = {
	{
	"", "", 0, 0, 0}, {
	"", "", 80, 0, 0}, {
	tiat, "", 0, 0, 0}, {
	"", tiat, 80, -((int)'T'), -1}, {
	tiat, "", 80, 'T', 1}, {
	tiat, tiat, 80, 0, 0}, {
	yat, tiat, 80, 'y' - 'a', 1}, {
	yat, tiat, 8, 0, 1}, {
	yat, tiat, 9, 'y' - 'a', 1}, {
	NULL, NULL, 0, 0, 0}
};
struct t_strncat {
	char *s1;
	char *s2;
	int n;
	char *s1ns2;
	int e_res;
} t_ncat[] = {
	{
	dst0, "", LONGSTR, "", 0}, {
	dst0, tiat, LONGSTR, tiat, 0}, {
	dst0, "", LONGSTR, tiat, 0}, {
	dst0, yat, LONGSTR, tiatyat, 0}, {
	dst1, longstr, LONGSTR, longstr, 0},
	{
	dst2, longstr, 0, "", 0}, {
	dst2, longstr, 1, "t", 0}, {
	dst2, longstr, LONGSTR - 1, longstr, 0}, {
	NULL, NULL, 0, NULL, 0}
};
struct t_strncpy {
	char *s1;
	char *s2;
	int n;
	char *s1n;
	int e_res;
} t_ncpy[] = {
	{
	dst0, "", LONGSTR, "", 0}, {
	dst0, tiat, LONGSTR, tiat, 0}, {
	dst0, longstr, LONGSTR, longstr, 0},
	{
	dst1, tiat, 0, "", 0}, {
	dst1, longstr, 5, "ttttt", 0}, {
	NULL, NULL, 0, NULL, 0}
};
void setup();
int blenter();
int blexit();
int anyfail();
void setup(void)
{
	temp = stderr;
}
int blenter(void)
{
	local_flag = PASSED;
	return 0;
}
int blexit(void)
{
	(local_flag == PASSED) ? tst_resm(TPASS,
					  "Test passed") : tst_resm(TFAIL,
								    "Test failed");
	return 0;
}
int anyfail(void)
{
	tst_exit();
}
int main(int argc, char *argv[])
{
	register int n, i;
	char *s, *pr;
	tst_parse_opts(argc, argv, NULL, NULL);
	 * Init longstr
	 */
	s = longstr;
	n = LONGSTR;
	while (n--)
		*s++ = 't';
	setup();
	 * Index
	 */
	i = 0;
	while (t_index[i].s) {
		if ((pr =
		     strchr(t_index[i].s, t_index[i].c)) != t_index[i].e_res) {
			fprintf(temp, "(Strchr) test %d", i);
			local_flag = FAILED;
		}
		i++;
	}
	 * Strrchr
	 */
	i = 0;
	while (t_rindex[i].s) {
		if ((pr = strrchr(t_rindex[i].s, t_rindex[i].c))
		    != t_rindex[i].e_res) {
			fprintf(temp, "(Strrchr) test %d", i);
			local_flag = FAILED;
		}
		i++;
	}
	 * Strlen
	 */
	i = 0;
	while (t_len[i].s) {
		if ((n = strlen(t_len[i].s)) != t_len[i].e_res) {
			fprintf(temp, "(Strlen) test %d: expected %d, got %d",
				i, t_len[i].e_res, n);
			local_flag = FAILED;
		}
		i++;
	}
	 * Strcmp
	 */
	i = 0;
#define sign(x) ((x) < 0 ? -1 : ((x) > 0 ? 1 : 0))
	while (t_cmp[i].s1) {
		n = strcmp(t_cmp[i].s1, t_cmp[i].s2);
		if (sign(n) != sign(t_cmp[i].e_res)) {
			fprintf(temp, "(Strcmp) test %d: expected %d, got %d",
				i, sign(t_cmp[i].e_res), sign(n));
			local_flag = FAILED;
		}
		i++;
	}
	 * Strcat
	 */
	i = 0;
	while (t_cat[i].s1) {
		if ((n =
		     strcmp(strcat(t_cat[i].s1, t_cat[i].s2), t_cat[i].s1s2))
		    != t_cat[i].e_res) {
			fprintf(temp, "(Strcat) test %d: expected %d, got %d",
				i, t_cat[i].e_res, n);
			local_flag = FAILED;
		}
		i++;
	}
	 * Strcpy
	 */
	i = 0;
	while (t_cpy[i].s1) {
		if ((n = strcmp(strcpy(t_cpy[i].s1, t_cpy[i].s2), t_cpy[i].s2))
		    != t_cpy[i].e_res) {
			fprintf(temp, "(Strcpy) test %d: expected %d, got %d",
				i, t_cpy[i].e_res, n);
			local_flag = FAILED;
		}
		i++;
	}
	 * Strncat
	 */
	i = 0;
	while (t_ncat[i].s1) {
		if ((n =
		     strcmp(strncat(t_ncat[i].s1, t_ncat[i].s2, t_ncat[i].n),
			    t_ncat[i].s1ns2)) != t_ncat[i].e_res) {
			fprintf(temp, "(Strncat) test %d: expected %d, got %d",
				i, t_ncat[i].e_res, n);
			local_flag = FAILED;
		}
		i++;
	}
	 * Strncmp
	 */
	i = 0;
	while (t_ncmp[i].s1) {
		if ((n = strncmp(t_ncmp[i].s1, t_ncmp[i].s2, t_ncmp[i].n))
		    != t_ncmp[i].e_res) {
			if ((t_ncmp[i].a_res < 0 && n > t_ncmp[i].a_res)
			    || (t_ncmp[i].a_res > 0 && n < t_ncmp[i].a_res)) {
				fprintf(temp,
					"(Strncmp) test %d: expected %d, got %d",
					i, t_ncmp[i].e_res, n);
				local_flag = FAILED;
			}
		}
		i++;
	}
	 * Strncpy
	 */
	i = 0;
	while (t_ncpy[i].s1) {
		if ((n =
		     strcmp(strncpy(t_ncpy[i].s1, t_ncpy[i].s2, t_ncpy[i].n),
			    t_ncpy[i].s1n)) != t_ncpy[i].e_res) {
			fprintf(temp, "(Strncpy) test %d: expected %d, got %d",
				i, t_ncpy[i].e_res, n);
			local_flag = FAILED;
		}
		i++;
	}
	blexit();
	anyfail();
	tst_exit();
}


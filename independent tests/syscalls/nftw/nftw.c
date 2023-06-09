#include "incl.h"
void setup(void);
void blenter(void);
void blexit(void);
void anyfail(void);
char progname[] = "nftw.c";
#define FAILED 0
#define PASSED 1
int local_flag = PASSED;
int block_number;
FILE *temp;
char *TCID = "nftw01";
int TST_TOTAL = 10;
int s2;
char ebuf[ERR_BUF_SIZ];
char *dirlist[NDIRLISTENTS];
int visit;
int next_fd[4];
pathdata pathdat[] = {
	{
	 "./tmp/data",
	 S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH,
	 DIR, ""}, {
		    "./tmp/byebye",
		    S_IRWXU | S_IRWXG | S_IROTH | S_IWOTH,
		    REG, "byebye!\n"}, {
					"./tmp/data/d333",
					S_IRWXU | S_IRWXG | S_IRWXO,
					DIR, ""}, {
						   "./tmp/data/d666",
						   S_IRWXU | S_IRWXG | S_IRWXO,
						   DIR, ""}, {
							      "./tmp/data/d777",
							      S_IRWXU | S_IRWXG
							      | S_IRWXO,
							      DIR, ""}, {
									 "./tmp/data/dirg",
									 S_IRWXU
									 |
									 S_IRWXG
									 |
									 S_IROTH
									 |
									 S_IWOTH,
									 DIR,
									 ""}, {
									       "./tmp/data/dirh",
									       S_IRWXU
									       |
									       S_IRWXG
									       |
									       S_IROTH
									       |
									       S_IWOTH,
									       DIR,
									       ""},
	{
	 "./tmp/data/dirl",
	 S_IRWXU | S_IRWXG | S_IROTH | S_IWOTH,
	 DIR, ""}, {
		    "./tmp/data/d333/errs",
		    S_IRWXU | S_IRWXG | S_IROTH | S_IWOTH,
		    REG, "Do not eat yellow snow!\n"}, {
							"./tmp/data/d666/errs",
							S_IRWXU | S_IRWXG |
							S_IROTH | S_IWOTH,
							REG,
							"Do not eat yellow snow!\n"},
	{
	 "./tmp/data/d777/errs",
	 S_IRWXU | S_IRWXG | S_IROTH | S_IWOTH,
	 REG, "Do not eat yellow snow!\n"}, {
					     "./tmp/data/dirg/filebad",
					     S_IRUSR | S_IWUSR | S_IRGRP |
					     S_IROTH,
					     REG, ""}, {
							"./tmp/data/dirg/fileok",
							S_IRUSR | S_IWUSR |
							S_IRGRP | S_IROTH,
							REG, ""}, {
								   "./tmp/data/dirg/symlink",
								   S_IRWXU |
								   S_IRWXG |
								   S_IRWXO,
								   SYM,
								   "../../byebye"},
	{
	 "./tmp/data/dirg/dir_left.1",
	 S_IRWXU | S_IRWXG | S_IROTH | S_IWOTH,
	 DIR, ""}, {
		    "./tmp/data/dirg/dir_left.1/dir_left.2",
		    S_IRWXU | S_IRWXG | S_IROTH | S_IWOTH,
		    DIR, ""}, {
			       "./tmp/data/dirg/dir_right.1",
			       S_IRWXU | S_IRWXG | S_IRWXO,
			       DIR, ""}, {
					  "./tmp/data/dirg/dir_left.1/dir_left.2/left.3",
					  S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP
					  | S_IROTH,
					  REG, ""}, {
						     "./tmp/data/dirh/dir_left.1",
						     S_IRWXU | S_IRWXG | S_IROTH
						     | S_IWOTH,
						     DIR, ""}, {
								"./tmp/data/dirh/dir_right.1",
								S_IRWXU |
								S_IRWXG |
								S_IROTH |
								S_IWOTH,
								DIR, ""}, {
									   "./tmp/data/dirh/dir_left.1/dir_left.2",
									   S_IRWXU
									   |
									   S_IRWXG
									   |
									   S_IROTH
									   |
									   S_IWOTH,
									   DIR,
									   ""},
	{
	 "./tmp/data/dirh/dir_left.1/dir_left.2/left.3",
	 S_IRWXU | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH,
	 REG, "left leaf\n"}, {
			       "./tmp/data/dirh/dir_right.1/dir_right.2",
			       S_IRWXU | S_IRWXG | S_IROTH | S_IWOTH,
			       DIR, ""}, {
					  "./tmp/data/dirh/dir_right.1/dir_right.2/right.3",
					  S_IRWXU | S_IRGRP | S_IWGRP | S_IROTH
					  | S_IWOTH,
					  REG, "right leaf\n"}, {
								 "./tmp/data/dirl/dir_left.1",
								 S_IRWXU |
								 S_IRWXG |
								 S_IROTH |
								 S_IWOTH,
								 DIR, ""}, {
									    "./tmp/data/dirl/dir_left.1/dir_left.2",
									    S_IRWXU
									    |
									    S_IRWXG
									    |
									    S_IROTH
									    |
									    S_IWOTH,
									    DIR,
									    ""},
	{
	 "./tmp/data/dirl/dir_left.1/dir_left.2/left.3",
	 0,
	 SYM, "../../../dirh"}, {
				 "./tmp/data/dirl/dir_right.1",
				 S_IRWXU | S_IRWXG | S_IROTH | S_IWOTH,
				 DIR, ""}, {
					    "./tmp/data/dirl/dir_right.1/dir_right.2",
					    S_IRWXU | S_IRWXG | S_IROTH |
					    S_IWOTH,
					    DIR, ""}, {
						       "./tmp/data/dirl/dir_right.1/dir_right.2/right.3",
						       0,
						       SYM, "../dir_right.2"}, {
										"./tmp/data/loop",
										0,
										SYM,
										"./loop"}
};
char *goodlist[] = {
	"/dirh",
	"/dirh/dir_left.1",
	"/dirh/dir_right.1",
	"/dirh/dir_left.1/dir_left.2",
	"/dirh/dir_right.1/dir_right.2",
	"/dirh/dir_left.1/dir_left.2/left.3",
	"/dirh/dir_right.1/dir_right.2/right.3"
};
struct list badlist[] = {
	{"/dirg", FTW_D},
	{"/dirg/dir_left.1", FTW_D},
	{"/dirg/filebad", FTW_F},
	{"/dirg/fileok", FTW_F},
	{"/dirg/symlink", FTW_SL},
	{"/dirg/dir_right.1", FTW_DNR},
	{"/dirg/dir_left.1/dir_left.2", FTW_D},
	{"/dirg/dir_left.1/dir_left.2/left.3", FTW_F},
};
struct list mnem[] = {
	{"FTW_F", FTW_F},
	{"FTW_D", FTW_D},
	{"FTW_DNR", FTW_DNR},
	{"FTW_NS", FTW_NS},
	{"FTW_SL", FTW_SL},
#ifndef __linux__
	{"FTW_DP", FTW_DP},
	{"FTW_SLN", FTW_SLN},
#endif
};
int npathdats, ngoods, nbads, nmnem;
int main(void)
{
	npathdats = ARRAY_SIZE(pathdat);
	ngoods = ARRAY_SIZE(goodlist);
	nbads = ARRAY_SIZE(badlist);
	nmnem = ARRAY_SIZE(mnem);
	setup_path();
	blenter();
#ifdef DEBUG
	fprintf(temp, "A call to int nftw(const char *path, int (*fn)(const\n");
	fprintf(temp, "char *, const struct stat *, int, struct FTW *), int\n");
	fprintf(temp, "depth, int flags) shall recursively descend the\n");
	fprintf(temp, "directory hierarchy rooted in path until it has\n");
	fprintf(temp,
		"traversed the whole tree, calling the function fn for\n");
	fprintf(temp, "each object in the directory tree, and return 0.\n\n");
#endif
	test1A();
	blexit();
	blenter();
#ifdef DEBUG
	fprintf(temp, "A call to int nftw(const char *path, int (*fn)(const\n");
	fprintf(temp, "char *, const struct stat *, int, struct FTW *), int\n");
	fprintf(temp, "depth, int flags) when flags contains FTW_PHYS shall\n");
	fprintf(temp, "not traverse symbolic links.\n\n");
#endif
	test2A();
	blexit();
	blenter();
#ifdef DEBUG
	fprintf(temp, "A call to int nftw(const char *path, int (*fn)(const\n");
	fprintf(temp, "char *, const struct stat *, int, struct FTW *), int\n");
	fprintf(temp,
		"depth, int flags) when flags does not contain FTW_PHYS\n");
	fprintf(temp,
		"shall follow links instead of reporting them and shall\n");
	fprintf(temp, "not report the same file twice.\n\n");
#endif
	test3A();
	blexit();
	blenter();
#ifdef DEBUG
	fprintf(temp, "A call to int nftw(const char *path, int (*fn)(const\n");
	fprintf(temp, "char *, const struct stat *, int, struct FTW *), int\n");
	fprintf(temp,
		"depth, int flags) when flags contains FTW_DEPTH shall\n");
	fprintf(temp, "report all files in a directory before reporting the\n");
	fprintf(temp, "directory.\n\n");
#endif
	test4A();
	blexit();
	blenter();
#ifdef DEBUG
	fprintf(temp, "A call to int nftw(const char *path, int (*fn)(const\n");
	fprintf(temp, "char *, const struct stat *, int, struct FTW *), int\n");
	fprintf(temp, "depth, int flags) when flags does not contain\n");
	fprintf(temp, "FTW_DEPTH shall report a directory before reporting\n");
	fprintf(temp, "the files in that directory.\n\n");
#endif
	test5A();
	blexit();
	blenter();
#ifdef DEBUG
	fprintf(temp, "A call to int nftw(const char *path, int (*fn)(const\n");
	fprintf(temp, "char *, const struct stat *, int, struct FTW *), int\n");
	fprintf(temp,
		"depth, int flags) when flags contains FTW_CHDIR shall\n");
	fprintf(temp,
		"change the current working directory to each directory\n");
	fprintf(temp, "as it reports files in that directory.\n\n");
#endif
	test6A();
	blexit();
	blenter();
#ifdef DEBUG
	fprintf(temp, "A call to int nftw(const char *path, int (*fn)(const\n");
	fprintf(temp, "char *, const struct stat *, int, struct FTW *), int\n");
	fprintf(temp, "depth, int flags) shall pass the path-name of the\n");
	fprintf(temp, "current object as the first argument of the function\n");
	fprintf(temp, "fn.\n\n");
#endif
	test7A();
	blexit();
	blenter();
#ifdef DEBUG
	fprintf(temp, "A call to int nftw(const char *path, int (*fn)(const\n");
	fprintf(temp, "char *, const struct stat *, int, struct FTW *), int\n");
	fprintf(temp, "depth, int flags) shall pass a pointer to a stat\n");
	fprintf(temp, "structure containing information about the current\n");
	fprintf(temp, "object as the second argument to fn.\n\n");
#endif
	test8A();
	blexit();
	blenter();
#ifdef DEBUG
	fprintf(temp, "A call to int nftw(const char *path, int (*fn)(const\n");
	fprintf(temp, "char *, const struct stat *, int, struct FTW *), int\n");
	fprintf(temp, "depth, int flags) shall pass FTW_F as the third\n");
	fprintf(temp,
		"argument of the function fn when the object is a file.\n\n");
#endif
	test9A();
	blexit();
	blenter();
#ifdef DEBUG
	fprintf(temp, "A call to int nftw(const char *path, int (*fn)(const\n");
	fprintf(temp, "char *, const struct stat *, int, struct FTW *), int\n");
	fprintf(temp, "depth, int flags) shall pass FTW_D as the third\n");
	fprintf(temp, "argument of the function fn when the object is a\n");
	fprintf(temp, "directory.\n\n");
#endif
	test10A();
	blexit();
	blenter();
#ifdef DEBUG
	fprintf(temp, "A call to int nftw(const char *path, int (*fn)(const\n");
	fprintf(temp, "char *, const struct stat *, int, struct FTW *), int\n");
	fprintf(temp, "depth, int flags) shall pass FTW_DP as the third\n");
	fprintf(temp, "argument of the function fn when the object is a\n");
	fprintf(temp, "directory and subdirectories have been visited.\n\n");
#endif
	test11A();
	blexit();
	blenter();
#ifdef DEBUG
	fprintf(temp, "A call to int nftw(const char *path, int (*fn)(const\n");
	fprintf(temp, "char *, const struct stat *, int, struct FTW *), int\n");
	fprintf(temp, "depth, int flags) shall pass FTW_SL as the third\n");
	fprintf(temp, "argument of the function fn when the object is a\n");
	fprintf(temp, "symbolic link.\n\n");
#endif
	test12A();
	blexit();
	blenter();
#ifdef DEBUG
	fprintf(temp, "A call to int nftw(const char *path, int (*fn)(const\n");
	fprintf(temp, "char *, const struct stat *, int, struct FTW *), int\n");
	fprintf(temp, "depth, int flags) shall pass FTW_SLN as the third\n");
	fprintf(temp, "argument of the function fn when the object is a\n");
	fprintf(temp, "symbolic link that does not name an existing file.\n\n");
#endif
	test13A();
	blexit();
	blenter();
#ifdef DEBUG
	fprintf(temp, "A call to int nftw(const char *path, int (*fn)(const\n");
	fprintf(temp, "char *, const struct stat *, int, struct FTW *), int\n");
	fprintf(temp, "depth, int flags) shall pass FTW_DNR as the third\n");
	fprintf(temp, "argument of the function fn when the object is a\n");
	fprintf(temp, "directory that cannot be read.\n\n");
#endif
	test14A();
	blexit();
	blenter();
#ifdef DEBUG
	fprintf(temp, "A call to int nftw(const char *path, int (*fn)(const\n");
	fprintf(temp, "char *, const struct stat *, int, struct FTW *), int\n");
	fprintf(temp, "depth, int flags) shall pass FTW_NS as the third\n");
	fprintf(temp,
		"argument of the function fn when stat() failed on the\n");
	fprintf(temp, "object because of lack of appropriate permission.\n\n");
#endif
	test15A();
	blexit();
	blenter();
#ifdef DEBUG
	fprintf(temp, "A call to int nftw(const char *path, int (*fn)(const\n");
	fprintf(temp, "char *, const struct stat *, int, struct FTW *), int\n");
	fprintf(temp, "depth, int flags) shall pass a structure which\n");
	fprintf(temp, "contains the offset into the pathname of the object\n");
	fprintf(temp, "and the depth relative to the root of the walk\n");
	fprintf(temp,
		"starting from 0 as the fourth argument of the function\n");
	fprintf(temp, "fn.\n\n");
#endif
	test16A();
	blexit();
	blenter();
#ifdef DEBUG
	fprintf(temp, "A call to int nftw(const char *path, int (*fn)(const\n");
	fprintf(temp, "char *, const struct stat *, int, struct FTW *), int\n");
	fprintf(temp, "depth, int flags) shall pass FTW_SL as the third\n");
	fprintf(temp, "argument to the function fn if and only if the\n");
	fprintf(temp, "FTW_PHYS flag is included in flags.\n\n");
#endif
	test17A();
	blexit();
	blenter();
#ifdef DEBUG
	fprintf(temp, "A call to int nftw(const char *path, int (*fn)(const\n");
	fprintf(temp, "char *, const struct stat *, int, struct FTW *), int\n");
	fprintf(temp, "depth, int flags) shall pass FTW_SLN as the third\n");
	fprintf(temp, "argument to the function fn if and only if the\n");
	fprintf(temp, "FTW_PHYS flag is not included in flags.\n\n");
#endif
	test18A();
	blexit();
	blenter();
#ifdef DEBUG
	fprintf(temp, "On a call to int nftw(const char *path, int\n");
	fprintf(temp, "(*fn)(const char *, const struct stat *, int, struct\n");
	fprintf(temp,
		"FTW *), int depth, int flags) when the third argument\n");
	fprintf(temp, "passed to the function fn is FTW_DNR then the\n");
	fprintf(temp,
		"descendants of the directory shall not be processed.\n\n");
#endif
	test19A();
	blexit();
	blenter();
#ifdef DEBUG
	fprintf(temp, "A call to int nftw(const char *path, int (*fn)(const\n");
	fprintf(temp, "char *, const struct stat *, int, struct FTW *), int\n");
	fprintf(temp,
		"depth, int flags) shall close any file descriptors or\n");
	fprintf(temp,
		"directory streams used to traverse the directory tree.\n\n");
#endif
	test20A();
	blexit();
	blenter();
#ifdef DEBUG
	fprintf(temp, "On a call to int nftw(const char *path, int\n");
	fprintf(temp, "(*fn)(const char *, const struct stat *, int, struct\n");
	fprintf(temp, "FTW *), int depth, int flags) depth shall be the\n");
	fprintf(temp,
		"maximum number of file descriptors used for the search.\n\n");
#endif
	test21A();
	blexit();
	blenter();
#ifdef DEBUG
	fprintf(temp, "A call to int nftw(const char *path, int (*fn)(const\n");
	fprintf(temp, "char *, const struct stat *, int, struct FTW *), int\n");
	fprintf(temp, "depth, int flags) shall use at most one file\n");
	fprintf(temp, "descriptor for each directory level.\n\n");
#endif
	test22A();
	blexit();
	blenter();
#ifdef DEBUG
	fprintf(temp, "A call to int nftw(const char *path, int (*fn)(const\n");
	fprintf(temp, "char *, const struct stat *, int, struct FTW *), int\n");
	fprintf(temp, "depth, int flags) when the function fn returns a\n");
	fprintf(temp, "non-zero value shall stop and return the value\n");
	fprintf(temp, "returned by fn.\n\n");
#endif
	test23A();
	blexit();
	blenter();
#ifdef DEBUG
	fprintf(temp, "ENAMETOOLONG in errno and return -1 on a call to int\n");
	fprintf(temp, "nftw(const char *path, int (*fn)(const char *, const\n");
	fprintf(temp, "struct stat *, int, struct FTW *), int depth, int\n");
	fprintf(temp, "flags) when the length of path exceeds PATH_MAX.\n\n");
#endif
	test24A();
	blexit();
	blenter();
#ifdef DEBUG
	fprintf(temp, "ENAMETOOLONG in errno and return -1 on a call to int\n");
	fprintf(temp, "nftw(const char *path, int (*fn)(const char *, const\n");
	fprintf(temp, "struct stat *, int, struct FTW *), int depth, int\n");
	fprintf(temp, "flags) when a component of path exceeds NAME_MAX.\n\n");
#endif
	test25A();
	blexit();
	blenter();
#ifdef DEBUG
	fprintf(temp, "ENOENT in errno and return -1 on a call to int\n");
	fprintf(temp, "nftw(const char *path, int (*fn)(const char *, const\n");
	fprintf(temp, "struct stat *, int, struct FTW *), int depth, int\n");
	fprintf(temp,
		"flags) when path points to a file which does not exist.\n\n");
#endif
	test26A();
	blexit();
	blenter();
#ifdef DEBUG
	fprintf(temp, "ENOENT in errno and return -1 on a call to int\n");
	fprintf(temp, "nftw(const char *path, int (*fn)(const char *, const\n");
	fprintf(temp, "struct stat *, int, struct FTW *), int depth, int\n");
	fprintf(temp, "flags) when path points to an empty string.\n\n");
#endif
	test27A();
	blexit();
	blenter();
#ifdef DEBUG
	fprintf(temp, "ENOTDIR in errno and return -1 on a call to int\n");
	fprintf(temp, "nftw(const char *path, int (*fn)(const char *, const\n");
	fprintf(temp, "struct stat *, int, struct FTW *), int depth, int\n");
	fprintf(temp, "flags) when path is not a directory.\n\n");
#endif
	test28A();
	blexit();
	blenter();
#ifdef DEBUG
	fprintf(temp, "EACCES in errno and return -1 on a call to int\n");
	fprintf(temp, "nftw(const char *path, int (*fn)(const char *, const\n");
	fprintf(temp, "struct stat *, int, struct FTW *), int depth, int\n");
	fprintf(temp, "flags) when search permission is denied for any\n");
	fprintf(temp, "component of path.\n\n");
#endif
	test29A();
	blexit();
	blenter();
#ifdef DEBUG
	fprintf(temp, "EACCES in errno and return -1 on a call to int\n");
	fprintf(temp, "nftw(const char *path, int (*fn)(const char *, const\n");
	fprintf(temp, "struct stat *, int, struct FTW *), int depth, int\n");
	fprintf(temp, "flags) when read permission is denied for path.\n\n");
#endif
	test30A();
	blexit();
	cleanup_function();
	tst_exit();
}
void setup(void)
{
	temp = stderr;
	if ((ltpuser = getpwnam("nobody")) == NULL) {
		perror("nobody not found in /etc/passwd");
		exit(1);
	}
	setuid(ltpuser->pw_uid);
	tst_tmpdir();
}
void blenter(void)
{
	local_flag = PASSED;
	return;
}
void blexit(void)
{
	(local_flag == PASSED) ? tst_resm(TPASS, "Test block %d", block_number)
	    : tst_resm(TFAIL, "Test block %d", block_number);
	block_number++;
	return;
}
void anyfail(void)
{
	(local_flag == FAILED) ? tst_resm(TFAIL, "Test failed")
	    : tst_resm(TPASS, "Test passed");
	tst_rmdir();
	tst_exit();
}


#include "incl.h"
extern pathdata pathdat[];
extern struct list mnem[];
extern char ebuf[ERR_BUF_SIZ];
extern int npathdats, ngoods, nbads, nmnem;
extern FILE *temp;
void fail_exit(void);
void cleanup_function(void)
{
	chmod("./tmp/data/d333", (mode_t) S_IRWXU | S_IRWXG | S_IRWXO);
	chmod("./tmp/data/d666", (mode_t) S_IRWXU | S_IRWXG | S_IRWXO);
	chmod("./tmp/data/dirg/dir_right.1", (mode_t) S_IRWXU | S_IRWXG |
	      S_IRWXO);
	system("rm -rf ./tmp");
	wait(NULL);
}
void setup_path(void)
{
	int i, fd;
	FILE *temp = stderr;
	if (mkdir("./tmp", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1) {
		sprintf(ebuf, "Can't mkdir ./tmp");
		perror(ebuf);
		fprintf(temp, "ERROR: setup_path function failed\n");
		fail_exit();
	}
	for (i = 0; i < npathdats; i++) {
		if (pathdat[i].type == DIR) {
			if (mkdir(pathdat[i].name, pathdat[i].mode) == -1) {
				sprintf(ebuf, "Can't mkdir %s %d",
					pathdat[i].name, i);
				perror(ebuf);
				fprintf(temp,
					"ERROR: setup_path function failed\n");
				cleanup_function();
				fail_exit();
			}
		} else if (pathdat[i].type == SYM) {
			if (symlink(pathdat[i].contents, pathdat[i].name) == -1) {
				sprintf(ebuf, "Can't symlink %s ",
					pathdat[i].name);
				perror(ebuf);
				fprintf(temp,
					"ERROR: setup_path function failed\n");
				cleanup_function();
				fail_exit();
			}
		} else {
			if ((fd = open(pathdat[i].name, O_WRONLY | O_CREAT,
				       pathdat[i].mode)) == -1) {
				sprintf(ebuf, "Can't open %s", pathdat[i].name);
				perror(ebuf);
				fprintf(temp,
					"ERROR: setup_path function failed\n");
				cleanup_function();
				fail_exit();
			}
			if (write(fd, pathdat[i].contents,
				  strlen(pathdat[i].contents)) == -1) {
				perror("Can't write");
				close(fd);
				fprintf(temp,
					"ERROR: setup_path function failed\n");
				cleanup_function();
				fail_exit();
			}
			close(fd);
		}
	}
	if (chmod("./tmp/data/d333", (mode_t) S_IWUSR | S_IXUSR | S_IWGRP |
		  S_IXGRP | S_IWOTH | S_IXOTH) == -1) {
		sprintf(ebuf, "Can't chmod %s ", "./tmp/data/d333");
		perror(ebuf);
		fprintf(temp, "ERROR: setup_path function failed\n");
		cleanup_function();
		fail_exit();
	}
	if (chmod("./tmp/data/d666", (mode_t) S_IRUSR | S_IWUSR | S_IRGRP |
		  S_IWGRP | S_IROTH | S_IWOTH) == -1) {
		sprintf(ebuf, "Can't chmod %s ", "./tmp/data/d666");
		perror(ebuf);
		fprintf(temp, "ERROR: setup_path function failed\n");
		cleanup_function();
		fail_exit();
	}
	if (chmod("./tmp/data/dirg/dir_right.1", (mode_t) S_IWUSR | S_IXUSR |
		  S_IWGRP | S_IXGRP | S_IWOTH | S_IXOTH) == -1) {
		sprintf(ebuf, "Can't chmod %s ", "./tmp/data/dirg/dir_right.1");
		perror(ebuf);
		fprintf(temp, "ERROR: setup_path function failed\n");
		cleanup_function();
		fail_exit();
	}
}
int nftw_fn(const char *path, const struct stat *st, int ival, struct FTW *FTWS)
{
	return (0);
}
int callback(const char *path)
{
	return nftw(path, nftw_fn, 10, FTW_MOUNT);
}
char *ftw_mnemonic(int x)
{
	

char s[STRLEN];
	int i;
	for (i = 0; i < nmnem; i++)
		if (x == mnem[i].i)
			return (mnem[i].s);
	sprintf(s, "Unknown value for third argument to fn(): %d\n", x);
	return (s);
}
int getbase(const char *s)
{
	int i, last = 0;
	for (i = 0; *s != '\0'; s++, i++)
		if (*s == '/')
			last = i;
	return (last ? last + 1 : 0);
}
int getlev(const char *s)
{
	int i;
	for (i = 0; *s != '\0'; s++)
		if (*s == '/')
			i++;
	return (i);
}
void do_info(const char *path_name)
{
#ifdef DEBUG
	fprintf(temp, "INFO: Call to fn() at %s\n", path_name);
#endif
}
void fail_exit(void)
{
	tst_brkm(TFAIL, NULL, "Test failed");
}

void main(){
	setup();
	cleanup();
}

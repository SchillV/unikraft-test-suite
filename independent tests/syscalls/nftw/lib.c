#include "incl.h"
extern int s2;

char *tmp_path = "./tmp";

const char *no_file = "./tmp/no_such_file";

const char *is_a_file = "./tmp/is_a_file";
extern FILE *temp;
void remove_test_ENOTDIR_files(void)
{
	(void)unlink(is_a_file);
}
void remove_test_ENOENT_files(void)
{
	(void)unlink(no_file);
}

char *get_long_name_buffer(size_t * length, size_t extra)
{
	char *buffer;
	size_t path_length, name_length;
	temp = stderr;
	if ((path_length = pathconf(tmp_path, _PC_PATH_MAX)) == -1) {
		tst_resm(TFAIL, "pathconf(_PC_PATH_MAX) failed: %s",
			 strerror(errno));
		cleanup_function();
		fail_exit();
	}
	if ((name_length = pathconf(tmp_path, _PC_NAME_MAX)) == -1) {
		tst_resm(TFAIL, "pathconf(_PC_NAME_MAX) failed: %s",
			 strerror(errno));
		cleanup_function();
		fail_exit();
	}
	if ((strlen(tmp_path) + name_length + extra) > path_length) {
		tst_resm(TFAIL,
			 "pathconf(_PC_NAME_MAX)[=%zi] too large relative to pathconf(_PC_PATH_MAX)[=%zi]",
			 name_length, path_length);
		cleanup_function();
		fail_exit();
	}
	if ((buffer = malloc(path_length + extra)) == NULL) {
		tst_resm(TFAIL, "malloc(%zi) failed: %s", path_length + extra,
			 strerror(errno));
		cleanup_function();
		fail_exit();
	}
	return buffer;
}

void
execute_function(char *name, int (*callback) (const char *), char *buffer,
		 int expected)
{
	int result;
	temp = stderr;
#ifdef DEBUG
	fprintf(temp, "TEST: %s fails with ENAMETOOLONG\n", name);
#endif
	errno = 0;
	result = (*callback) (buffer);
	if (result == -752) {
		tst_resm(TFAIL, "%s callback did not work correctly", name);
		cleanup_function();
		fail_exit();
	}
	if (result != expected) {
		tst_resm(TFAIL,
			 "%s did not return value as expected; Expected=%d Received=%d",
			 name, expected, result);
		cleanup_function();
		fail_exit();
	}
	if (errno != ENAMETOOLONG) {
		tst_resm(TFAIL, "%s failed: errno should be %i but is %i", name,
			 ENAMETOOLONG, errno);
		cleanup_function();
		fail_exit();
	}
}

void
test_long_file_name(char *name, int (*callback) (const char *), int expected)
{
	char *ptr, *ptr_end, *buffer;
	size_t name_length;
	buffer = get_long_name_buffer(&name_length, 1);
	strcpy(buffer, tmp_path);
	ptr = buffer + strlen(buffer);
	ptr_end = ptr + name_length + 1;
	while (ptr <= ptr_end)
		*(ptr++) = 'E';
	execute_function(name, callback, buffer, expected);
	free(buffer);
}

void
test_long_component_name(char *name, int (*callback) (const char *),
			 int expected)
{
	char *ptr, *ptr_end, *buffer;
	size_t name_length;
	buffer = get_long_name_buffer(&name_length, 3);
	strcpy(buffer, tmp_path);
	ptr = buffer + strlen(buffer);
	ptr_end = ptr + name_length + 2;
	for (; ptr < ptr_end; ptr++)
		*ptr = 'E';
	execute_function(name, callback, buffer, expected);
	free(buffer);
}
void
test_ENAMETOOLONG_path(char *name, int (*callback) (const char *), int expected)
{
	size_t pcPathMax;
	char *path, *tmpPtr;
	int pathLength, tempPathLength;
	temp = stderr;
	if ((pcPathMax = pathconf(tmp_path, _PC_PATH_MAX)) == -1) {
		tst_resm(TFAIL, "pathconf(_PC_PATH_MAX) failed: %s",
			 strerror(errno));
		cleanup_function();
		fail_exit();
	}
#ifdef DEBUG
	fprintf(temp, "INFO: pathconf(_PC_PATH_MAX) for %s is %lu\n",
		tmp_path, pcPathMax);
#endif
	if ((path = malloc(pcPathMax + 2)) == NULL) {
		tst_resm(TFAIL, "malloc(%zu) for path failed: %s",
			 pcPathMax + 2, strerror(errno));
		cleanup_function();
		fail_exit();
	}
	path = strcpy(path, tmp_path);
	pathLength = (int)strlen(path);
	tempPathLength = pathLength + 1;
	pcPathMax = pcPathMax - tempPathLength - 5;
	tmpPtr = path + strlen(path);
	while (pathLength < pcPathMax) {
		tmpPtr += sprintf(tmpPtr, "/%s", tmp_path);
		pathLength += tempPathLength;
	}
	pcPathMax = pcPathMax + tempPathLength + 5;
	tmpPtr = path + pathLength;
	pathLength++;
	while (pathLength <= pcPathMax) {
		*tmpPtr++ = 'z';
		pathLength++;
	}
	pathLength = (int)strlen(path);
	if (pathLength != pcPathMax + 1) {
		tst_resm(TFAIL,
			 "test logic failure, path length is %d, should be %lu",
			 pathLength, (long unsigned int)pcPathMax + 1);
		free(path);
		cleanup_function();
		fail_exit();
	}
	execute_function(name, callback, path, expected);
	free(path);
}
void
test_ENAMETOOLONG_name(char *name, int (*callback) (const char *), int expected)
{
	test_long_file_name(name, callback, expected);
	test_long_component_name(name, callback, expected);
}
void test_ENOENT_empty(char *name, int (*callback) (const char *), int expected)
{
	char *empty_string;
	empty_string = "";
	errno = 0;
	temp = stderr;
#ifdef DEBUG
	fprintf(temp, "TEST: ENOENT when empty string is passed\n");
#endif
	if ((s2 = (*callback) (empty_string)) == expected) {
		if (errno != ENOENT) {
			tst_resm(TFAIL,
				 "%s failed: errno should be %i but is %i",
				 name, ENOENT, errno);
			cleanup_function();
			fail_exit();
		}
	} else {
		tst_resm(TFAIL,
			 "%s did not return correct value; Expected=%d Received=%d",
			 name, expected, s2);
		cleanup_function();
		fail_exit();
	}
}
void test_ENOTDIR(char *name, int (*callback) (const char *), int expected)
{
	int fd;
	if ((fd = creat(is_a_file, (mode_t) (S_IRWXU | S_IRWXG |
					     S_IRWXO))) == -1) {
		tst_resm(TFAIL, "creat(%s) failed: %s", is_a_file,
			 strerror(errno));
		cleanup_function();
		fail_exit();
	}
	if (close(fd) == -1) {
		tst_resm(TFAIL, "close(%i) failed: %s", fd, strerror(errno));
		remove_test_ENOTDIR_files();
		cleanup_function();
		fail_exit();
	}
	errno = 0;
	temp = stderr;
#ifdef DEBUG
	fprintf(temp, "TEST: ENOTDIR when a component is not a directory\n");
#endif
	s2 = (*callback) ("./tmp/is_a_file/no_file");
	if (s2 == -752) {
		remove_test_ENOTDIR_files();
		cleanup_function();
		fail_exit();
	}
	if (s2 == expected) {
		if (errno != ENOTDIR) {
			tst_resm(TFAIL,
				 "%s failed: errno should be %i but is %i",
				 name, ENOTDIR, errno);
			cleanup_function();
			fail_exit();
		}
	} else {
		tst_resm(TFAIL,
			 "%s did not return correct value; Expected=%d Received=%d",
			 name, expected, s2);
		cleanup_function();
		fail_exit();
	}
	remove_test_ENOTDIR_files();
}
void
test_ENOENT_nofile(char *name, int (*callback) (const char *), int expected)
{
	remove_test_ENOENT_files();
	temp = stderr;
#ifdef DEBUG
	fprintf(temp, "TEST: ENOENT when file does not exist\n");
#endif
	if ((s2 = (*callback) (no_file)) == expected) {
		if (errno != ENOENT) {
			tst_resm(TFAIL,
				 "%s failed: errno should be %i but is %i",
				 name, ENOENT, errno);
			cleanup_function();
			fail_exit();
		}
	} else {
		tst_resm(TFAIL,
			 "%s did not return correct value; Expected=%d Received=%d",
			 name, expected, s2);
		cleanup_function();
		fail_exit();
	}
}

void main(){
	setup();
	cleanup();
}

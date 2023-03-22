#include "incl.h"
   File:        epoll-ltp.c
   Description:
     Test the epoll_* system calls. This test program attempts to
     be very thorough in exercising epoll_* system calls. Large
     combinations of valid and invalid parameters are passed with
     valid and invalid sequences. Due to the combinatorial nature
     of this test program the test may take a "long" time to
     execute.
   Total Tests: 2 (2 system calls are being tested for)
   Test Name:   epoll_create, epoll_ctl
   Test Assertion
   & Strategy:  Test a variety of incorrect parameters for epoll_create
                Then run a reasonable epoll_create and get a fd for the epoll
                     set.
                Next run epoll_ctl on that fd (epoll_fd) with a variety of
                     incorrect parameters and a couple correct ones.
                Finally ?How to thoroughly test epoll_wait?
   Author:      Matt Helsley <matthltc@us.ibm.com>
   History:     Created - May 22 2003 - Matt Helsley <matthltc@us.ibm.com>
                Added   -
   Notes: Currently we assume that the OS will never allocate an fd s.t.
          fd == INT_MAX and that it will instead choose to allocate fds
          from the "low" numbers. -MH
   Currently pokes epoll_create several times in 2 + NUM_RAND_ATTEMPTS ways
             pokes epoll_ctl 27648 - (2 + NUM_RAND_ATTEMPTS) ways
             does not poke epoll_wait
   TODO: change errno test code to build lists of possible errno values for
            each erroneous parameter. Check that the errno value is in one
            of the lists. Currently errno is not checked at all when multiple
            erroneous parameters are passed in.
         test epoll_ctl with a large number of file descriptor events in the
            set
   Link against epoll and ltp (-lepoll -lltp)
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
char *TCID = "epoll01";
int TST_TOTAL = 1;
#ifdef HAVE_SYS_EPOLL_H
#if !defined(TRUE) && !defined(FALSE)
#define TRUE  1
#define FALSE 0
#endif
#define NUM_RAND_ATTEMPTS 16
#define BACKING_STORE_SIZE_HINT 32
  Define the beginning of a "protected region".
  This is a region where a wide variety of errors
  could occur or signals could arrive (including
  SIGSEGV and SIGKILL).
$
  The test program uses this to catch those
  conditions as best it can and continue testing.
  The region MUST be marked by a corresponding
  PROTECT_REGION_END.
  DO NOT nest protected regions! i.e. Do not build
  code of the form:
	PROTECT_REGION_START
              ...
	PROTECT_REGION_START
              ...
	PROTECT_REGION_END
              ...
	PROTECT_REGION_END
#define PROTECT_REGION_START		\
do {					\
	pid_t kid_pid;			\
	int kid_status;			\
					\
	tst_old_flush();			\
	kid_pid = FORK_OR_VFORK();	\
	if (kid_pid == 0) {
#define PROTECT_REGION_EXIT(errval) return (errval);
#define PROTECT_REGION_END(result, errval)					\
	return 0;								\
	} else {								\
	 waitpid(kid_pid, &kid_status, 0);					\
	 if (WIFEXITED(kid_status)) {						\
		(result) = WEXITSTATUS(kid_status);				\
		(result) = (errval);						\
		if (WIFSIGNALED(kid_status))						\
			tst_resm(TFAIL, "Protected function test exited due to signal %d (%s)", \
				WTERMSIG(kid_status), strsignal(WTERMSIG(kid_status)));	\
		}								\
	}									\
} while (0)
#define PROTECT_FUNC(fn, errval, epoll_fd) (					\
{										\
	pid_t kid_pid;								\
	int kid_status;								\
										\
	tst_old_flush();								\
	kid_pid = FORK_OR_VFORK();						\
		return fn(epoll_fd);						\
	} else {								\
		waitpid(kid_pid, &kid_status, 0);				\
		if (WIFEXITED(kid_status)) {					\
		kid_status = WEXITSTATUS(kid_status);				\
		kid_status = (errval);						\
		if (WIFSIGNALED(kid_status))					\
			tst_resm(TFAIL, "Protected function test exited due to signal %d (%s)", \
						WTERMSIG(kid_status), strsignal(WTERMSIG(kid_status))); \
	}									\
}										\
kid_status = kid_status;})
int test_epoll_create(unsigned int num_rand_attempts)
{
	int epoll_fd = -1;
	int fd_set_size = -1;
	unsigned int attempt_count;
	unsigned int num_epoll_create_test_fails = 0;
	unsigned int num_epoll_create_test_calls = 0;
	errno = 0;
	fd_set_size = -1;
	num_epoll_create_test_calls++;
	epoll_fd = epoll_create(fd_set_size);
	if (epoll_fd >= 0) {
		tst_resm(TFAIL | TERRNO,
			 "epoll_create with negative set size succeeded unexpectedly");
		num_epoll_create_test_fails++;
		close(epoll_fd);
	} else {
		if (errno != EINVAL) {
			tst_resm(TFAIL | TERRNO,
				 "epoll_create with negative set size didn't set errno to EINVAL");
			num_epoll_create_test_fails++;
		} else {
			tst_resm(TPASS, "epoll_create with negative set size");
		}
	}
	   small amount (expect num_rand_attempts to be approximately the
	   amount we'd like to go below INT_MAX). */
	fd_set_size = INT_MAX;
	for (attempt_count = num_rand_attempts; attempt_count > 0;
	     attempt_count--, fd_set_size--) {
		num_epoll_create_test_calls++;
		epoll_fd = epoll_create(fd_set_size);
		if (epoll_fd == -1) {
			if (errno != ENOMEM) {
				tst_resm(TFAIL,
					 "epoll_create with large set size (size = %d)",
					 fd_set_size);
				num_epoll_create_test_fails++;
			} else {
				tst_resm(TPASS,
					 "epoll_create with large set size (size = %d)",
					 fd_set_size);
			}
		} else {
			tst_resm(TPASS,
				 "epoll_create with large set size (size = %d)",
				 fd_set_size);
			close(epoll_fd);
		}
	}
	for (attempt_count = num_rand_attempts; attempt_count > 0;
	     attempt_count--) {
		fd_set_size = abs(rand() + SHRT_MAX) % INT_MAX;
		errno = 0;
		num_epoll_create_test_calls++;
		epoll_fd = epoll_create(fd_set_size);
		if (epoll_fd < 0) {
			if (errno != ENOMEM) {
				tst_resm(TFAIL,
					 "epoll_create with random random large set size (size = %d)",
					 fd_set_size);
				num_epoll_create_test_fails++;
			} else {
				tst_resm(TPASS,
					 "epoll_create with random random large set size (size = %d)",
					 fd_set_size);
			}
		} else {
			tst_resm(TPASS,
				 "epoll_create with random large set size (size = %d)",
				 fd_set_size);
			close(epoll_fd);
		}
	}
	tst_resm(TINFO,
		 "Summary: Of %d tests, epoll_create failed %d (%3.0f%% passed).",
		 num_epoll_create_test_calls, num_epoll_create_test_fails,
		 ((float)
		  (num_epoll_create_test_calls - num_epoll_create_test_fails)
		  * 100.0f / (float)
		  num_epoll_create_test_calls));
	return num_epoll_create_test_fails;
}
#define RES_PASS 0
#define RES_FAIL_RETV_MIS_ERRNO_MAT 1
#define RES_FAIL_RETV_BAD_ERRNO_MAT 2
#define RES_FAIL_RETV_MAT_ERRNO_MIS 3
#define RES_FAIL_RETV_BAD_ERRNO_MIS 4
#define RES_FAIL_RETV_MIS_ERRNO_IGN 5
#define RES_FAIL_RETV_BAD_ERRNO_IGN 6
#define RES_PASS_RETV_MAT_ERRNO_IGN 7

const char *result_strings[] = {
	"Passed",
	"Return value mismatched yet errno matched.",
	"Return value was bad    yet errno matched.",
	"Return value matched    yet errno mismatched.",
	"Return value was bad    and errno mismatched.",
	"Return value mismatched  so errno ignored.",
	"Return value was bad     so errno ignored.",
	"Return value matched    but errno ignored. (multiple errors expected)"
};
   failure message passed to it plus the parameters to the system call. */
#define EPOLL_CTL_TEST_RESULTS_SHOW_PARAMS 1
#if EPOLL_CTL_TEST_RESULTS_SHOW_PARAMS
#define EPOLL_CTL_TEST_FAIL(msg , ...) \
({ \
	if (ev_ptr != NULL) { \
		tst_resm(TFAIL, ( "(epoll_ctl(%d,%08x,%d,%p = {%08x,%08d}) returned %d:%s)" ) , ##__VA_ARGS__ , \
			epoll_fds[epfd_index], epoll_ctl_ops[op_index], \
			epoll_fds[fd_index], ev_ptr, ev_ptr->events, ev_ptr->data.fd, errno, \
			strerror(errno)); \
	} else { \
		tst_resm(TFAIL, ( "(epoll_ctl(%d,%08x,%d,%p) returned %d:%s)" ) , ##__VA_ARGS__  , \
			epoll_fds[epfd_index], epoll_ctl_ops[op_index], \
			epoll_fds[fd_index], ev_ptr, errno, strerror(errno)); \
	} \
})
#define EPOLL_CTL_TEST_PASS(msg , ...) \
({ \
	if (ev_ptr != NULL) { \
		tst_resm(TPASS, ( "(epoll_ctl(%d,%08x,%d,%p = {%08x,%08d}) returned %d:%s)" ) , ##__VA_ARGS__ , \
			epoll_fds[epfd_index], epoll_ctl_ops[op_index], \
			epoll_fds[fd_index], ev_ptr, ev_ptr->events, ev_ptr->data.fd, errno, \
			strerror(errno)); \
	} else { \
		tst_resm(TPASS, ( "(epoll_ctl(%d,%08x,%d,%p) returned %d:%s)" ) , ##__VA_ARGS__  , \
			epoll_fds[epfd_index], epoll_ctl_ops[op_index], \
			epoll_fds[fd_index], ev_ptr, errno, strerror(errno)); \
	} \
})
#else
#define EPOLL_CTL_TEST_FAIL(msg , ...) tst_resm(TFAIL, msg , ##__VA_ARGS__)
#define EPOLL_CTL_TEST_PASS(msg , ...) tst_resm(TPASS, msg , ##__VA_ARGS__)
#endif
int test_epoll_ctl(int epoll_fd)
{
	int fds[] = { -1, INT_MAX };
	int epoll_fds[] = { 0, -1, 0, INT_MAX };
	int epoll_events[64];
	int epoll_ctl_ops[] =
	    { EPOLL_CTL_DEL, EPOLL_CTL_MOD, EPOLL_CTL_ADD, EPOLL_CTL_MOD,
		EPOLL_CTL_DEL, EPOLL_CTL_MOD, EPOLL_CTL_DEL, INT_MAX, -1
	};
	struct epoll_event event;
	char event_mem[sizeof(struct epoll_event) * 2];
	struct epoll_event *unaligned_event_ptr;
				   lists. Second use is to iterate over the implicit
				   list of structs to pass in */
					   struct epoll_event parameter */
				   parameter */
	unsigned int num_epoll_ctl_test_fails = 0;
	unsigned int num_epoll_ctl_test_calls = 0;
	   Assume we know nothing about the EPOLL event types _except_
	   that they describe bits in a set. */
	for (index = 0; index < 64; index++) {
		epoll_events[index] = ((EPOLLIN * ((index & 0x01) >> 0)) |
				       (EPOLLOUT * ((index & 0x02) >> 1)) |
				       (EPOLLPRI * ((index & 0x04) >> 2)) |
				       (EPOLLERR * ((index & 0x08) >> 3)) |
				       (EPOLLHUP * ((index & 0x10) >> 4)) |
				       (EPOLLET * ((index & 0x20) >> 5)));
	}
	{
		char *unalign_ptr = event_mem;
		unalign_ptr =
		    unalign_ptr + (((unsigned long)unalign_ptr & 1) ? 0 : 1);
		unaligned_event_ptr = (struct epoll_event *)unalign_ptr;
	}
	epoll_fds[0] = epoll_fd;
	   take a while (in compute cycles). It took less than 1 minute to
	   run on a PIII 500 without checking the results. */
	for (index = 0; index < 3; index++) {
		struct epoll_event *ev_ptr = NULL;
		switch (index) {
			event.data.u64 = 0;
			ev_ptr = &event;
			break;
			unaligned_event_ptr->data.u64 = 0;
			ev_ptr = unaligned_event_ptr;
			break;
		case 2:
			ev_ptr = NULL;
			break;
		}
		for (epfd_index = 0;
		     epfd_index < (sizeof(epoll_fds) / sizeof(int));
		     epfd_index++) {
			for (event_index = 0;
			     event_index < (sizeof(epoll_events) / sizeof(int));
			     event_index++) {
				for (fd_index = 0;
				     fd_index < (sizeof(fds) / sizeof(int));
				     fd_index++) {
					   file descriptors we are interested in. Next we test epoll_ctl */
					for (op_index = 0;
					     op_index <
					     (sizeof(epoll_ctl_ops) /
					      sizeof(int)); op_index++) {
						int result;
						int expected_errno = 0;
						int num_errors_expected = 0;
						if (ev_ptr != NULL)
							ev_ptr->events =
							    epoll_events
							    [event_index];
						   returns -1 in the variable result if a protection violation
						   occurs (see PROTECT_REGION_END for the result) */
						PROTECT_REGION_START errno = 0;
						   a fork() call such that a subsequent fork() in the parent
						   will also manipulate the same set */
						result =
						    epoll_ctl(epoll_fds
							      [epfd_index],
							      epoll_ctl_ops
							      [op_index],
							      fds[fd_index],
							      ev_ptr);
						   the PROTECT_REGION hence we do not have a PROTECT_REGION_END
						   here */
						   Test the results. Look for appropriate error conditions
						 */
						if (epoll_fds[epfd_index] !=
						    epoll_fd) {
							if (epoll_fds
							    [epfd_index] == 0)
								expected_errno =
								    EINVAL;
								   neither epoll_fd nor stdin */
								expected_errno =
								    EBADF;
							num_errors_expected++;
						}
						switch (epoll_ctl_ops[op_index]) {
						case EPOLL_CTL_ADD:
						case EPOLL_CTL_MOD:
						case EPOLL_CTL_DEL:
							break;
							expected_errno = EINVAL;
							num_errors_expected++;
							break;
						}
						expected_errno = EPERM;
						num_errors_expected++;
						if (ev_ptr == NULL) {
							expected_errno = EINVAL;
							num_errors_expected++;
						} else if ((ev_ptr == &event)
							   || (ev_ptr ==
							       unaligned_event_ptr))
						{
							if (ev_ptr->events == 0) {
								expected_errno =
								    EINVAL;
								num_errors_expected++;
							}
							for (index = 1;
							     index < 64;
							     index++) {
								if ((int)ev_ptr->events != epoll_events[index]) {
									expected_errno
									    =
									    EINVAL;
									num_errors_expected++;
								}
							}
						} else {
						}
						if (num_errors_expected == 0) {
							if (result == 0) {
								return RES_PASS;
								return
								    RES_FAIL_RETV_MIS_ERRNO_IGN;
							} else {
								return
								    RES_FAIL_RETV_BAD_ERRNO_IGN;
							}
						} else if (num_errors_expected
							   == 1) {
							if (result == 0) {
							} else if (result == -1) {
								if (errno ==
								    expected_errno)
								{
								} else {
									return
									    RES_FAIL_RETV_MAT_ERRNO_MIS;
								}
							} else {
								   getting an error and check errno. */
								if (errno ==
								    expected_errno)
									return
									    RES_FAIL_RETV_BAD_ERRNO_MAT;
								else
									return
									    RES_FAIL_RETV_BAD_ERRNO_MIS;
							}
						} else if (num_errors_expected >
							   1) {
							if (result == 0) {
							} else if (result == -1) {
								if (errno ==
								    expected_errno)
								{
								} else {
									   without looking at the structure of the code. */
									return
									    RES_PASS_RETV_MAT_ERRNO_IGN;
								}
							} else {
								   getting an error and check errno. */
								if (errno ==
								    expected_errno)
									   happened to match what we expected. */
									return
									    RES_FAIL_RETV_BAD_ERRNO_MAT;
								else
									   without looking at the structure of the code. */
									return
									    RES_FAIL_RETV_BAD_ERRNO_IGN;
							}
						}
						   and PROTECT_REGION_END place their value in
						   the result parameter. If the region caused
						   a protection violation (segfault or otherwise)
						   then the result is set to the second parameter's
						   value (-1 in this case). */
						PROTECT_REGION_END(result, -1);
						num_epoll_ctl_test_calls++;
						if (!((result == RES_PASS)
						      || (result ==
							  RES_PASS_RETV_MAT_ERRNO_IGN)))
						{
							if (result >
							   (int)(sizeof(result_strings) /
							     sizeof(const char
								    *))) {
								EPOLL_CTL_TEST_FAIL
								    ("FIXME FIX ME BUG in Test Program itself!");
							} else {
									EPOLL_CTL_TEST_FAIL
									    ("Test arguments caused abnormal exit.");
									EPOLL_CTL_TEST_FAIL
									    ((result_strings[result]));
							}
							num_epoll_ctl_test_fails++;
#ifdef DEBUG
							EPOLL_CTL_TEST_PASS((result_strings[result]));
#else
						}
#endif
					}
				}
			}
		}
		close(epoll_fd);
	}
	tst_resm(TINFO,
		 "Summary: Of %d tests, epoll_ctl failed %d (%3.0f%% passed).",
		 num_epoll_ctl_test_calls, num_epoll_ctl_test_fails,
		 ((float)(num_epoll_ctl_test_calls - num_epoll_ctl_test_fails) *
		  100.0f / (float)num_epoll_ctl_test_calls));
	return (num_epoll_ctl_test_fails / num_epoll_ctl_test_calls);
}
int main(void)
{
	int epoll_fd;
	struct timeval tv;
	int last_result;
	tst_resm(TINFO, "testing if epoll() system call works");
	if (gettimeofday(&tv, NULL) != 0) {
		tst_brkm(TBROK | TERRNO, NULL, "gettimeofday failed");
	} else {
		tst_resm(TINFO, "gettimeofday() works");
	}
	srand(tv.tv_usec);
	tst_resm(TINFO,
		 "random number seeded with gettimeofday() [seed = %ld] works",
		 tv.tv_usec);
	tst_resm(TINFO, "Testing epoll_create");
	last_result = PROTECT_FUNC(test_epoll_create, -1, NUM_RAND_ATTEMPTS);
	if (last_result != 0) {
	}
	epoll_fd = epoll_create(BACKING_STORE_SIZE_HINT);
	if (epoll_fd < 0) {
		tst_brkm(TFAIL | TERRNO, NULL, "epoll_create failed");
	}
	tst_resm(TINFO, "Testing epoll_ctl");
	last_result = PROTECT_FUNC(test_epoll_ctl, -1, epoll_fd);
	if (last_result != 0) {
	}
	tst_exit();
}
#else
int main(void)
{
	tst_brkm(TCONF, NULL, "No epoll support found.");
}
#endif


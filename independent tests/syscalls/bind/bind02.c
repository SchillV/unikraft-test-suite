#include "incl.h"
#define TCP_PRIVILEGED_PORT 463
#define TEST_USERNAME "nobody"

void run(void)
{
	struct sockaddr_in servaddr;
	int sockfd;
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(TCP_PRIVILEGED_PORT);
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	TST_EXP_FAIL(bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)),
	             EACCES, "bind()");
	close(sockfd);
}

void setup(void)
{
	struct passwd *pw;
	struct group *gr;
	pw = getpwnam(TEST_USERNAME);
	gr = getgrgid(pw->pw_gid);
	tst_res(TINFO, "Switching credentials to user: %s, group: %s",
		pw->pw_name, gr->gr_name);
	setegid(gr->gr_gid);
	seteuid(pw->pw_uid);
}

void main(){
	setup();
	cleanup();
}

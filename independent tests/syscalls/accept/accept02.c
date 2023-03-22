#include "incl.h"
#define MULTICASTIP "224.0.0.0"
#define LOCALHOSTIP "127.0.0.1"

int server_sockfd;

int clone_server_sockfd;

int client_sockfd;

int server_port;

socklen_t addr_len;

struct sockaddr_in *server_addr;

struct sockaddr_in *client_addr;

struct group_req *mc_group;

void *server_thread(void *arg)
{
	int op, op_len, mc_group_len;
	op = 1;
	op_len = sizeof(op);
	mc_group_len = sizeof(*mc_group);
	server_sockfd = socket(AF_INET, SOCK_STREAM, 0);
	setsockopt(server_sockfd, SOL_SOCKET, SO_REUSEADDR, &op, op_len);
	setsockopt(server_sockfd, SOL_IP, MCAST_JOIN_GROUP,
			mc_group, mc_group_len);
	bind(server_sockfd, server_sockfd, struct sockaddr *)server_addr, addr_len);
	listen(server_sockfd, 1);
	TST_CHECKPOINT_WAKE(0);
accept(server_sockfd, (struct sockaddr *)client_addr, &addr_len);
	if (TST_RET == -1)
		tst_brk(TBROK | TTERRNO, "Could not accept connection");
	clone_server_sockfd = TST_RET;
setsockopt(clone_server_sockfd, SOL_IP, MCAST_LEAVE_GROU;
			mc_group, mc_group_len));
	close(clone_server_sockfd);
	if (TST_RET != -1)
		tst_res(TFAIL, "Multicast group was copied!");
	else if (TST_ERR == EADDRNOTAVAIL)
		tst_res(TPASS | TTERRNO, "Multicast group was not copied");
	else
		tst_brk(TBROK | TTERRNO, "setsockopt() failed unexpectedly");
	close(server_sockfd);
	return arg;
}

void *client_thread(void *arg)
{
	client_sockfd = socket(AF_INET, SOCK_STREAM, 0);
	bind(client_sockfd, client_sockfd, struct sockaddr *)client_addr, addr_len);
	connect(client_sockfd, client_sockfd, struct sockaddr *)server_addr, addr_len);
	close(client_sockfd);
	return arg;
}

void run(void)
{
	pthread_t server_thr, client_thr;
	server_addr->sin_port = server_port;
	client_addr->sin_port = htons(0);
	pthread_create(&server_thr, NULL, server_thread, NULL);
	TST_CHECKPOINT_WAIT(0);
	pthread_create(&client_thr, NULL, client_thread, NULL);
	pthread_join(server_thr, NULL);
	pthread_join(client_thr, NULL);
}

void setup(void)
{
	struct sockaddr_in *mc_group_addr;
	server_addr = tst_alloc(sizeof(*server_addr));
	client_addr = tst_alloc(sizeof(*client_addr));
	mc_group = tst_alloc(sizeof(*mc_group));
	mc_group->gr_interface = 0;
	mc_group_addr = (struct sockaddr_in *) &mc_group->gr_group;
	mc_group_addr->sin_family = AF_INET;
	inet_aton(MULTICASTIP, &mc_group_addr->sin_addr);
	server_addr->sin_family = AF_INET;
	inet_aton(LOCALHOSTIP, &server_addr->sin_addr);
	client_addr->sin_family = AF_INET;
	client_addr->sin_addr.s_addr = htons(INADDR_ANY);
	addr_len = sizeof(struct sockaddr_in);
	server_port = TST_GET_UNUSED_PORT(AF_INET, SOCK_STREAM);
	tst_res(TINFO, "Starting listener on port: %d", ntohs(server_port));
}

void cleanup(void)
{
	if (clone_server_sockfd > 0)
		close(clone_server_sockfd);
	if (client_sockfd > 0)
		close(client_sockfd);
	if (server_sockfd > 0)
		close(server_sockfd);
}

void main(){
	setup();
	cleanup();
}

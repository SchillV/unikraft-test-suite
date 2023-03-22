#include "incl.h"

struct sockaddr_un unix_addr = {
	.sun_family = AF_UNIX,
	.sun_path = MAIN_SOCKET_FILE
};

struct sockaddr_un abstract_addr = {
	.sun_family = AF_UNIX,
	.sun_path = ABSTRACT_SOCKET_PATH
};

struct sockaddr_un peer_addr = {
	.sun_family = AF_UNIX,
	.sun_path = PEER_SOCKET_FILE
};

struct sockaddr_in ipv4_addr;

struct sockaddr_in ipv4_any_addr;

struct sockaddr_in6 ipv6_addr;

struct sockaddr_in6 ipv6_any_addr;

struct test_case testcase_list[] = {
	{SOCK_DGRAM, 0, (struct sockaddr *)&unix_addr, sizeof(unix_addr),
		"AF_UNIX pathname datagram"},
	{SOCK_DGRAM, 0, (struct sockaddr *)&abstract_addr,
		sizeof(abstract_addr), "AF_UNIX abstract datagram"},
	{SOCK_DGRAM, 0, (struct sockaddr *)&ipv4_addr, sizeof(ipv4_addr),
		"IPv4 loop UDP variant 1"},
	{SOCK_DGRAM, IPPROTO_UDP, (struct sockaddr *)&ipv4_addr,
		sizeof(ipv4_addr), "IPv4 loop UDP variant 2"},
	{SOCK_DGRAM, IPPROTO_UDPLITE, (struct sockaddr *)&ipv4_addr,
		sizeof(ipv4_addr), "IPv4 loop UDP-Lite"},
	{SOCK_DGRAM, 0, (struct sockaddr *)&ipv4_any_addr,
		sizeof(ipv4_any_addr), "IPv4 any UDP variant 1"},
	{SOCK_DGRAM, IPPROTO_UDP, (struct sockaddr *)&ipv4_any_addr,
		sizeof(ipv4_any_addr), "IPv4 any UDP variant 2"},
	{SOCK_DGRAM, IPPROTO_UDPLITE, (struct sockaddr *)&ipv4_any_addr,
		sizeof(ipv4_any_addr), "IPv4 any UDP-Lite"},
	{SOCK_DGRAM, 0, (struct sockaddr *)&ipv6_addr, sizeof(ipv6_addr),
		"IPv6 loop UDP variant 1"},
	{SOCK_DGRAM, IPPROTO_UDP, (struct sockaddr *)&ipv6_addr,
		sizeof(ipv6_addr), "IPv6 loop UDP variant 2"},
	{SOCK_DGRAM, IPPROTO_UDPLITE, (struct sockaddr *)&ipv6_addr,
		sizeof(ipv6_addr), "IPv6 loop UDP-Lite"},
	{SOCK_DGRAM, 0, (struct sockaddr *)&ipv6_any_addr,
		sizeof(ipv6_any_addr), "IPv6 any UDP variant 1"},
	{SOCK_DGRAM, IPPROTO_UDP, (struct sockaddr *)&ipv6_any_addr,
		sizeof(ipv6_any_addr), "IPv6 any UDP variant 2"},
	{SOCK_DGRAM, IPPROTO_UDPLITE, (struct sockaddr *)&ipv6_any_addr,
		sizeof(ipv6_any_addr), "IPv6 any UDP-Lite"}
};

void setup(void)
{
	srand(time(0));
	tst_init_sockaddr_inet(&ipv4_addr, IPV4_ADDRESS, 0);
	tst_init_sockaddr_inet_bin(&ipv4_any_addr, INADDR_ANY, 0);
	tst_init_sockaddr_inet6_bin(&ipv6_addr, &in6addr_loopback, 0);
	tst_init_sockaddr_inet6_bin(&ipv6_any_addr, &in6addr_any, 0);
}

void *peer_thread(void *tc_ptr)
{
	const struct test_case *tc = tc_ptr;
	int sock;
	unsigned int request = 0;
	const char *response;
	sock = socket(tc->address->sa_family, tc->type, tc->protocol);
	 * Both sides of AF_UNIX/SOCK_DGRAM socket must be bound for
	 * bidirectional communication
	 */
	if (tc->address->sa_family == AF_UNIX)
		bind(sock, sock, struct sockaddr *)&peer_addr,
			sizeof(struct sockaddr_un));
	connect(sock, tc->address, tc->addrlen);
	write(1, sock, &request, sizeof1, sock, &request, sizeofrequest));
	read(1, sock, &request, sizeof1, sock, &request, sizeofrequest));
	if (request < ARRAY_SIZE(testcase_list))
		response = testcase_list[request].description;
	else
		response = "Invalid request value";
	write(1, sock, response, strlen1, sock, response, strlenresponse) + 1);
	close(sock);
	if (tc->address->sa_family == AF_UNIX)
		unlink(PEER_SOCKET_FILE);
	return NULL;
}

void test_bind(unsigned int n)
{
	struct test_case tc_copy, *tc = testcase_list + n;
	struct sockaddr_storage listen_addr, remote_addr;
	struct sockaddr_un *tmp_addr;
	socklen_t remote_len = sizeof(struct sockaddr_storage);
	int sock, size;
	unsigned int rand_index;
	pthread_t thread_id;
	char buffer[BUFFER_SIZE];
	const char *exp_data;
	tst_res(TINFO, "Testing %s", tc->description);
	sock = socket(tc->address->sa_family, tc->type, tc->protocol);
	TST_EXP_PASS_SILENT(bind(sock, tc->address, tc->addrlen), "bind()");
	if (!TST_PASS) {
		close(sock);
		return;
	}
	 * IPv4/IPv6 tests use wildcard addresses, resolve a valid connection
	 * address for peer thread
	 */
	memcpy(&tc_copy, tc, sizeof(struct test_case));
	tc_copy.addrlen = tst_get_connect_address(sock, &listen_addr);
	tc_copy.address = (struct sockaddr *)&listen_addr;
	pthread_create(&thread_id, NULL, peer_thread, &tc_copy);
	size = recvfrom(sock, &rand_index, sizeof(rand_index), 0,
		(struct sockaddr *)&remote_addr, &remote_len);
	if (size != sizeof(rand_index)) {
		close(sock);
		tst_brk(TBROK | TERRNO, "Error while waiting for connection");
	}
	rand_index = rand() % ARRAY_SIZE(testcase_list);
	sendto(1, sock, &rand_index, sizeof1, sock, &rand_index, sizeofrand_index), 0,
		(struct sockaddr *)&remote_addr, remote_len);
	size = read(0, sock, buffer, BUFFER_SIZE - 1);
	buffer[size] = '\0';
	exp_data = testcase_list[rand_index].description;
	if (!strcmp(buffer, exp_data))
		tst_res(TPASS, "Communication successful");
	else
		tst_res(TFAIL, "Received invalid data. Expected: \"%s\". "
			"Received: \"%s\"", exp_data, buffer);
	close(sock);
	pthread_join(thread_id, NULL);
	tmp_addr = (struct sockaddr_un *)tc->address;
	if (tc->address->sa_family == AF_UNIX && tmp_addr->sun_path[0])
		unlink(tmp_addr->sun_path);
}

void main(){
	setup();
	cleanup();
}

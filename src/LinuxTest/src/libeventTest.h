#ifndef LIB_EVENT_TEST
#define LIB_EVENT_TEST

#include <stdio.h>
#include <iostream>
#include <event.h>
#include <evdns.h>
#include <event2/dns_struct.h>
#include <arpa/inet.h>
// libeventͷ�ļ�
#include <event.h>
using namespace std;
#include <event2/dns.h>
#include <event2/util.h>
#include <event2/event.h>

#include <sys/socket.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

int n_pending_requests = 0;
struct event_base *base = NULL;

class user_data 
{
public:
	char *name; /* the name we're resolving */
	int idx; /* its position on the command line */
};

void callback(int errcode, struct evutil_addrinfo *addr, void *ptr)
{
	struct user_data *data =(struct user_data *)ptr;
	const char *name = data->name;
	if (errcode) {
		printf("%d. %s -> %s\n", data->idx, name, evutil_gai_strerror(errcode));
	}
	else {
		struct evutil_addrinfo *ai;
		printf("%d. %s", data->idx, name);
		if (addr->ai_canonname)
			printf(" [%s]", addr->ai_canonname);
		puts("");
		for (ai = addr; ai; ai = ai->ai_next) {
			char buf[128];
			const char *s = NULL;
			if (ai->ai_family == AF_INET) {
				struct sockaddr_in *sin = (struct sockaddr_in *)ai->ai_addr;
				s = evutil_inet_ntop(AF_INET, &sin->sin_addr, buf, 128);
			}
			else if (ai->ai_family == AF_INET6) {
				struct sockaddr_in6 *sin6 = (struct sockaddr_in6 *)ai->ai_addr;
				s = evutil_inet_ntop(AF_INET6, &sin6->sin6_addr, buf, 128);
			}
			if (s)
				printf("    -> %s\n", s);
		}
		evutil_freeaddrinfo(addr);
	}
	free(data->name);
	free(data);
	if (--n_pending_requests == 0)
		event_base_loopexit(base, NULL);
}

/* Take a list of domain names from the command line and resolve them in
* parallel. */
int dnsTest()
{
	int argc = 2;
	int i;
	struct evdns_base *dnsbase;

	base = event_base_new();
	if (!base)
		return 1;
	dnsbase = evdns_base_new(base, 1);
	if (!dnsbase)
		return 2;
	const char *addr = "www.baidu.com";
	const char *addr2 = "www.sina.com";
	const char *argv[2] = { addr,addr2 };
	for (i = 1; i <= argc; ++i) {
		struct evutil_addrinfo hints;
		struct evdns_getaddrinfo_request *req;
		memset(&hints, 0, sizeof(hints));
		hints.ai_family = AF_UNSPEC;
		hints.ai_flags = EVUTIL_AI_CANONNAME;
		/* Unless we specify a socktype, we'll get at least two entries for
		* each address: one for TCP and one for UDP. That's not what we
		* want. */
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_protocol = IPPROTO_TCP;

		user_data *pdata = new user_data();
		if (!(pdata->name = strdup(argv[i]))) {
			perror("strdup");
			exit(1);
		}
		pdata->idx = i;

		++n_pending_requests;
		req = evdns_getaddrinfo(
			dnsbase, argv[i], NULL /* no service name given */,
			&hints, callback, pdata);
		if (req == NULL) {
			printf("    [request for %s returned immediately]\n", argv[i]);
			/* No need to free user_data or decrement n_pending_requests; that
			* happened in the callback. */
		}
	}

	if (n_pending_requests)
		event_base_dispatch(base);

	evdns_base_free(dnsbase, 0);
	event_base_free(base);
	//getchar();
	return 0;
}


#endif
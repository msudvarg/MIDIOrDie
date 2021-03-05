#pragma once

#include <sys/un.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "scoped_thread.h"

class Socket_Client {

    int sfd;
	struct sockaddr_in addr;
    scoped_thread st;
    void (*f)(Socket_Client*);

public:

    //Exceptions
    class No_Socket {};
    class Invalid_IP {};
    class Connection_Error {};

    Socket_Client(const char * ipaddr, const int portno);
    ~Socket_Client();
};

Socket_Client::Socket_Client(const char * ipaddr, const int portno, void (*f_)(Socket_Client*)) :
    f {f_}
{

	sfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sfd == -1) {
		throw No_Socket{};
	}

	memset(&addr, 0, sizeof(struct sockaddr_in));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(portno);
	if(inet_aton(ipaddr, &addr.sin_addr) <= 0) {
		throw Invalid_IP{};
	}

	if (connect(sfd, (struct sockaddr*) &addr, sizeof(struct sockaddr_in)) == -1) {
		throw Connection_Error{};
	}

    std::thread t(f, this);
    st {t};

}

Socket_Client::~Socket_Client() {
    close(sfd);
}
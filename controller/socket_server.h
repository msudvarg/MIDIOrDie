#pragma once

#include <vector>
#include "socket_connection.h"
#include "socket_manifest.h"





class Socket_Server {

    const int max_clients;
    int sfd;
	struct sockaddr_in addr;
    std::vector<Socket_Connection> clients; 
    scoped_thread t;

public:

    //Exceptions
    class No_Socket {};
    class Invalid_IP {};
    class Bind_Error {};
    class Listen_Error {};

    Socket_Server(const char * ipaddr, const int portno, const int max_clients_);
    ~Socket_Server();
};

Socket_Server::Socket_Server(const char * ipaddr, const int portno, const int max_clients_):
    max_clients {max_clients_}
{
    if(max_clients > 0) clients.reserve(max_clients);

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

	if (bind(sfd, (struct sockaddr*) &addr, sizeof(struct sockaddr_in)) == -1) {
		throw Bind_Error{};
	}

    int backlog = max_clients > 0 ? max_clients : BACKLOG;
	if (listen(sfd, backlog) == -1) {
		throw Listen_Error{};
	}
}

Socket_Server::~Socket_Server() {
    close(sfd);
}
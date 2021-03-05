#include "socket_server.h"

//Constructor
Socket_Server::Socket_Server(
    const char * ipaddr,
    const int portno,
    void (*f_)(Socket_Connection*),
    const int max_clients_):
    //Initialize member variables
    max_clients {max_clients_},
    f {f_},
    running {true}
{

    //Open socket
	sfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sfd == -1) {
		throw No_Socket{};
	}

    //Convert string to IP address
	memset(&addr, 0, sizeof(struct sockaddr_in));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(portno);
	if(inet_aton(ipaddr, &addr.sin_addr) <= 0) {
		throw Invalid_IP{};
	}

    //Bind to socket
	if (bind(sfd, (struct sockaddr*) &addr, sizeof(struct sockaddr_in)) == -1) {
		throw Bind_Error{};
	}

    //Listen on socket
    int backlog = max_clients > 0 ? max_clients : BACKLOG;
	if (listen(sfd, backlog) == -1) {
		throw Listen_Error{};
	}

    //Run thread
    t = std::thread { [this] {this->accept_clients();} };
}

//Move constructor
Socket_Connection::Socket_Connection(Socket_Connection && other) noexcept :
    cfd {other.cfd},
    t {std::move(other.t)},
    running {other.running},
    f {other.f}
{}

//Move assignment operator
Socket_Connection & Socket_Connection::operator=(Socket_Connection && other) noexcept {
    cfd = other.cfd;
    t = std::move(other.t);
    running = other.running;
    f = other.f;
    return *this;
}

//Destructor: close socket, stop and join thread
Socket_Server::~Socket_Server() {
    //TODO: Modify for graceful closing of client connections
    close(sfd);
    running = false;
    if(t.joinable()) t.join();
}

//Accept new clients on socket
void Socket_Server::accept_clients() {
    
    //TODO: Modify so we only accept up to max_clients connections
    while(running) {
        int cfd = accept(sfd, NULL, NULL);
		if (cfd == -1) {
            running = false;
            return;
        }
        clients.emplace_back(cfd, f);
    }

}
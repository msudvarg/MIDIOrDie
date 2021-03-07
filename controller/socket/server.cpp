#include "server.h"

namespace Socket {

//Constructor
Server::Server(
    const char * ipaddr,
    const int portno,
    void (*f_)(Connection*),
    const int max_clients_) :
    //Construct parent 
    Socket(ipaddr, portno, f_),
    //Initialize member variables
    max_clients {max_clients_},
    running {true}
{

    //Bind to socket
	if (bind(sfd, (struct sockaddr*) &addr, sizeof(struct sockaddr_in)) == -1) {
		throw Bind_Error{};
	}

    //Listen on socket
    int backlog = max_clients > 0 ? max_clients : default_backlog;
	if (listen(sfd, backlog) == -1) {
		throw Listen_Error{};
	}

    //Run thread
    t = std::thread { [this] {this->accept_clients();} };
}

//Destructor: close socket, stop and join thread
Server::~Server() {
    //TODO: Modify for graceful closing of client connections
    close(sfd);
    running = false;
    if(t.joinable()) t.join();
}

//Accept new clients on socket
void Server::accept_clients() {
    
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

}
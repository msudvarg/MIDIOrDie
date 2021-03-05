#include "socket_client.h"

//Constructor
Socket_Client::Socket_Client(
    const char * ipaddr,
    const int portno,
    void (*f_)(Socket_Client*)) :
    //Initialize member variables
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

    //Connect to socket
	if (connect(sfd, (struct sockaddr*) &addr,sizeof(struct sockaddr_in)) == -1) {
		throw Connection_Error{};
	}

    //Run thread
    t = std::thread { [this] {this->run();} };

}

//Destructor: close socket, stop and join thread
Socket_Client::~Socket_Client() {
    close(sfd);
    running = false;
    if(t.joinable()) t.join();
}

//Send data over socket
void Socket_Client::send(void * buf, size_t count) {
    size_t remaining = count;
    while (remaining > 0) {
        ssize_t ret = write(sfd, buf, count);
        if (ret == -1) {
            running = false;
            return;
        }
        remaining -= ret;
    }
}

//Run registered function until socket disconnects
void Socket_Client::run() {
    while (running) {
        f(this);
    }
}
#include "client.h"
#include "connection.h"

namespace Socket {

//Constructor
Client::Client(
    const char * ipaddr,
    const int portno,
    void (*f_)(Connection*)) :
    //Construct parent 
    Socket(ipaddr, portno, f_)
{
    
    //Connect to socket
	if (connect(sfd, (struct sockaddr*) &addr,sizeof(struct sockaddr_in)) == -1) {
		throw Connection_Error{};
	}

    std::make_unique<Connection>(sfd, f);  

}

//Destructor: close socket, stop and join thread
Client::~Client() {
    close(sfd);
}

}
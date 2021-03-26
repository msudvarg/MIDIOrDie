#include "base.h"

namespace Socket {


//Constructor
Socket::Socket(
    const char * ipaddr,
    const int portno,
    void (*f_)(Connection*)) :
    f {f_}
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

}

}

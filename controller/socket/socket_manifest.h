#pragma once

namespace Socket {

constexpr int PORTNO = 31522;
constexpr char IPADDR[] = "127.0.0.1";
constexpr int BACKLOG = 10;


//Exceptions
class No_Socket {};
class Invalid_IP {};
class Bind_Error {};
class Listen_Error {};
class Connection_Error {};

}
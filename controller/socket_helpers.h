
#pragma once

#include <thread>
#include <chrono>
#include <iostream>
#include "common.h"
#include "shared_array.h"
#include "socket/socket.h"

//Socket manifest variables for default port/IP
constexpr char IPADDR[] = "0.0.0.0"; //Server binds to all available IPs
constexpr int PORTNO = 10520;

extern Shared_Array<double,ROLLING_WINDOW_SIZE> sharedArray;

void socket_send(Socket::Connection * client) {

    //Copy shared array to local array
    decltype(sharedArray)::array_type localArray = sharedArray.read();

    //Send local array over socket
    client->send(
        localArray.data(),
        sizeof(decltype(sharedArray)::value_type) * decltype(sharedArray)::size);

    //Sleep
    std::this_thread::sleep_for(std::chrono::milliseconds(50));

}

void socket_recv(Socket::Connection * client) {

    //Declare local array
    decltype(sharedArray)::array_type localArray;

    //Read from socket into local array
    client->recv(
        localArray.data(),
        sizeof(decltype(sharedArray)::value_type) * decltype(sharedArray)::size);

    //Copy local array to shared array
    sharedArray.write(localArray);
    
    std::this_thread::sleep_for(std::chrono::milliseconds(50));

}

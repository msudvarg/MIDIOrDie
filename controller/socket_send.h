
#pragma once

#include <thread>
#include <chrono>
#include "shared_buffer.h"

extern Shared_Array<double,ROLLING_WINDOW_SIZE> sharedArray;

void socket_send(Socket_Client * client) {

    decltype(sharedArray)::array_type localArray = sharedArray.read();
    client->send(localArray.data(), sizeof(decltype(sharedArray)::value_type) * decltype(sharedArray)::size);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    return;

}
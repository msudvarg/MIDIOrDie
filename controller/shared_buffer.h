#pragma once

#include <array>
#include <mutex>
template <typename T, unsigned N>
class Shared_Array {

public:
    using typename array_t = std::array<T, N>;

private:
    array_t buffer;
    std::mutex m;
    
	/*
	  This is a sequence lock:
	  We assume a single writer. It supports multiple readers.
	  The writer increments the sequence variable before writing,
	  so if it's odd we must restart the read.
	  Additionally, if the sequence variable is different before and after the read,
	  we must restart.

      Useful in situations where we want to share data
      with programs or scripts written in other languages,
      that don't guarantee similar mutex representations in shared memory.
	*/
    int lock_sequence = 0;

public:

    //Read using the mutex for synchronization
    array_t read();

    //Read using the sequence lock for synchronization
    array_t read_sequence();

    void write(array_t input);
    void write(T input[N]);
};

template <typename T, unsigned N>
Shared_Array<T,N>::array_t Shared_Array<T,N>::read() {
    std::scoped_lock lk {m};
    return buffer;
}

template <typename T, unsigned N>
Shared_Array<T,N>::array_t Shared_Array<T,N>::read_sequence() {
    array_t temp;
    int lock_before, lock_after;
        do {
            lock_before = lock_sequence;
            if (lock_before % 2) continue; //Odd indicates we are in a write stage
            temp = buffer;
            lock_after = lock_sequence;
        }
        while(lock_before != lock_after);
    return temp;
}

template <typename T, unsigned N>
void Shared_Array<T,N>::write(Shared_Array<T,N>::array_t input) {
    std::scoped_lock lk {m};
    lock_sequence++;
    buffer = input;
    lock_sequence++;
}

template <typename T, unsigned N>
void Shared_Array<T,N>::write(T input[N]) {
    std::scoped_lock lk {m};
    lock_sequence++;
    for(int i = 0; i < N; ++i) {
        buffer[i] = input[i];
    }
    lock_sequence++;
}
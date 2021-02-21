#pragma once

#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>

#include <string>

template <typename T>
class Shared_Memory {

private:
    std::string name;
    bool owner;
    int fd;
    T * shm;

    const mode_t mode = S_IRUSR | S_IWUSR; //User can read and write

public:
    Shared_Memory(const char * _name, bool _owner = true);
    ~Shared_Memory();

    T & operator*(void) const {
        return *shm;
    }
    T * operator->(void) const {
        return shm;
    }

    
    //Exceptions
    class Error_Open{};
    class Error_Resize{};
    class Error_Map{};
};

template <typename T>
Shared_Memory<T>::Shared_Memory(const char * _name, bool _owner) :
    name {_name}
{
    //Create 
    if (_owner) {
        fd = shm_open(_name, O_RDWR | O_CREAT | O_EXCL, mode);
        owner = true;
    }

    //Open
    else {
        //Try to open existing
        fd = shm_open(_name, O_RDWR, mode);

        //Does not exist, create and obtain ownership
        if (fd == -1) {
            fd = shm_open(_name, O_RDWR | O_CREAT, mode);
            owner = true;
        }

        //Does exist, not owner
        else {
            owner = false;
        }
    }

    //Could not open shared memory
    if (fd == -1) {
        throw Error_Open{};
    }

    if(owner && ftruncate(fd, sizeof(T)) == -1) {
        throw Error_Resize{};
    }

    shm = static_cast<T*>(mmap(NULL, sizeof(T), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0));
    if(shm == MAP_FAILED) {
        throw Error_Map{};
    }
}

template <typename T>
Shared_Memory<T>::~Shared_Memory() {
    close(fd);
    if(owner) {
        shm_unlink(name.c_str());
    } 
}


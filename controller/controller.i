%module controller

%{
    #define SWIG_FILE_WITH_INIT
    #include <iostream>
    #include "common.h"
    #include "Shared_Memory.h"
    static Shared_Memory<Shared_Buffer> sharedBuffer {"finalOutputBuffer"};
%}

%inline %{
    double *ReadFromSharedMemory() {
	return sharedBuffer->Read();
    }
%}

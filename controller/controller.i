%module controller

%{
    #define SWIG_FILE_WITH_INIT
    #include <iostream>
    #include "common.h"
    #include "Shared_Memory.h"
    #include "tone.h"
    static Shared_Memory<Shared_Buffer> sharedBuffer {"fftData"};
%}

%include "tone.h"

%newobject ToneFromSharedMemory;

%inline %{
    Tone *ToneFromSharedMemory() {
	return new Tone(sharedBuffer->Read());
    }
%}

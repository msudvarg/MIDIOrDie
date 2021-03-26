#define PY_SSIZE_T_CLEAN
#include "Python.h"

#include <iostream>

#include "../socket/socket.h" //Socket wrapper
#include "socket_manifest.h" //Functions to pass to socket connections
#include "shared_array.h" //Thread-safe array
#include "fft.h"
#include "poller.h"

//Thread-safe array to receive FFT data from socket
FFT::Shared_Array_t sharedArray;

//Function to receive from socket
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

//Socket server to receive FFT data from client
static Socket::Server socket_server {IPADDR, PORTNO, socket_recv};

//Open shared memory for visualization
//static Shared_Memory<Shared_Array> sharedBuffer {"fftData"}; 

static PyObject *
fft_histogram_histogram(PyObject *self, PyObject *args) {

    //Copy shared array to local array
    decltype(sharedArray)::array_type localArray = sharedArray.read();

    PyObject * pylist = PyList_New(FFT::OUTPUT_FFT_SIZE);

    for(size_t i = 0; i < FFT::OUTPUT_FFT_SIZE; ++i) {
        PyObject * pydouble = Py_BuildValue("d",localArray[i]);
        PyList_SetItem(pylist, i, pydouble);
    }
    
    return pylist;
}

static PyObject *
fft_histogram_bin_count(PyObject *self, PyObject *args) {
    PyObject * val = PyLong_FromLong(FFT::OUTPUT_FFT_SIZE);
    return val;
}

static PyObject *
fft_histogram_max_hz(PyObject *self, PyObject *args) {
    PyObject * val = PyLong_FromLong(FFT::OUTPUT_FFT_MAX_HZ);
    return val;
}

static PyMethodDef FFT_HistogramMethods[] = {
    {"histogram",  fft_histogram_histogram, METH_VARARGS,
     "Execute a shell command."},
    {"bin_count", fft_histogram_bin_count, METH_VARARGS,
     "Get size of incoming array"},
    {"max_hz", fft_histogram_max_hz, METH_VARARGS,
     "Get Hz value of last bucket"},
    {NULL, NULL, 0, NULL}        /* Sentinel */
};

static struct PyModuleDef fft_histogram = {
    PyModuleDef_HEAD_INIT,
    "fft_histogram",   /* name of module */
    NULL, /* module documentation, may be NULL */
    -1,       /* size of per-interpreter state of the module,
                 or -1 if the module keeps state in global variables. */
    FFT_HistogramMethods
};

PyMODINIT_FUNC
PyInit_fft_histogram(void)
{
    return PyModule_Create(&fft_histogram);
}

int
main(int argc, char *argv[])
{

    PyObject * pmodule;

    wchar_t *program = Py_DecodeLocale(argv[0], NULL);
    if (program == NULL) {
        fprintf(stderr, "Fatal error: cannot decode argv[0]\n");
        exit(1);
    }

    /* Add a built-in module, before Py_Initialize */
    if (PyImport_AppendInittab("fft_histogram", PyInit_fft_histogram) == -1) {
        fprintf(stderr, "Error: could not extend in-built modules table\n");
        exit(1);
    }

    /* Pass argv[0] to the Python interpreter */
    Py_SetProgramName(program);

    /* Initialize the Python interpreter.  Required.
       If this step fails, it will be a fatal error. */
    Py_Initialize();

    /* Optionally import the module; alternatively,
       import can be deferred until the embedded script
       imports it. */
    pmodule = PyImport_ImportModule("fft_histogram");
    if (!pmodule) {
        PyErr_Print();
        fprintf(stderr, "Error: could not import module 'fft_histogram'\n");
    }

    PyMem_RawFree(program);
    return 0;
}


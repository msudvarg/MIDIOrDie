#define PY_SSIZE_T_CLEAN
#include "Python.h"

#include <iostream>
#include <string.h>

#include "common.h"
#include "Shared_Memory.h"

//Open shared memory for visualization
static Shared_Memory<Shared_Buffer> sharedBuffer {"finalOutputBuffer"}; 

double finalOutputBuffer[ROLLING_WINDOW_SIZE];

static PyObject *
fft_histogram_histogram(PyObject *self, PyObject *args) {
    
    /*
    This is a sequence lock:
    We assume a single writer. It supports multiple readers.
    The writer increments the sequence variable before writing,
    so if it's odd we must restart the read.
    Additionally, if the sequence variable is different before and after the read,
    we must restart.
    */
    {
        int lock_before, lock_after;
        do {
            lock_before = sharedBuffer->lock_sequence;
            if (lock_before % 2) continue; //Odd indicates we are in a write stage
            memcpy(finalOutputBuffer, sharedBuffer->finalOutputBuffer, sizeof(finalOutputBuffer));        
            lock_after = sharedBuffer->lock_sequence;
        }
        while(lock_before != lock_after);
    }

    PyObject * pylist = PyList_New(ROLLING_WINDOW_SIZE);

    for(int i = 0; i < ROLLING_WINDOW_SIZE; ++i) {
        PyObject * pydouble = Py_BuildValue("d",finalOutputBuffer[i]);
        PyList_SetItem(pylist, i, pydouble);
    }
    
    return pylist;
}

static PyMethodDef FFT_HistogramMethods[] = {
    {"histogram",  fft_histogram_histogram, METH_VARARGS,
     "Execute a shell command."},
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


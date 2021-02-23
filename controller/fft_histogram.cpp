#define PY_SSIZE_T_CLEAN
#include "Python.h"

#include <iostream>

#include "common.h"
#include "Shared_Memory.h"

//Open shared memory for visualization
static Shared_Memory<Shared_Buffer> sharedBuffer {"fftData"}; 

static PyObject *
fft_histogram_histogram(PyObject *self, PyObject *args) {
    
    double *fftData = sharedBuffer->Read();

    PyObject * pylist = PyList_New(OUTPUT_FFT_SIZE);

    for(int i = 0; i < OUTPUT_FFT_SIZE; ++i) {
        PyObject * pydouble = Py_BuildValue("d",fftData[i]);
        PyList_SetItem(pylist, i, pydouble);
    }

    delete[] fftData;
    
    return pylist;
}

static PyObject *
fft_histogram_bin_count(PyObject *self, PyObject *args) {
    PyObject * val = PyLong_FromLong(OUTPUT_FFT_SIZE);
    return val;
}

static PyObject *
fft_histogram_max_hz(PyObject *self, PyObject *args) {
    PyObject * val = PyLong_FromLong(OUTPUT_FFT_MAX_HZ);
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


#include <stdio.h>
#include <Python.h>

int main(int argc, char *argv[]) {
    Py_Initialize();
    if (!Py_IsInitialized()) {
        return -1;
    }

    PyObject *sys = PyImport_ImportModule("sys");
    if (sys == NULL) {
        return -2;
    }

    PyObject *argv_list = PyList_New(argc);
    for (int i = 0; i < argc; ++i) {
        PyList_SetItem(argv_list, i, PyUnicode_FromString(argv[i]));
    }
    PyModule_AddObject(sys, "argv", argv_list);

    FILE *fp = fopen("src/main.py", "r");
    if (fp == NULL) {
        return -3;
    }
    PyRun_SimpleFile(fp, "imgToAscii.py");


    fclose(fp);
    Py_DECREF(sys);
    Py_Finalize();

    return 0;
}


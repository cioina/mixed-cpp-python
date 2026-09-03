#include <Python.h>

static PyObject* method_say_hello(PyObject* self, PyObject* args) {
    const char* name;
    if (!PyArg_ParseTuple(args, "s", &name)) return NULL;
    return PyUnicode_FromFormat("Hello, %s!", name);
};

static PyObject* method_inc(PyObject* self, PyObject* args)
{
    __int64 input_value;
    if (!PyArg_ParseTuple(args, "L", &input_value)) return NULL;
    return PyLong_FromLongLong(input_value + 1);
};

static PyObject* method_add(PyObject* self, PyObject* args) {
    int a, b;
    if (!PyArg_ParseTuple(args, "ii", &a, &b)) return NULL;
    return PyLong_FromLong(a + b);
};

static PyObject* method_div(PyObject* self, PyObject* args) {
    int a, b;
    if (!PyArg_ParseTuple(args, "ii", &a, &b)) return NULL;
    return PyLong_FromLong(a / b);
};

static PyMethodDef MyMethods[] = {
    {"div", (PyCFunction)method_div, METH_VARARGS, "Divide two numbers."},
    {"add", (PyCFunction)method_add, METH_VARARGS, "Add two numbers."},
    {"inc",(PyCFunction)method_inc, METH_VARARGS, "Plus one."},
    {"say_hello", (PyCFunction)method_say_hello, METH_VARARGS, "Greet someone."},
    {NULL, NULL, 0, NULL}  // Sentinel / Terminator
};

static struct PyModuleDef spam = {
    PyModuleDef_HEAD_INIT, "spam", NULL, -1, MyMethods
};

// NOLINTNEXTLINE(readability-make-member-function-static)
PyMODINIT_FUNC PyInit_spam(void) {
    return PyModule_Create(&spam);
};

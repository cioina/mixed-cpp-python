#include <Python.h>
#include <stdexcept>
#include "exprEval.cpp"

// Global pointer for your custom exception
static PyObject* CustomError = nullptr;

static PyObject* method_testConversion(PyObject* self, PyObject* args) {
    char* testPath;
    if (!PyArg_ParseTuple(args, "s", &testPath)) return nullptr;

    try {
        testConversion(testPath);
    }
    catch (const std::exception& e) {
        // Raise the custom Python exception with a message
        PyErr_SetString(CustomError, e.what());
        return nullptr; // Returning NULL signals an error indicator to Python
    }

    return PyBool_FromLong(1);
}

static PyObject* method_say_hello(PyObject* self, PyObject* args) {
    const char* name;
    if (!PyArg_ParseTuple(args, "s", &name)) return nullptr;
    return PyUnicode_FromFormat("Hello, %s!", name);
};

static PyObject* method_inc(PyObject* self, PyObject* args)
{
    __int64 input_value;
    if (!PyArg_ParseTuple(args, "L", &input_value)) return nullptr;
    return PyLong_FromLongLong(input_value + 1);
};

static PyObject* method_add(PyObject* self, PyObject* args) {
    int a, b;
    if (!PyArg_ParseTuple(args, "ii", &a, &b)) return nullptr;
    return PyLong_FromLong(a + b);
};

static PyObject* method_div(PyObject* self, PyObject* args) {
    int a, b;
    if (!PyArg_ParseTuple(args, "ii", &a, &b)) return nullptr;

    try {
        if (b == 0) {
            // Throw a C++ exception or handle logic directly
            throw std::runtime_error("Integer division by zero!");
        }
    }
    catch (const std::exception& e) {
        // Raise the custom Python exception with a message
        PyErr_SetString(CustomError, e.what());
        return nullptr; // Returning NULL signals an error indicator to Python
    }

    return PyLong_FromLong( a / b);
};

static PyMethodDef MyMethods[] = {
    {"div", (PyCFunction)method_div, METH_VARARGS, "Divide two numbers."},
    {"add", (PyCFunction)method_add, METH_VARARGS, "Add two numbers."},
    {"inc",(PyCFunction)method_inc, METH_VARARGS, "Plus one."},
    {"say_hello", (PyCFunction)method_say_hello, METH_VARARGS, "Greet someone."},
    {"test_conversion", (PyCFunction)method_testConversion, METH_VARARGS, "Test math expressions."},
    {nullptr, nullptr, 0, nullptr}  // Sentinel / Terminator
};

static struct PyModuleDef spam = {
    PyModuleDef_HEAD_INIT, "spam", nullptr, -1, MyMethods
};

// NOLINTNEXTLINE(readability-make-member-function-static)
PyMODINIT_FUNC PyInit_spam(void) {

    PyObject* m = PyModule_Create(&spam);
    if (!m) return nullptr;

    // Create the custom exception: 'mymodule.CustomError'
    // Passing NULL for base and dict makes it inherit from standard Exception
    CustomError = PyErr_NewException("mymodule.CustomError", nullptr, nullptr);
    if (!CustomError) {
        Py_DECREF(m);
        return nullptr;
    }

    // Add exception to module dictionary so Python code can catch it
    Py_INCREF(CustomError);
    PyModule_AddObject(m, "CustomError", CustomError);

    return m;
};

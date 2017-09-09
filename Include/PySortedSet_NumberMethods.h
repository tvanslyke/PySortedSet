#ifndef PY_SORTED_SET_NUMBER_METHODS_H
#define PY_SORTED_SET_NUMBER_METHODS_H
#include "PySortedSet.h"

PyAPI_FUNC(PyObject*) PySortedSet_arith_add(PyObject* self, PyObject* other);
PyAPI_FUNC(PyObject*) PySortedSet_arith_sub(PyObject* self, PyObject* other);
PyAPI_FUNC(PyObject*) PySortedSet_arith_and(PyObject* self, PyObject* other);
PyAPI_FUNC(PyObject*) PySortedSet_arith_or(PyObject* self, PyObject* other);
PyAPI_FUNC(PyObject*) PySortedSet_arith_xor(PyObject* self, PyObject* other);

PyAPI_FUNC(PyObject*) PySortedSet_arith_add_eq(PyObject* self, PyObject* other);
PyAPI_FUNC(PyObject*) PySortedSet_arith_sub_eq(PyObject* self, PyObject* other);
PyAPI_FUNC(PyObject*) PySortedSet_arith_and_eq(PyObject* self, PyObject* other);
PyAPI_FUNC(PyObject*) PySortedSet_arith_or_eq(PyObject* self, PyObject* other);
PyAPI_FUNC(PyObject*) PySortedSet_arith_xor_eq(PyObject* self, PyObject* other);



#endif /* PY_SORTED_SET_NUMBER_METHODS_H */

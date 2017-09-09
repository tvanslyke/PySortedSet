#ifndef PY_SORTED_SET_NAMED_METHODS_H
#define PY_SORTED_SET_NAMED_METHODS_H


#include "PySortedSet.h"

PyObject* PySortedSet_add(PyObject* self, PyObject* tup);
PyObject* PySortedSet_ClearSet(PySortedSetObject *self);
PyObject* PySortedSet_discard(PyObject* self, PyObject* tup);
PyObject* PySortedSet_intersection(PyObject* self, PyObject* tup);

PyObject* PySortedSet_intersection_update(PyObject* self, PyObject* tup);
PyObject* PySortedSet_difference(PyObject* self, PyObject* tup);
PyObject* PySortedSet_difference_update(PyObject* self, PyObject* argtup);
PyObject* PySortedSet_update(PyObject* self, PyObject* iter_tuple);
PyObject* PySortedSet_union(PyObject* self, PyObject* tup);
PyObject* PySortedSet_isdisjoint(PyObject* self, PyObject* other);
PyObject* PySortedSet_resort(PyObject* self);



#endif /* PY_SORTED_SET_NAMED_METHODS_H */

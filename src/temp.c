#ifndef PY_SORTED_SET_NAMED_METHODS_H
#define PY_SORTED_SET_NAMED_METHODS_H



PyAPI_FUNC(PyObject*) PySortedSet_add(PyObject* self, PyObject* tup);
PyAPI_FUNC(PyObject*) PySortedSet_ClearSet(PySortedSetObject *self);
PyAPI_FUNC(PyObject*) PySortedSet_discard(PyObject* self, PyObject* tup);
PyAPI_FUNC(PyObject*) PySortedSet_copy(PyObject* self);
PyAPI_FUNC(PyObject*) PySortedSet_difference(PyObject* self, PyObject* tup);
PyAPI_FUNC(PyObject*) PySortedSet_difference_update(PyObject* self, PyObject* argtup);
PyAPI_FUNC(PyObject*) PySortedSet_intersection(PyObject* self, PyObject* tup);
PyAPI_FUNC(PyObject*) PySortedSet_update(PyObject* self, PyObject* iter_tuple);
PyAPI_FUNC(PyObject*) PySortedSet_isdisjoint(PyObject* self, PyObject* other);
PyAPI_FUNC(PyObject*) PySortedSet_union(PyObject* self, PyObject* tup);
PyAPI_FUNC(PyObject*) PySortedSet_resort(PyObject* self);



#endif PY_SORTED_SET_NAMED_METHODS_H

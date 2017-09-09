#ifndef PY_SORTED_SET_MAPPING_METHODS
#define PY_SORTED_SET_MAPPING_METHODS


#include "PySortedSet.h"


PyAPI_FUNC(PyObject*) PySortedSet_subscript(PyObject* self, PyObject* index);


PyAPI_FUNC(int)  PySortedSet_assign_subscript(PyObject* self, PyObject* item, PyObject* value);







#endif /* PY_SORTED_SET_MAPPING_METHODS */

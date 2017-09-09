
#ifndef PY_SORTED_SET_INTERNAL_H
#define PY_SORTED_SET_INTERNAL_H

#include "PySortedSet.h"

#ifndef _PyList_ITEMS
#define _PyList_ITEMS(self) ((PyObject**)(((PyListObject*)(self))->ob_item))
#endif


PyObject* PySortedSet_AsFinalizedSortedSet(PyObject* obj);

int PySortedSet_NumberToIndex(PyObject* self, PyObject* index, Py_ssize_t* dest);

void PySortedSet_MOVE_ASSIGN_LIST(PyObject* self, PyObject* list);


void PySortedSet_MOVE_ASSIGN(PyObject* self, PyObject* other);
#endif /* PY_SORTED_SET_INTERNAL_H */

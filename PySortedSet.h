/*
 * PySortedSet.h
 *
 *  Created on: Sep 3, 2017
 *      Author: tim
 */

#ifndef PYSORTEDSET_H_
#define PYSORTEDSET_H_
#include <python2.7/Python.h>
#include <python2.7/structmember.h>
typedef struct py_sorted_set_{
	PyObject_HEAD
	PyListObject* sorted_set_;
	Py_ssize_t sorted_count_;
} PySortedSetObject;

inline int PySortedSet_LessThan(PyObject* a, PyObject* b){return PyObject_Compare(a, b) < 0;}
inline int PySortedSet_Equal(PyObject* a, PyObject* b){return !PyObject_Compare(a, b);}

#define PY_SORTED_SET_GET_LIST(set)   		(((PySortedSetObject*)(set))->sorted_set_)
#define PY_SORTED_SET_SORTED_COUNT(set)   	(((PySortedSetObject*)(set))->sorted_count_)
#define PY_SORTED_SET_SIZE(set)   		(PyList_GET_SIZE(PY_SORTED_SET_GET_LIST(set)))
#define PY_SORTED_SET_BEGIN(set)      		((PyObject**)(PY_SORTED_SET_GET_LIST(set)->ob_item))
#define PY_SORTED_SET_END(set)        		(PY_SORTED_SET_BEGIN(set) + PyList_GET_SIZE((PyObject*)PY_SORTED_SET_GET_LIST(set)))
#define PY_SORTED_SET_SORTED_END(set) 		(PY_SORTED_SET_BEGIN(set) + ((PySortedSetObject*)(set))->sorted_count_)


#ifndef Py_RETURN_NOTIMPLEMENTED
		#define Py_RETURN_NOTIMPLEMENTED \
			return Py_INCREF(Py_NotImplemented), Py_NotImplemented
#endif

PyAPI_FUNC(int) PySortedSet_Check(PyObject* self);
PyAPI_FUNC(int) PySortedSet_CheckExact(PyObject* self);

PyAPI_FUNC(Py_ssize_t) PySortedSet_Size(PyObject* self);
PyAPI_FUNC(Py_ssize_t) PySortedSet_SIZE(PyObject* self);

PyAPI_FUNC(Py_ssize_t) PySortedSet_SortedCount(PyObject* self);
PyAPI_FUNC(Py_ssize_t) PySortedSet_SORTED_COUNT(PyObject* self);



PyAPI_FUNC(int) PySortedSet_IsSorted(PyObject* self);
PyAPI_FUNC(int) PySortedSet_IS_SORTED(PyObject* self);

PyAPI_FUNC(PyObject*) PySortedSet_GetItem(PyObject* self, Py_ssize_t index);
PyAPI_FUNC(PyObject*) PySortedSet_GET_ITEM(PyObject* self, Py_ssize_t index);

PyAPI_FUNC(int) PySortedSet_AddItem(PyObject* self, PyObject* item);
PyAPI_FUNC(int) PySortedSet_ADD_ITEM(PyObject* self, PyObject* item);


PyAPI_FUNC(int) PySortedSet_IsSorted(PyObject* self);
PyAPI_FUNC(int) PySortedSet_IS_SORTED(PyObject* self);

PyAPI_FUNC(int) PySortedSet_Finalize(PyObject* self);
PyAPI_FUNC(int) PySortedSet_FINALIZE(PyObject* self);

PyAPI_FUNC(Py_ssize_t) PySortedSet_IndexOf(PyObject* self, PyObject* item);
PyAPI_FUNC(Py_ssize_t) PySortedSet_INDEX_OF(PyObject* self, PyObject* item);

PyAPI_FUNC(int) PySortedSet_Erase(PyObject* self, PyObject* item);
PyAPI_FUNC(int) PySortedSet_ERASE(PyObject* self, PyObject* item);

PyAPI_FUNC(int) PySortedSet_EraseIndex(PyObject* self, Py_ssize_t index);
PyAPI_FUNC(int) PySortedSet_ERASE_INDEX(PyObject* self, Py_ssize_t index);

PyAPI_FUNC(int) PySortedSet_LexCompare(PyObject* self, PyObject* other);
PyAPI_FUNC(int) PySortedSet_LEX_COMPARE(PyObject* self, PyObject* other);

PyAPI_FUNC(PyObject*) PySortedSet_new(PyTypeObject* tp, PyObject* args, PyObject* kwargs);

PyAPI_FUNC(PyObject*) PySortedSet_Intersection(PyObject* sources);
PyAPI_FUNC(PyObject*) PySortedSet_Difference(PyObject* sources);


PyAPI_FUNC(PyObject*) PySortedSet_FromIterable(PyObject* iterable);
PyAPI_FUNC(PyObject*) PySortedSet_FROM_ITERABLE(PyObject* iterable);

PyAPI_FUNC(PyObject*) PySortedSet_UpdateFromIterable(PyObject* self, PyObject* iterable);


PyTypeObject* _PySortedSet_TypeObject();
#endif /* PYSORTEDSET_H_ */

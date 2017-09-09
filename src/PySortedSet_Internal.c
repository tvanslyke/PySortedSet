#include "PySortedSet_Internal.h"



PyObject* PySortedSet_AsFinalizedSortedSet(PyObject* obj)
{
	PyObject* as_set = NULL;
	if(PySortedSet_Check(obj))
	{
		as_set = obj;
		Py_INCREF(as_set);
	}
	else
	{
		as_set = PySortedSet_FromIterable(obj);
	}
	if(!as_set)
		return NULL;
	if(PySortedSet_FINALIZE(as_set) != 0)
	{
		Py_DECREF(as_set);
		return NULL;
	}
	return as_set;
}

int PySortedSet_NumberToIndex(PyObject* self, PyObject* index, Py_ssize_t* dest)
{
	if(!PyNumber_Check(index))
	{
		PyErr_SetString(PyExc_TypeError, "Non-integer type passed SortedSet method.");
		return -1;
	}
	Py_ssize_t idx = PyNumber_AsSsize_t(index, PyExc_IndexError);
	if(idx == -1 && PyErr_Occurred())
		return -1;
	Py_ssize_t len = PY_SORTED_SET_SIZE(self);
	idx += (idx < 0) * len;
	if(idx < 0 || idx >= len)
	{
		PyErr_SetString(PyExc_IndexError, "Index out-of-bounds in SortedSet method.");
		return -1;
	}
	*dest = idx;
	return 0;
}


void PySortedSet_MOVE_ASSIGN_LIST(PyObject* self, PyObject* list)
{
	Py_INCREF(list);
	PyObject* self_list = PY_SORTED_SET_GET_LIST_AS_OBJ(self);
	PY_SORTED_SET_GET_LIST(self) = (PyListObject*)list;
	Py_DECREF(self_list);
}


void PySortedSet_MOVE_ASSIGN(PyObject* self, PyObject* other)
{
	PyListObject* newlist = PY_SORTED_SET_GET_LIST_AS_OBJ(other);
	Py_ssize_t sorted_count = PY_SORTED_SET_SORTED_COUNT(other);
	PyListObject* self_list = PY_SORTED_SET_GET_LIST_AS_OBJ(self);
	Py_INCREF(newlist);
	PY_SORTED_SET_GET_LIST(self) = newlist;
	PY_SORTED_SET_SORTED_COUNT(self) = sorted_count;
	Py_DECREF(self_list);
	Py_DECREF(other);
}







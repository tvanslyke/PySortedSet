
#include "sortedsetobject.h"



PyObject* PySortedSet_subscript(PyObject* self, PyObject* index)
{
	if(PySortedSet_FINALIZE(self) != 0)
		return NULL;
	
	Py_ssize_t len = PY_SORTED_SET_SIZE(self);
	
	if(PyIndex_Check(index))
		return PySortedSet_HandleIndex(self, index);
	else if(PySlice_Check(index))
		return PySortedSet_HandleSlice(self, index);
	
	PyObject* elem = PyObject_GetItem((PyObject*)PY_SORTED_SET_GET_LIST(self), index);
	if(elem)
		PY_SORTED_SET_SORTED_COUNT(self) = 0;
	return elem;
}


int PySortedSet_assign_subscript(PyObject* self, PyObject* item, PyObject* value)
{
	if(PySortedSet_FINALIZE(self) != 0)
		return -1;
	
	// TODO:  Something more efficient than just changing 'sorted_count_'
	Py_ssize_t len = PY_SORTED_SET_SIZE(self);
	if(PyIndex_Check(item))
	{
		return PySortedSet_HandleIndexAssignment(self, item, value);
	}
	else if(PySlice_Check(item))
	{
		Py_ssize_t lower_bound = 0;
		{ /* scope */
			Py_ssize_t slc[4];
			PySlice_GetIndicesEx((PySliceObject*)item, PY_SORTED_SET_SIZE(self), slc, slc + 1, slc + 2, slc + 4);
			lower_bound = slc[0] < slc[1] ? slc[0] : slc[1];
			lower_bound = lower_bound < 0 ? 0 : lower_bound;
			lower_bound = lower_bound > len ? len : lower_bound;
 		}/* /scope */
		PY_SORTED_SET_SORTED_COUNT(self) = lower_bound;
		return PyObject_SetItem((PyObject*)PY_SORTED_SET_GET_LIST(self), item, value);
	}
	else
	{
		int errcode = PyObject_SetItem((PyObject*)PY_SORTED_SET_GET_LIST(self), item, value);
		if(errcode != 0)
			PY_SORTED_SET_SORTED_COUNT(self) = 0;
		return errcode;
	}
}

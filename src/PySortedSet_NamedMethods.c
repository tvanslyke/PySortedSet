

#include "sortedsetobject.h"


PyObject* PySortedSet_add(PyObject* self, PyObject* arg)
{

	if(PySortedSet_ADD_ITEM(self, arg) < 0)
		return NULL;
	Py_RETURN_NONE;
}



PyObject* PySortedSet_ClearSet(PySortedSetObject *self)
{
	if(PySortedSet_FINALIZE((PyObject*)self) != 0)
	{
		PyErr_BadInternalCall();
		return NULL;
	}
	self->sorted_count_ = 0;
	if(PySequence_DelSlice((PyObject*)PY_SORTED_SET_GET_LIST(self), 0, PY_SORTED_SET_SIZE(self)) < 0)
	{
		PyErr_BadInternalCall();
		return NULL;
	}
	Py_RETURN_NONE;
}
PyObject* PySortedSet_copy(PyObject* self)
{
	// even though we don't need to finalize the set to make a copy, doing this
	// may make the list smaller which provides the opportunity to allocate less memory
	// than if we did not finalize first.

	if(PySortedSet_FINALIZE(self) != 0)
		return NULL;
	PySortedSetObject* cpy = (PySortedSetObject*)PySortedSet_new(self->ob_type, NULL, NULL);
	if(cpy == NULL)
		return NULL;
	Py_ssize_t len = PyList_GET_SIZE(PY_SORTED_SET_GET_LIST(self));
	cpy->sorted_set_ = (PyListObject*)PyList_GetSlice((PyObject*)PY_SORTED_SET_GET_LIST(self), 0, len);
	
	
	if(!(cpy->sorted_set_))
		return NULL;
	cpy->sorted_count_ = PY_SORTED_SET_SORTED_COUNT(self);
	return (PyObject*)cpy;
}

PyObject* PySortedSet_discard(PyObject* self, PyObject* item)
{
	if(PySortedSet_FINALIZE(self) != 0)
		return NULL;
	if(!item)
	{
		PyErr_BadInternalCall();
		return NULL;
	}
	if(PySortedSet_FINALIZE(self) < 0)
	{
		PyErr_BadInternalCall();
		return NULL;
	}

	Py_ssize_t idx = PySortedSet_INDEX_OF(self, item);
	if(idx >= PY_SORTED_SET_SIZE(self) || idx < 0)
		Py_RETURN_FALSE;
	else if(PySortedSet_Equal(PySortedSet_GET_ITEM(self, idx), item))
	{
		if(PySequence_DelItem((PyObject*)PY_SORTED_SET_GET_LIST(self), idx) != 0)
		{
			PyErr_BadInternalCall();
			return NULL;
		}
		PY_SORTED_SET_SORTED_COUNT(self) = PY_SORTED_SET_SIZE(self);
		Py_RETURN_TRUE;
	}
	else
		Py_RETURN_FALSE;
}




PyObject* PySortedSet_intersection(PyObject* self, PyObject* tup)
{
	if(!PySortedSet_Check(self))
	{
		PyErr_SetString(PyExc_TypeError, "SortedSet.intersection() cannot be called on a non-SortedSet object.");
		return NULL;
	}
	Py_ssize_t len = PyTuple_GET_SIZE(tup);
	PyObject* packed_tup = PyTuple_New(len + 1);
	if(!packed_tup)
	{
		return NULL;
	}
	Py_INCREF(self);
	PyTuple_SET_ITEM(packed_tup, 0, self);
	PyObject* item;
	for(size_t i = 0; i < len; ++i)
	{
		item = PyTuple_GET_ITEM(tup, i);

		if(PySortedSet_Check(item))
		{
			if(self == item)
				_PyTuple_Resize(&packed_tup, (len--)); // shouldn't throw error when shrinking (right?)
			else
			{
				Py_INCREF(item);	
				PyTuple_SET_ITEM(packed_tup, i + 1, item);
			}
		}
		else
		{
			PyObject* as_set = PySortedSet_FromIterable(item);
			if((!as_set) || (PySortedSet_FINALIZE(as_set) != 0))
			{
				Py_DECREF(packed_tup);
				Py_XDECREF(as_set);
				return NULL;
			}
			Py_INCREF(as_set);
			PyTuple_SET_ITEM(packed_tup, i + 1, as_set);
			
		}
	}

	return PySortedSet_MultiIntersection(packed_tup);
}
PyObject* PySortedSet_intersection_update(PyObject* self, PyObject* argtup)
{
        PyObject* interset = PySortedSet_intersection(self, argtup);
        if(!interset)
        {
                return NULL;
        }
        PySortedSet_MOVE_ASSIGN(self, interset);
	Py_RETURN_NONE;
}


PyObject* PySortedSet_difference(PyObject* self, PyObject* tup)
{
        if(!PySortedSet_Check(self))
        {
                PyErr_SetString(PyExc_TypeError, "SortedSet.difference() cannot be called on a non-SortedSet object.");
                return NULL;
        }
	PyObject* cpy = PySortedSet_copy(self);
	if(!cpy)
		return NULL;
	PyObject* result = PySortedSet_difference_update(cpy, tup);
	if(!result)
	{
		Py_DECREF(cpy);
		return NULL;
	}
	Py_DECREF(result);
	return cpy;
}

PyObject* PySortedSet_difference_update(PyObject* self, PyObject* argtup)
{
        if(!PySortedSet_Check(self))
        {
                PyErr_SetString(PyExc_TypeError, "SortedSet.difference_update() cannot be called on a non-SortedSet object.");
                return NULL;
        }
	if(!PySortedSet_FINALIZE(self))
		return NULL;
	Py_ssize_t argcount = PyTuple_GET_SIZE(argtup);
	PyObject* as_sets = PyTuple_New(PyTuple_GET_SIZE(argtup));
	if(!as_sets)
		return NULL;
	PyObject* as_set = NULL;
	for(Py_ssize_t i = 0; i < as_sets; ++i)
	{
		as_set = PySortedSet_AsFinalizedSortedSet(PyTuple_GET_ITEM(argtup, i));
		if(!as_set)
		{
			Py_DECREF(as_sets);
			return NULL;
		}
		PyTuple_SET_ITEM(as_sets, i, as_set);
	}
	if(PySortedSet_MultiDifferenceUpdate(argtup) != 0)
	{
		Py_DECREF(as_sets);	
		return NULL;
	}
	Py_DECREF(as_sets);
	Py_RETURN_NONE;
}


PyObject* PySortedSet_update(PyObject* self, PyObject* iter_tuple)
{
	Py_ssize_t len = PyTuple_GET_SIZE(iter_tuple);
	PyObject* result = NULL;
	
	for(Py_ssize_t i = 0; i < len; ++i)
	{
		result = PySortedSet_UpdateFromIterable(self, PyTuple_GET_ITEM(iter_tuple, i));
		if(result)
		{
			Py_DECREF(result);
		}
		else
			return NULL;	
	}
	Py_RETURN_NONE;
}


PyObject* PySortedSet_union(PyObject* self, PyObject* tup)
{
        if(!PySortedSet_Check(self))
        {
                PyErr_SetString(PyExc_TypeError, "SortedSet.difference() cannot be called on a non-SortedSet object.");
                return NULL;
        }
        Py_ssize_t len = PyTuple_GET_SIZE(tup);
        PyObject* packed_tup = PyTuple_New(len + 1);
        if(!packed_tup)
        {
                return NULL;
        }
	Py_INCREF(self);
        PyTuple_SET_ITEM(packed_tup, 0, self);
	PyObject* item = NULL;
        for(size_t i = 0; i < len; ++i)
        {
		item = PyTuple_GET_ITEM(tup, i);
                if(PySortedSet_Check(item))
                {
			if(self == item)
			{
				_PyTuple_Resize(&packed_tup, (len--)); // shouldn't throw error when shrinking (right?)
			}
			else
			{
				Py_INCREF(PyTuple_GET_ITEM(tup, i));
	                        PyTuple_SET_ITEM(packed_tup, i + 1, PyTuple_GET_ITEM(tup, i));
			}
                }
                else
                {
                        PyObject* as_set = PySortedSet_FromIterable(item);
			if((!as_set) || (PySortedSet_FINALIZE(as_set) != 0))
                        {
                                Py_DECREF(packed_tup);
				Py_XDECREF(as_set);
                                return NULL;
                        }
			Py_INCREF(as_set);
                        PyTuple_SET_ITEM(packed_tup, i + 1, as_set);
                }
        }
        PyObject* result = PySortedSet_MultiUnion(packed_tup);
	Py_DECREF(packed_tup);
	return result;
}

PyObject* PySortedSet_isdisjoint(PyObject* self, PyObject* other)
{
	PyObject* otherset = PySortedSet_AsFinalizedSortedSet(other);
	if(!otherset)
		return NULL;
	if(PySortedSet_FINALIZE(self) != 0)
	{
		Py_DECREF(otherset);
		return NULL;	
	}
	if(PySortedSet_HasIntersection(self, other))
		Py_RETURN_FALSE;
	else
		Py_RETURN_TRUE;
}

PyObject* PySortedSet_resort(PyObject* self)
{
	int errcode = PySortedSet_Resort(self);
	if(errcode != 0)
		return NULL;
	Py_RETURN_NONE;
}



PyObject* PySortedSet_pop(PyObject* self, PyObject* args)
{
	if(PySortedSet_FINALIZE(self) != 0)
		return NULL;
	
	Py_ssize_t len = PY_SORTED_SET_SIZE(self);
	if(!len)
	{
		PyErr_SetString(PyExc_IndexError, "Attempt to pop() from empty SortedSet instance.");
		return NULL;
	}
	Py_ssize_t idx = len - 1;
	if(PyArg_ParseTuple(args, "|n", &idx) != 0)
		return NULL;
	idx -= (idx < 0) * len;
	if(idx < 0 || idx >= len)
	{
		PyErr_SetString(PyExc_IndexError, "Index out of bounds in SortedSet.pop().");
		return NULL;
	}
	PyObject* popped = PySortedSet_GET_ITEM(self, idx);
	Py_INCREF(popped);
	if(PySequence_DelItem(PY_SORTED_SET_GET_LIST(self), idx) != 0)
	{
		Py_XDECREF(popped);
		return NULL;
	}
	PY_SORTED_SET_SORTED_COUNT(self) -= 1;
	return popped;
}

PyObject* PySortedSet_issubset(PyObject* self, PyObject* other)
{
	if(PySortedSet_FINALIZE(self) != 0)
		return NULL;
	PyObject* otherset = PySortedSet_AsFinalizedSortedSet(other);
	if(!otherset)
		return NULL;
	if(!PySortedSet_FINALIZE(otherset))
	{
		Py_XDECREF(otherset);
		return NULL;
	}
	int result = PySortedSet_IsSubset(self, otherset);
	Py_DECREF(otherset);
	if(result)
		Py_RETURN_TRUE;
	else
		Py_RETURN_FALSE;
}
PyObject* PySortedSet_issuperset(PyObject* self, PyObject* other)
{
	if(PySortedSet_FINALIZE(self) != 0)
		return NULL;
	PyObject* otherset = PySortedSet_AsFinalizedSortedSet(other);
	if(!otherset)
		return NULL;
	if(!PySortedSet_FINALIZE(otherset))
	{
		Py_XDECREF(otherset);
		return NULL;
	}
	int result = PySortedSet_IsSubset(otherset, self);
	Py_DECREF(otherset);
	if(result)
		Py_RETURN_TRUE;
	else
		Py_RETURN_FALSE;
}

PyObject* PySortedSet_remove_index(PyObject* self, PyObject* index)
{	
	if(PySortedSet_FINALIZE(self) != 0)
		return NULL;
	Py_ssize_t idx = 0;
	if(PySortedSet_NumberToIndex(self, index, &idx) != 0)
		return NULL;
	if(PySortedSet_ERASE_INDEX(self, idx) != 0)
		return NULL;
	Py_RETURN_NONE;
}
PyObject* PySortedSet_remove(PyObject* self, PyObject* other)
{
	PyObject* discard_result = PySortedSet_discard(self, other);
	if(!discard_result)
		return NULL;
	else if(discard_result == Py_True)
		Py_RETURN_NONE;
	PyErr_SetString(PyExc_KeyError, "Requested key not found in call to SortedSet.remove().");
	return NULL;
	
}
PyObject* PySortedSet_symmetric_difference(PyObject* self, PyObject* other)
{
	if(PySortedSet_FINALIZE(self) != 0)
		return NULL;
	other = PySortedSet_AsFinalizedSortedSet(other);
	if(!other)
		return NULL;	
	
	PyObject* resultant_set = PySortedSet_SymmetricDifference(self, other);
	Py_DECREF(other);
	return resultant_set;

}
PyObject* PySortedSet_symmetric_difference_update(PyObject* self, PyObject* other)
{
	PyObject* symdiff = PySortedSet_symmetric_difference(self, other);
	if(!symdiff)
		return NULL;
	PySortedSet_MOVE_ASSIGN(self, symdiff);
	Py_RETURN_NONE;
}







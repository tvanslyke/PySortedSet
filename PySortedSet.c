/*
 * mut_str.cpp
 *
 *  Created on: Jun 29, 2017
 *      Author: tim
 */

/*
 * noddy.c
 *
 *  Created on: Jun 29, 2017
 *      Author: tim
 */




#include "PySortedSet.h"



static PyObject* PySortedSet_OneArg(PyObject* argtup, const char* meth_name)
{
	if((!PyTuple_Check(argtup)) || PyTuple_GET_SIZE(argtup) != 1)
	{
		char msg[1024];
		sprintf(msg, "%s accepts exactly one argument.", meth_name);
		PyErr_SetString(PyExc_TypeError, msg);
		return NULL;
	}
	return PyTuple_GET_ITEM(argtup, 0);
}

static int PySortedSet_clear(PyObject*);
static void PySortedSet_dealloc(PySortedSetObject* self)
{
	PyObject_GC_UnTrack(self);
	PySortedSet_clear(self);
	Py_TYPE(self)->tp_free((PyObject*)self);
}



PyObject* PySortedSet_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
	PySortedSetObject* self = (PySortedSetObject*)type->tp_alloc(type, 0);
	if(!self)
	{
		PyErr_SetString(PyExc_SystemError, "PySortedSet.__new__() returned NULL. (this is a bug)");
		return NULL;
	}
	self->sorted_set_ = (PyListObject*)PyList_New(0);
	if(!(self->sorted_set_))
	{
		PyErr_SetString(PyExc_SystemError, "PySortedSet.__new__() returned NULL. (this is a bug)");
		return NULL;
	}
	self->sorted_count_ = 0;
	return (PyObject*)self;
}

PyObject* PySortedSet_repr(PySortedSetObject * self);
static int PySortedSet_init(PyObject* self, PyObject *args, PyObject *kwds)
{
	Py_ssize_t len = PyTuple_Size(args);

	if(len > 1)
	{
		PyErr_SetString(PyExc_TypeError, "SortedSet() takes at most 1 argument.");
		Py_XDECREF(PY_SORTED_SET_GET_LIST(self));
		Py_XDECREF(self);
		return -1;
	}
	else if(kwds && PyDict_Size(kwds) != 0)
	{
		PyErr_SetString(PyExc_TypeError, "SortedSet() takes no keyword arguments.");
		Py_XDECREF(PY_SORTED_SET_GET_LIST(self));
		Py_XDECREF(self);
		return -1;
	}
	else if(len == 1)
	{
		PyObject* obj = PyTuple_GET_ITEM(args, 0);
		if(PySequence_Check(obj))
		{
			((PySortedSetObject*)self)->sorted_set_ = (PyListObject*)PySequence_List((PyObject*)obj);
		}
		else if(PyIter_Check(obj))
		{
			PyObject* item = PyIter_Next(obj);
			while(item)
			{
				PySortedSet_ADD_ITEM(self, item);
				Py_XDECREF(item);
			}
			Py_XDECREF(obj);
		}
		else
		{
			PyErr_SetString(PyExc_TypeError, "SortedSet() may only be constructed from an iterator or sequence.");
			Py_XDECREF(PY_SORTED_SET_GET_LIST(self));
			Py_XDECREF(self);
			return -1;
		}
	}

	return 0;
}


PyObject* PySortedSet_repr(PySortedSetObject * self)
{
	int i = Py_ReprEnter((PyObject*)self);
	PyObject* reprstr;
	if(i != 0)
	{
		reprstr = i > 0 ? PyString_FromString("SortedSet([...])") : NULL;
	}
	else
	{
		
		if(PySortedSet_FINALIZE((PyObject*)self) == -1)
		{
			Py_ReprLeave((PyObject*)self);
			return NULL;
		}


		PyObject* list_repr = PyObject_Repr((PyObject*)(self->sorted_set_));
		assert(list_repr);
		reprstr = PyString_FromFormat("SortedSet(%s)", PyString_AS_STRING(list_repr));
		
		Py_XDECREF(list_repr);
	}

	Py_ReprLeave((PyObject*)self);
	return reprstr;
}

static
int PySortedSet_contains(PyObject* set, PyObject* item)
{
	if(PySortedSet_FINALIZE(set) == -1)
	{
		PyErr_SetString(PyExc_RuntimeError, "Internal error encountered when finalizing SortedSet instance (this is a bug).");
		return 0;
	}
	Py_ssize_t len = PySortedSet_SIZE(set);
	if(!len)
		return 0;
	Py_ssize_t idx = PySortedSet_INDEX_OF(set, item);
	if(idx >= len)
		return 0;
	return PySortedSet_Equal(PySortedSet_GET_ITEM(set, idx), item);
}




static PyObject * PySortedSet_rich_compare(PyObject* left, PyObject* right, int op)
{
	if(!PySortedSet_Check(left) || !PySortedSet_Check(right))
		Py_RETURN_NOTIMPLEMENTED;
	if(left == right)
	{
		switch(op)
		{
	    case Py_EQ:
	    	Py_RETURN_TRUE;
	        break;
	    case Py_NE:
	    	Py_RETURN_FALSE;
	        break;
	    case Py_LE:
	    	Py_RETURN_TRUE;
	        break;
	    case Py_GE:
	    	Py_RETURN_TRUE;
	        break;
	    case Py_LT:
	    	Py_RETURN_FALSE;
	        break;
	    case Py_GT:
	    	Py_RETURN_FALSE;
	    	break;
	    default:
	    	Py_RETURN_NOTIMPLEMENTED;
	    	break;
		}
	}
	if((PySortedSet_FINALIZE(left) != 0) || (PySortedSet_FINALIZE(right) != 0))
	{
		PyErr_SetString(PyExc_RuntimeError, "Problem encountered while uniqifying sets (this is a bug).");
		Py_RETURN_NOTIMPLEMENTED;
	}
	return PyObject_RichCompare((PyObject*)PY_SORTED_SET_GET_LIST(left), (PyObject*)PY_SORTED_SET_GET_LIST(right), op);
}


static PyObject * PySortedSet_iter(PyObject *self)
{
	if(PySortedSet_FINALIZE(self) == -1)
	{
		PyErr_SetString(PyExc_TypeError, "Error while creating iterator for SortedSet.");
		return NULL;
	}
	return PyObject_GetIter((PyObject*)(((PySortedSetObject*)self)->sorted_set_));
}


static PyObject* PySortedSet_add(PyObject* self, PyObject* tup)
{

	PyObject* item = PySortedSet_OneArg(tup, "add");
	if((!item) || (PySortedSet_ADD_ITEM(self, item) < 0))
	{
		PyErr_BadInternalCall();
		return NULL;
	}
	
	Py_RETURN_NONE;
}



static PyObject* PySortedSet_ClearSet(PySortedSetObject *self)
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

static PyObject* PySortedSet_discard(PyObject* self, PyObject* tup)
{
	if(PySortedSet_FINALIZE(self) != 0)
	{
		PyErr_BadInternalCall();
		return NULL;
	}
	PyObject* item = PySortedSet_OneArg(tup, "discard");
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




static PyObject* PySortedSet_intersection(PyObject* self, PyObject* tup)
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
	int self_in_args = 0;
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

	return PySortedSet_Intersection(packed_tup);
}
static PyObject* PySortedSet_intersection_update(PyObject* self, PyObject* argtup)
{
        PyObject* interset = PySortedSet_intersection(self, argtup);
        if(!interset)
        {
                return NULL;
        }
        Py_XDECREF((PyObject*)PY_SORTED_SET_GET_LIST(self));
        PY_SORTED_SET_GET_LIST(self) = PY_SORTED_SET_GET_LIST(interset);
        PY_SORTED_SET_SORTED_COUNT(self) = PY_SORTED_SET_SORTED_COUNT(interset);
        Py_XINCREF((PyObject*)PY_SORTED_SET_GET_LIST(self));
        Py_XDECREF(interset);
        Py_RETURN_NONE;
}


static PyObject* PySortedSet_difference(PyObject* self, PyObject* tup)
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
	int self_in_args = 0;
	PyObject* item = NULL;
        for(size_t i = 0; i < len; ++i)
        {
		item = PyTuple_GET_ITEM(tup, i);
                if(PySortedSet_Check(item))
                {
			if(self == item)
			{
				self_in_args = 1;
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
	if(self_in_args)
	{
		return PySortedSet_new(_PySortedSet_TypeObject(), NULL, NULL);
	}
        PyObject* result = PySortedSet_Difference(packed_tup);
	Py_DECREF(packed_tup);
	return result;
}

static PyObject* PySortedSet_difference_update(PyObject* self, PyObject* argtup)
{
	PyObject* diffset = PySortedSet_difference(self, argtup);
	if(!diffset)
	{
		return NULL;
	}
	Py_XDECREF((PyObject*)PY_SORTED_SET_GET_LIST(self));
	PY_SORTED_SET_GET_LIST(self) = PY_SORTED_SET_GET_LIST(diffset);
	PY_SORTED_SET_SORTED_COUNT(self) = PY_SORTED_SET_SORTED_COUNT(diffset);
	Py_XINCREF((PyObject*)PY_SORTED_SET_GET_LIST(self));
	Py_XDECREF(diffset);
	Py_RETURN_NONE;
}


static PyObject* PySortedSet_update(PyObject* self, PyObject* iter_tuple)
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


static PyObject* PySortedSet_union(PyObject* self, PyObject* tup)
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
        PyObject* result = PySortedSet_Union(packed_tup);
	Py_DECREF(packed_tup);
	return result;
}



PyObject* PySortedSet_isdisjoint(PyObject* self, PyObject* other)
{
	if(PySortedSet_FINALIZE(self) != 0 || PySortedSet_FINALIZE(other) != 0)
	{
		
	}
}




PyObject * PySortedSet_SqGetItem(PyObject * self, Py_ssize_t index)
{
	if(PySortedSet_FINALIZE(self) != 0)
	{
		return NULL;
	}
	
	return PySequence_GetItem((PyObject*)PY_SORTED_SET_GET_LIST(self), index);
}

PyObject * PySortedSet_SqGetSlice(PyObject * self, Py_ssize_t i1, Py_ssize_t i2, Py_ssize_t index)
{
	if(PySortedSet_FINALIZE(self) != 0)
	{
		PyErr_BadInternalCall();
		return NULL;
	}
	return PySequence_GetSlice((PyObject*)PY_SORTED_SET_GET_LIST(self), i1, i2);
}


PyObject* PySortedSet_subscript(PyObject* self, PyObject* index)
{
	if(PySortedSet_FINALIZE(self) != 0)
	{
		return NULL;
	}
	Py_ssize_t len = PY_SORTED_SET_SIZE(self);
	if(PyIndex_Check(index))
	{
		Py_ssize_t idx = PyNumber_AsSsize_t(index, PyExc_IndexError);
	        if (idx == -1 && PyErr_Occurred())
			return NULL;
		Py_ssize_t len = PY_SORTED_SET_SIZE(self);
		if (idx < 0)
			idx += len;
		if(idx >= len)
		{
			PyErr_SetString(PyExc_IndexError, "Index out of bounds with SortedSet instance.");
			return NULL;
		}
		else
		{
			PY_SORTED_SET_SORTED_COUNT(self) = idx;
			PyObject* elem = PyList_GetItem(PY_SORTED_SET_GET_LIST(self), idx);
			Py_XINCREF(elem);
			return elem;
		}
	}
	else if(PySlice_Check(index))
	{
		
		Py_ssize_t lower_bound = 0;
		{ /* scope */
			Py_ssize_t slc[4];
			Py_ssize_t len = PY_SORTED_SET_SIZE(self);
			PySlice_GetIndicesEx((PySliceObject*)index, len, slc, slc + 1, slc + 2, slc + 4);
			lower_bound = slc[0] < slc[1] ? slc[0] : slc[1];
			lower_bound = lower_bound < 0 ? 0 : lower_bound;
			lower_bound = lower_bound > len ? len : lower_bound;
 		}/* /scope */
		PyObject* elem = PyObject_GetItem((PyObject*)PY_SORTED_SET_GET_LIST(self), index);
		if(elem)
		{
			PY_SORTED_SET_SORTED_COUNT(self) = lower_bound;
			Py_INCREF(elem);
		}
		return elem;
	}
	PyObject* elem = PyObject_GetItem((PyObject*)PY_SORTED_SET_GET_LIST(self), index);
	if(elem)
		PY_SORTED_SET_SORTED_COUNT(self) = 0;
	return elem;
}


static int PySortedSet_assign_subscript(PyObject* self, PyObject* item, PyObject* value)
{
	if(PySortedSet_FINALIZE(self) != 0)
	{
		return -1;
	}
	// TODO:  Something more efficient than just changing 'sorted_count_'
	Py_ssize_t len = PY_SORTED_SET_SIZE(self);
	if(PyIndex_Check(item))
	{
		PyObject* list = PY_SORTED_SET_GET_LIST(self);
		Py_ssize_t idx = PyNumber_AsSsize_t(item, PyExc_IndexError);
		if(idx == -1 && PyErr_Occurred())
			return -1;
		idx += (idx < 0) * (len);
		if(idx >= len || idx < 0)
		{
			PyErr_SetString(PyExc_IndexError, "Index out of bounds with SortedSet instance");
			return -1;	
		}	
		PY_SORTED_SET_SORTED_COUNT(self) = idx;
		Py_INCREF(value);
		return PyList_SetItem(list, idx, value);
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



static PyObject* PySortedSet_resort(PyObject* self)
{
	int errcode = PySortedSet_Resort(self);
	if(errcode != 0)
		return NULL;
	Py_RETURN_NONE;
}


static int PySortedSet_traverse(PyObject* self, visitproc visit, void* arg)
{
	Py_VISIT(PY_SORTED_SET_GET_LIST(self));
	return 0;
}



static int PySortedSet_clear(PyObject* self)
{
	Py_CLEAR(PY_SORTED_SET_GET_LIST(self));
	return 0;
}

static PyMethodDef PySortedSet_methods[] = {
   // {"name", (PyCFunction)Noddy_name, METH_NOARGS,
   //  "Return the name, combining the first and last name"
   //},
	{"add",   		(PyCFunction)(PySortedSet_add),   		METH_VARARGS, ""},
	{"clear", 		(PyCFunction)(PySortedSet_ClearSet),		METH_NOARGS,  ""},
	{"copy",  		(PyCFunction)(PySortedSet_copy),   		METH_NOARGS,  ""},
	{"discard", 		(PyCFunction)(PySortedSet_discard), 		METH_VARARGS, ""},
	{"difference", 		(PyCFunction)(PySortedSet_difference), 		METH_VARARGS, ""},
	{"difference_update", 	(PyCFunction)(PySortedSet_difference_update), 	METH_VARARGS, ""},
	{"intersection", 	(PyCFunction)(PySortedSet_intersection),	METH_VARARGS, ""},
	{"intersection_update", (PyCFunction)(PySortedSet_intersection_update), METH_VARARGS, ""},
	{"resort", 		(PyCFunction)(PySortedSet_resort),		METH_NOARGS,  ""},
	{"union",		(PyCFunction)(PySortedSet_union), 		METH_VARARGS, ""},
	{"update", 		(PyCFunction)(PySortedSet_update),		METH_VARARGS, ""},
	{NULL}  /* Sentinel */
};



static PyMethodDef PySortedSet_module_methods[] = {{NULL}};

static PyNumberMethods py_sorted_set_as_number = 
{
     (binaryfunc)(PySortedSet_arith_add),//     binaryfunc nb_add;
     0,//     binaryfunc nb_subtract;
     0,//     binaryfunc nb_multiply;
     0,//     binaryfunc nb_divide;
     0,//     binaryfunc nb_remainder;
     0,//     binaryfunc nb_divmod;
     0,//     ternaryfunc nb_power;
     0,//     unaryfunc nb_negative;
     0,//     unaryfunc nb_positive;
     0,//     unaryfunc nb_absolute;
     0,//     inquiry nb_nonzero;       /* Used by PyObject_IsTrue */
     0,//     unaryfunc nb_invert;
     0,//     binaryfunc nb_lshift;
     0,//     binaryfunc nb_rshift;
     0,//     binaryfunc nb_and;
     0,//     binaryfunc nb_xor;
     0,//     binaryfunc nb_or;
     0,//     coercion nb_coerce;       /* Used by the coerce() function */
     0,//     unaryfunc nb_int;
     0,//     unaryfunc nb_long;
     0,//     unaryfunc nb_float;
     0,//     unaryfunc nb_oct;
     0,//     unaryfunc nb_hex;

     /* Added in release 2.0 */
     0,//     binaryfunc nb_inplace_add;
     0,//     binaryfunc nb_inplace_subtract;
     0,//     binaryfunc nb_inplace_multiply;
     0,//     binaryfunc nb_inplace_divide;
     0,//     binaryfunc nb_inplace_remainder;
     0,//     ternaryfunc nb_inplace_power;
     0,//     binaryfunc nb_inplace_lshift;
     0,//     binaryfunc nb_inplace_rshift;
     0,//     binaryfunc nb_inplace_and;
     0,//     binaryfunc nb_inplace_xor;
     0,//     binaryfunc nb_inplace_or;

     /* Added in release 2.2 */
     0,//     binaryfunc nb_floor_divide;
     0,//     binaryfunc nb_true_divide;
     0,//     binaryfunc nb_inplace_floor_divide;
     0,//     binaryfunc nb_inplace_true_divide;

     /* Added in release 2.5 */
     0,//     unaryfunc nb_index;
};



static PySequenceMethods sorted_set_as_sequence =
{
	PySortedSet_SIZE,                   		/* sq_length */
    0,                                  		/* sq_concat */
    0,                                  		/* sq_repeat */
    PySortedSet_SqGetItem,              		/* sq_item */
	(ssizessizeargfunc)PySortedSet_SqGetSlice,  /* sq_slice */
    0,                                  		/* sq_ass_item */
    0,                                  		/* sq_ass_slice */
    (objobjproc)PySortedSet_contains,
	/* sq_contains */
};


static PyMappingMethods sorted_set_as_mapping =
{
	    (lenfunc)PySortedSet_SIZE,
	    (binaryfunc)PySortedSet_subscript,
	    (objobjargproc)PySortedSet_assign_subscript,
};





static PyMemberDef sorted_set_members[] =
{
	    {NULL}
};

static PyTypeObject PySortedSet_Type = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "SortedSet",  					/* tp_name */
    sizeof(PySortedSetObject),          /* tp_basicsize */
    0,                         			/* tp_itemsize */
    (destructor)PySortedSet_dealloc,	/* tp_dealloc */
    0, 								/* tp_print */
    0,                         		/* tp_getattr */
    0,                         		/* tp_setattr */
	0,    				/* tp_compare */
	(reprfunc)PySortedSet_repr,     /* tp_repr */
        &py_sorted_set_as_number,       /* tp_as_number */
	&sorted_set_as_sequence,    	/* tp_as_sequence */
	&sorted_set_as_mapping,     		/* tp_as_mapping */
	PyObject_HashNotImplemented,	/* tp_hash */
    0,                        		/* tp_call */
    0, 								/* tp_str */
    0,                        	 	/* tp_getattro */
    0,                         		/* tp_setattro */
    0,                         		/* tp_as_buffer */
    Py_TPFLAGS_DEFAULT |
        Py_TPFLAGS_BASETYPE |
	Py_TPFLAGS_HAVE_SEQUENCE_IN |
	Py_TPFLAGS_HAVE_ITER |
	Py_TPFLAGS_HAVE_RICHCOMPARE | 
	Py_TPFLAGS_HAVE_GC,   		/* tp_flags */
    "SortedSet",          		 /* tp_doc */
    (traverseproc)PySortedSet_traverse, /* tp_traverse */
    (inquiry)PySortedSet_clear,     /* tp_clear */
	PySortedSet_rich_compare,/* PySortedSet_rich_compare */      /* tp_richcompare */
    0,                         		/* tp_weaklistoffset */
	PySortedSet_iter,                 /*  tp_iter */
    0,                       		  /* tp_iternext */
	PySortedSet_methods,       		  /* tp_methods */
	sorted_set_members,       		  /* tp_members */
    0,                        		 /* aaaa */
    0,                         /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    (initproc)PySortedSet_init,  /* tp_init */
    0,                         /* tp_alloc */
    PySortedSet_new,             /* tp_new */
};

PyTypeObject* _PySortedSet_TypeObject()
{
	return &PySortedSet_Type;
}

PyAPI_FUNC(int) PySortedSet_Check(PyObject* self)
{
	return PyObject_IsInstance(self, (PyObject*)&PySortedSet_Type);
}
PyAPI_FUNC(int) PySortedSet_CheckExact(PyObject* self)
{
	return Py_TYPE(self) ==  &PySortedSet_Type;
}

#ifndef PyMODINIT_FUNC	/* declarations for DLL import/export */
#define PyMODINIT_FUNC void
#endif
PyMODINIT_FUNC
initSortedSet(void)
{
    PyObject* m;

    if (PyType_Ready(&PySortedSet_Type) < 0)
        return;

    m = Py_InitModule3("SortedSet", PySortedSet_module_methods,
                       "Provides a SortedSet object.");

    if (m == NULL)
    {
    	PyErr_SetString(PyExc_SystemError, "Unable to load module SortedSet.");
    	return;
    }

    Py_INCREF(&PySortedSet_Type);
    PyModule_AddObject(m, "SortedSet", (PyObject *)&PySortedSet_Type);
}



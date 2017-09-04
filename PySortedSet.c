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


static void PySortedSet_dealloc(PySortedSetObject* self)
{
	Py_XDECREF(self->sorted_set_);
	Py_TYPE(self)->tp_free((PyObject*)self);
}



static PyObject* PySortedSet_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
	PySortedSetObject* self = (PySortedSetObject*)type->tp_alloc(type, 0);
	if(!self)
	{
		PyErr_SetString(PyExc_SystemError, "PySortedSet.__new__() returned NULL. (this is a bug)");
		PyErr_BadInternalCall();
		return NULL;
	}
	self->sorted_set_ = (PyListObject*)PyList_New(0);
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
			PyListObject* list = PY_SORTED_SET_GET_LIST(self);
			PyObject* item = PyIter_Next(obj);
			while(item)
			{
				PyList_Append((PyObject*)list, item);
				Py_XDECREF(item);
			}
			Py_XDECREF(obj);
			PySortedSet_FINALIZE(self);
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
		fflush(stderr);
		reprstr = i > 0 ? PyString_FromString("SortedSet([...])") : NULL;
	}
	else
	{
		if(!self)
		{
			Py_ReprLeave((PyObject*)self);
			PyErr_BadInternalCall();
			return NULL;
		}
		if(PySortedSet_FINALIZE((PyObject*)self) == -1)
		{
			Py_ReprLeave((PyObject*)self);
			PyErr_BadInternalCall();
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

static int PySortedSet_traverse(PySortedSetObject *self, visitproc visit, void *arg)
{
	if(PySortedSet_Finalize((PyObject*)self) == -1)
	{
		PyErr_SetString(PyExc_RuntimeError, "Internal error encountered when finalizing SortedSet instance (this is a bug).");
		return -1;
	}
	return PyList_Type.tp_traverse((PyObject*)(self->sorted_set_), visit, arg);
}
static int PySortedSet_clear(PySortedSetObject *self)
{
	self->sorted_count_ = 0;
	return (&PyList_Type)->tp_clear((PyObject*)(self->sorted_set_));
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
	    	return Py_True;
	        break;
	    case Py_NE:
	    	return Py_False;
	        break;
	    case Py_LE:
	    	return Py_True;
	        break;
	    case Py_GE:
	    	return Py_True;
	        break;
	    case Py_LT:
	    	return Py_False;
	        break;
	    case Py_GT:
	    	return Py_False;
	    	break;
		}
	}
	if((PySortedSet_FINALIZE(left) == -1) || (PySortedSet_FINALIZE(right) == -1))
	{
		PyErr_SetString(PyExc_RuntimeError, "Problem encountered while uniqifying sets (this is a bug).");
		Py_RETURN_NOTIMPLEMENTED;
	}
	int result = 0;
    switch (op)
    {
    case Py_EQ:
    	result = (!PySortedSet_LEX_COMPARE(left, right)) && (!PySortedSet_LEX_COMPARE(right, left));
        break;
    case Py_NE:
    	result = (PySortedSet_LEX_COMPARE(left, right)) || (PySortedSet_LEX_COMPARE(right, left));
        break;
    case Py_LE:
    	result = !PySortedSet_LEX_COMPARE(right, left);
        break;
    case Py_GE:
    	result = !PySortedSet_LEX_COMPARE(left, right);
        break;
    case Py_LT:
    	result = PySortedSet_LEX_COMPARE(left, right);
        break;
    case Py_GT:
    	result = PySortedSet_LEX_COMPARE(right, left);
    	break;
    default:
    	Py_RETURN_NOTIMPLEMENTED;
    	break;
    }
    if(result != -1)
    	return PyBool_FromLong(result);
    else
    	Py_RETURN_NOTIMPLEMENTED;
}


static PyObject * PySortedSet_iter(PyObject *self)
{
	if(PySortedSet_FINALIZE(self) == -1)
	{
		PyErr_SetString(PyExc_TypeError, "Error while creatomg iterator for SortedSet.");
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
	return Py_None;
}
static PyObject* PySortedSet_ClearSet(PySortedSetObject *self)
{
	self->sorted_count_ = 0;
	if(PySequence_DelSlice((PyObject*)PY_SORTED_SET_GET_LIST(self), 0, PY_SORTED_SET_SIZE(self)) < 0)
	{
		PyErr_BadInternalCall();
		return NULL;
	}
	return Py_None;
}
static PyObject* PySortedSet_copy(PyObject* self)
{
	PySortedSetObject* cpy = (PySortedSetObject*)PySortedSet_new(self->ob_type, NULL, NULL);
	if(cpy == NULL)
	{
		PyErr_BadInternalCall();
		return NULL;
	}
	cpy->sorted_set_ = (PyListObject*)PySequence_List((PyObject*)PY_SORTED_SET_GET_LIST(self));
	if(!(cpy->sorted_set_))
	{
		PyErr_BadInternalCall();
		Py_XDECREF(cpy);
		return NULL;
	}
	cpy->sorted_count_ = ((PySortedSetObject*)self)->sorted_count_;
	return (PyObject*)cpy;
}

static PyObject* PySortedSet_discard(PyObject* self, PyObject* tup)
{
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
	if(idx >= PY_SORTED_SET_SIZE(self))
		return Py_False;
	else if(PySortedSet_Equal(PySortedSet_GET_ITEM(self, idx), item))
	{
		PySortedSet_ERASE_INDEX(self, idx);
		return Py_True;
	}
	else
		return Py_False;
}

static PyObject* PySortedSet_difference(PyObject* self, PyObject* tup)
{
	Py_ssize_t nargs = PyTuple_GET_SIZE(tup);
	if(nargs < 1)
	{
		return PySortedSet_copy(self);
	}
	PyObject * diffset = PySortedSet_new(_PySortedSet_TypeObject(), NULL, NULL);
	PySortedSet_FINALIZE(self);
	Py_ssize_t len = PY_SORTED_SET_SIZE(self);
	if(len < 1)
		return diffset;
	for(Py_ssize_t j = 0; j < nargs; ++j)
	{
		if(!PySortedSet_Check(PyTuple_GET_ITEM(tup, j)))
		{
			PyErr_SetString(PyExc_TypeError, "SortedSet.difference() accepts only SortedSet instances.");
			return NULL;
		}
	}
	PyObject** elems = PY_SORTED_SET_GET_LIST(self)->ob_item;

	// TODO:  This more efficiently
	Py_ssize_t j = 0;
	for(Py_ssize_t i = 0; i < len; ++i)
	{
		for(j = 0; j < nargs; ++j)
		{
			if(PySortedSet_contains(PyTuple_GET_ITEM(tup, j), elems[i]))
			{
				j = -1;
				break;
			}
		}
		if(j != -1 && (PySortedSet_ADD_ITEM(diffset, elems[i]) == -1))
		{
			PyErr_BadInternalCall();
			Py_XDECREF(diffset);
			return NULL;
		}
	}
	return diffset;

}

PyObject * PySortedSet_SqGetItem(PyObject * self, Py_ssize_t index)
{
	if(PySortedSet_FINALIZE(self) != 0)
	{
		PyErr_BadInternalCall();
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

int PySortedSet_SqDelItem(PyObject * self, Py_ssize_t i1, Py_ssize_t i2, Py_ssize_t index)
{
	if(PySortedSet_FINALIZE(self) != 0)
		return -1;
	int errcode = PySequence_DelItem((PyObject*)PY_SORTED_SET_GET_LIST(self), index);
	if(errcode < 0)
	{
		return errcode;
	}
	else
	{
		PY_SORTED_SET_SORTED_COUNT(self) = PyList_GET_SIZE(PY_SORTED_SET_GET_LIST(self));
		return 0;
	}
}
int PySortedSet_SqDelSlice(PyObject * self, Py_ssize_t i1, Py_ssize_t i2, Py_ssize_t index)
{
	if(PySortedSet_FINALIZE(self) != 0)
		return -1;
	int errcode = PySequence_DelSlice((PyObject*)PY_SORTED_SET_GET_LIST(self), i1, i2);
	if(errcode < 0)
	{
		return errcode;
	}
	else
	{
		PY_SORTED_SET_SORTED_COUNT(self) = PyList_GET_SIZE(PY_SORTED_SET_GET_LIST(self));
		return 0;
	}
}




static PyMethodDef PySortedSet_methods[] = {
   // {"name", (PyCFunction)Noddy_name, METH_NOARGS,
   //  "Return the name, combining the first and last name"
   //},
	{"add",   		(PyCFunction)(PySortedSet_add),   			METH_VARARGS, ""},
	{"clear", 		(PyCFunction)(PySortedSet_ClearSet),		METH_NOARGS,  ""},
	{"copy",  		(PyCFunction)(PySortedSet_copy),   			METH_NOARGS,  ""},
	{"discard", 	(PyCFunction)(PySortedSet_discard), 		METH_VARARGS, ""},
	{"difference", 	(PyCFunction)(PySortedSet_difference), 		METH_VARARGS, ""},
	{NULL}  /* Sentinel */
};

static PyMethodDef PySortedSet_module_methods[] = {{NULL}};

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

static PyTypeObject PySortedSet_Type = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "SortedSet",  					/* tp_name */
    sizeof(PySortedSetObject),          /* tp_basicsize */
    0,                         			/* tp_itemsize */
    (destructor)PySortedSet_dealloc,	/* tp_dealloc */
    0, 								/* tp_print */
    0,                         		/* tp_getattr */
    0,                         		/* tp_setattr */
	0,    							/* tp_compare */
	(reprfunc)PySortedSet_repr,     /* tp_repr */
    0,                         		/* tp_as_number */
	&sorted_set_as_sequence,    	/* tp_as_sequence */
    0,                        		/* tp_as_mapping */
	PyObject_HashNotImplemented,	/* tp_hash */
    0,                        		/* tp_call */
	0, 								/* tp_str */
    0,                        	 	/* tp_getattro */
    0,                         		/* tp_setattro */
    0,                         		/* tp_as_buffer */
    Py_TPFLAGS_DEFAULT |
        Py_TPFLAGS_BASETYPE,   		/* tp_flags */
    "SortedSet",          		 /* tp_doc */
    (traverseproc)PySortedSet_traverse, /* tp_traverse */
	(inquiry)PySortedSet_clear,     /* tp_clear */
	0,/* PySortedSet_rich_compare */      /* tp_richcompare */
    0,                         		/* tp_weaklistoffset */
	PySortedSet_iter,                 /*  tp_iter */
    0,                       		  /* tp_iternext */
	PySortedSet_methods,       		  /* tp_methods */
	0,       		  /* tp_members */
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



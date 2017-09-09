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





#include "sortedsetobject.h"



static void PySortedSet_dealloc(PyObject* self)
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



static int PySortedSet_traverse(PyObject* self, visitproc visit, void* arg)
{
	Py_VISIT(PY_SORTED_SET_GET_LIST(self));
	return 0;
}



int PySortedSet_clear(PyObject* self)
{
	Py_CLEAR(PY_SORTED_SET_GET_LIST(self));
	return 0;
}

PyObject* PySortedSet_issubset(PyObject* self, PyObject* other);
PyObject* PySortedSet_issuperset(PyObject* self, PyObject* other);
PyObject* PySortedSet_remove(PyObject* self, PyObject* other);
PyObject* PySortedSet_remove_index(PyObject* self, PyObject* index);
PyObject* PySortedSet_symmetric_difference(PyObject* self, PyObject* other);
PyObject* PySortedSet_symmetric_difference_update(PyObject* self, PyObject* other);
static PyMethodDef PySortedSet_methods[] = {
	
	{"add",   			(PyCFunction)(PySortedSet_add),   			METH_O,       ""},
	{"clear", 			(PyCFunction)(PySortedSet_ClearSet),			METH_NOARGS,  ""},
	{"copy",  			(PyCFunction)(PySortedSet_copy),   			METH_NOARGS,  ""},
	{"discard", 			(PyCFunction)(PySortedSet_discard), 			METH_O,       ""},
	{"difference", 			(PyCFunction)(PySortedSet_difference), 			METH_VARARGS, ""},
	{"difference_update", 		(PyCFunction)(PySortedSet_difference_update), 		METH_VARARGS, ""},
	{"intersection", 		(PyCFunction)(PySortedSet_intersection),		METH_VARARGS, ""},
	{"intersection_update", 	(PyCFunction)(PySortedSet_intersection_update), 	METH_VARARGS, ""},
	
	{"pop", 			(PyCFunction)(PySortedSet_pop), 			METH_VARARGS, ""},
	{"isdisjoint", 			(PyCFunction)(PySortedSet_isdisjoint), 			METH_O, ""},
	{"issubset", 			(PyCFunction)(PySortedSet_issubset), 			METH_O, ""},
	{"issuperset", 			(PyCFunction)(PySortedSet_issuperset), 			METH_O, ""},
	{"remove", 			(PyCFunction)(PySortedSet_remove), 			METH_O, ""},
	{"remove_index", 		(PyCFunction)(PySortedSet_remove_index), 		METH_O, ""},
	{"symmetric_difference", 	(PyCFunction)(PySortedSet_symmetric_difference), 	METH_O, ""},
	{"symmetric_difference_update", (PyCFunction)(PySortedSet_symmetric_difference_update), METH_O, ""},
	
	{"resort", 			(PyCFunction)(PySortedSet_resort),			METH_NOARGS,  ""},
	{"union",			(PyCFunction)(PySortedSet_union), 			METH_VARARGS, ""},
	{"update", 			(PyCFunction)(PySortedSet_update),			METH_VARARGS, ""},
	{NULL}  /* Sentinel */
};



static PyMethodDef PySortedSet_module_methods[] = {{NULL}};

static PyNumberMethods py_sorted_set_as_number = 
{
     (binaryfunc)(PySortedSet_arith_add),//     binaryfunc nb_add;
     (binaryfunc)(PySortedSet_arith_sub) ,//     binaryfunc nb_subtract;
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
     (binaryfunc)(PySortedSet_arith_sub), //     binaryfunc nb_and;
     (binaryfunc)(PySortedSet_arith_xor),//     binaryfunc nb_xor;
     (binaryfunc)(PySortedSet_arith_or),//     binaryfunc nb_or;
     0,//     coercion nb_coerce;       /* Used by the coerce() function */
     0,//     unaryfunc nb_int;
     0,//     unaryfunc nb_long;
     0,//     unaryfunc nb_float;
     0,//     unaryfunc nb_oct;
     0,//     unaryfunc nb_hex;

     /* Added in release 2.0 */
     PySortedSet_arith_add_eq,//     binaryfunc nb_inplace_add;
     PySortedSet_arith_sub_eq,//     binaryfunc nb_inplace_subtract;
     0,//     binaryfunc nb_inplace_multiply;
     0,//     binaryfunc nb_inplace_divide;
     0,//     binaryfunc nb_inplace_remainder;
     0,//     ternaryfunc nb_inplace_power;
     0,//     binaryfunc nb_inplace_lshift;
     0,//     binaryfunc nb_inplace_rshift;
     (binaryfunc)(PySortedSet_arith_and),//     binaryfunc nb_inplace_and;
     (binaryfunc)(PySortedSet_arith_xor),//     binaryfunc nb_inplace_xor;
     (binaryfunc)(PySortedSet_arith_or),//     binaryfunc nb_inplace_or;

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
    0,              					/* sq_item */
    0,  						/* sq_slice */
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
	PySortedSet_rich_compare,  /* tp_richcompare */
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



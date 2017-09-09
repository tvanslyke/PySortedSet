#include "sortedsetobject.h"








PyAPI_FUNC(PyObject*) PySortedSet_arith_add(PyObject* self, PyObject* other)
{
	if(!PySortedSet_Check(other))
		Py_RETURN_NOTIMPLEMENTED;
	else if(self == other)	
		return PySortedSet_copy(self);
	else if(PySortedSet_FINALIZE(self) != 0 || PySortedSet_FINALIZE(other) != 0)
		return NULL;	
	return PySortedSet_Union(self, other);	
}

PyAPI_FUNC(PyObject*) PySortedSet_arith_sub(PyObject* self, PyObject* other)
{
	if(!PySortedSet_Check(other))
		Py_RETURN_NOTIMPLEMENTED;
	else if(self == other)	
		return PySortedSet_new(_PySortedSet_TypeObject(), NULL, NULL);
	else if(PySortedSet_FINALIZE(self) != 0 || PySortedSet_FINALIZE(other) != 0)
		return NULL;	
	return PySortedSet_Difference(self, other);	
}

PyAPI_FUNC(PyObject*) PySortedSet_arith_or(PyObject* self, PyObject* other)
{
	return PySortedSet_arith_add(self, other);	
}

PyAPI_FUNC(PyObject*) PySortedSet_arith_and(PyObject* self, PyObject* other)
{
	if(!PySortedSet_Check(other))
		Py_RETURN_NOTIMPLEMENTED;
	else if(self == other)	
		return PySortedSet_copy(self);
	else if(PySortedSet_FINALIZE(self) != 0 || PySortedSet_FINALIZE(other) != 0)
		return NULL;	
	return PySortedSet_Intersection(self, other);	
}

PyAPI_FUNC(PyObject*) PySortedSet_arith_xor(PyObject* self, PyObject* other)
{
	if(!PySortedSet_Check(other))
		Py_RETURN_NOTIMPLEMENTED;
	else if(self == other)	
		return PySortedSet_new(_PySortedSet_TypeObject(), NULL, NULL);
	else if(PySortedSet_FINALIZE(self) != 0 || PySortedSet_FINALIZE(other) != 0)
		return NULL;	
	return PySortedSet_SymmetricDifference(self, other);	
}












PyAPI_FUNC(PyObject*) PySortedSet_arith_add_eq(PyObject* self, PyObject* other)
{
	if(!PySortedSet_Check(other))
		Py_RETURN_NOTIMPLEMENTED;
	PyObject* temp = PySortedSet_arith_add(self, other);
	if(!temp)
		return NULL;
	PySortedSet_MOVE_ASSIGN(self, temp);
	Py_INCREF(self);
	return self;
}

PyAPI_FUNC(PyObject*) PySortedSet_arith_sub_eq(PyObject* self, PyObject* other)
{
	if(!PySortedSet_Check(other))
		Py_RETURN_NOTIMPLEMENTED;
	PyObject* temp = PySortedSet_arith_sub(self, other);
	if(!temp)
		return NULL;
	PySortedSet_MOVE_ASSIGN(self, temp);
	Py_INCREF(self);
	return self;
}



PyAPI_FUNC(PyObject*) PySortedSet_arith_and_eq(PyObject* self, PyObject* other)
{
	if(!PySortedSet_Check(other))
		Py_RETURN_NOTIMPLEMENTED;
	PyObject* temp = PySortedSet_arith_and(self, other);
	if(!temp)
		return NULL;
	PySortedSet_MOVE_ASSIGN(self, temp);
	Py_INCREF(self);
	return self;
}

PyAPI_FUNC(PyObject*) PySortedSet_arith_xor_eq(PyObject* self, PyObject* other)
{
	if(!PySortedSet_Check(other))
		Py_RETURN_NOTIMPLEMENTED;
	PyObject* temp = PySortedSet_arith_xor(self, other);
	if(!temp)
		return NULL;
	PySortedSet_MOVE_ASSIGN(self, temp);
	Py_INCREF(self);
	Py_RETURN_NONE;
}


PyAPI_FUNC(PyObject*) PySortedSet_arith_or_eq(PyObject* self, PyObject* other)
{
	if(!PySortedSet_Check(other))
		Py_RETURN_NOTIMPLEMENTED;
	PyObject* temp = PySortedSet_arith_or(self, other);
	if(!temp)
		return NULL;
	PySortedSet_MOVE_ASSIGN(self, temp);
	Py_INCREF(self);
	Py_RETURN_NONE;
}









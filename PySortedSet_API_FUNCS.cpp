/*
 * PySortedSet_API_FUNCS.c
 *
 *  Created on: Sep 3, 2017
 *      Author: tim
 */
extern "C"{
#include "PySortedSet.h"
}
#include <iostream>
#include <algorithm>




static PyObject** PySortedSet_unique_merge(PyObject** begin, PyObject** mid, PyObject** end)
{
	auto in_front = [&](PyObject* obj)
	{
		PyObject** loc = std::lower_bound(begin, mid, obj, PySortedSet_LessThan);
		return std::make_pair(std::upper_bound(loc, mid, obj, PySortedSet_LessThan),
				(loc < mid and not PySortedSet_LessThan(*loc, obj)));
	};
	auto pos = mid;
	auto ins_loc = begin;
	bool is_in_front = false;
	while(pos < end)
	{
		if(is_in_front)
		{

		}
		else
		{
			std::rotate(begin, ins_loc, mid);
		}
	}
	return pos;
}

static PySortedSetObject* PySortedSet_ThrowIfBadType(PyObject* self)
{
	if(!PySortedSet_Check(self))
	{
		PyErr_SetString(PyExc_RuntimeError, "Bad type encountered when invoking SortedSet method. (this is a bug)");
		PyErr_BadInternalCall();
		return nullptr;
	}
	else
		return (PySortedSetObject*)self;
}

extern "C"{
Py_ssize_t PySortedSet_SIZE(PyObject* self)
{
	return PyList_GET_SIZE(((PySortedSetObject*)self)->sorted_set_);
}
Py_ssize_t PySortedSet_Size(PyObject* self)
{
	if(PySortedSet_ThrowIfBadType(self))
		return PySortedSet_SIZE(self);
	else
		return -1;
}

Py_ssize_t PySortedSet_SORTED_COUNT(PyObject* self)
{
	return ((PySortedSetObject*)self)->sorted_count_;
}
Py_ssize_t PySortedSet_SortedCount(PyObject* self)
{
	if(PySortedSet_ThrowIfBadType(self))
		return PySortedSet_SORTED_COUNT(self);
	else
		return -1;
}









PyObject* PySortedSet_GET_ITEM(PyObject* self, Py_ssize_t index)
{
	return PyList_GET_ITEM(((PySortedSetObject*)self)->sorted_set_, index);
}
PyObject* PySortedSet_GetItem(PyObject* self, Py_ssize_t index)
{
	if(self and PySortedSet_ThrowIfBadType(self))
		return PySortedSet_GET_ITEM(self, index);
	else
		return nullptr;
}

int PySortedSet_ADD_ITEM(PyObject* self, PyObject* item)
{
	PyObject* set_list = (PyObject*)((PySortedSetObject*)self)->sorted_set_;
	return PyList_Append(set_list, item);
}
int PySortedSet_AddItem(PyObject* self, PyObject* item)
{
	if(not item)
	{
		PyErr_BadArgument();
		return -1;
	}
	if(self and PySortedSet_ThrowIfBadType(self))
		return PySortedSet_ADD_ITEM(self, item);
	else
		return -1;
}

int PySortedSet_IS_SORTED(PyObject* self)
{
	Py_ssize_t len = PySortedSet_SIZE(self);
	Py_ssize_t sorted_len = PySortedSet_SORTED_COUNT(self);
	return len == sorted_len;
}
int PySortedSet_IsSorted(PyObject* self)
{
	if(self and PySortedSet_ThrowIfBadType(self))
		return PySortedSet_IS_SORTED(self);
	else
		return -1;
}

int PySortedSet_FINALIZE(PyObject* self)
{
	if(PySortedSet_IS_SORTED(self))
	{
		return 0;
	}
	else
	{
		try
		{
			PyObject** begin = PY_SORTED_SET_BEGIN(self);
			PyObject** back_begin = PY_SORTED_SET_SORTED_END(self);
			PyObject** end = PY_SORTED_SET_END(self);
			std::stable_sort(back_begin, end, PySortedSet_LessThan);

			auto pos = back_begin;
			auto subrange_begin = pos;
			auto obj_in_front = [&](PyObject* obj){return std::binary_search(begin, back_begin, obj, PySortedSet_LessThan);};
			bool already_in_set = false;
			while(pos < end)
			{
				subrange_begin = pos;
				already_in_set = obj_in_front(*subrange_begin);
				pos = std::find_if(pos, end, [&](PyObject* v){ return not PySortedSet_Equal(v, *subrange_begin);});
				pos = std::find_if(pos, end, [&](PyObject* v){ return not obj_in_front(v);});
				subrange_begin += (!already_in_set);
				end = std::rotate(subrange_begin, pos, end);
				pos = subrange_begin;
			}
			std::inplace_merge(begin, back_begin, end, PySortedSet_LessThan);
			PyListObject* list = PY_SORTED_SET_GET_LIST(self);
			int errcode = PySequence_DelSlice((PyObject*)list,
							std::distance(begin, end),
							std::distance(begin, list->ob_item + PyList_GET_SIZE((PyObject*)list)));
			if(errcode < 0)
			{
				PyErr_BadInternalCall();
				return -1;
			}
		}
		catch(const std::exception & e)
		{
			PyErr_BadInternalCall();
			return -1;
		}
	}
	return 0;

}
int PySortedSet_Finalize(PyObject* self)
{
	if(self and PySortedSet_ThrowIfBadType(self))
		return PySortedSet_FINALIZE(self);
	else
		return -1;
}

Py_ssize_t PySortedSet_INDEX_OF(PyObject* self, PyObject* item)
{
	PySortedSet_FINALIZE(self);
	auto begin = PY_SORTED_SET_BEGIN(self);
	auto end = PY_SORTED_SET_END(self);
	auto pos = std::lower_bound(begin, end, item, PySortedSet_LessThan);
	return std::distance(begin, pos);
}
Py_ssize_t PySortedSet_IndexOf(PyObject* self, PyObject* item)
{
	if(not item)
	{
		PyErr_BadArgument();
		return -1;
	}
	if(self and PySortedSet_ThrowIfBadType(self))
		return PySortedSet_INDEX_OF(self, item);
	else
		return -1;
}


int PySortedSet_ERASE_INDEX(PyObject* self, Py_ssize_t index)
{
	if(PySequence_DelItem((PyObject*)PY_SORTED_SET_GET_LIST(self), index) != -1)
		--(((PySortedSetObject*)self)->sorted_count_);
	else
		return -1;
	return 0;
}
int PySortedSet_EraseIndex(PyObject* self, Py_ssize_t index)
{
	if(self and PySortedSet_ThrowIfBadType(self))
		return PySortedSet_ERASE_INDEX(self, index);
	else
		return -1;
}

int PySortedSet_ERASE(PyObject* self, PyObject* item)
{
	Py_ssize_t index = PySortedSet_INDEX_OF(self, item);
	Py_ssize_t len = PySortedSet_SIZE(self);
	if(index >= len)
		return false;
	auto list = PY_SORTED_SET_GET_LIST(self);
	PyObject* listitem = PyList_GetItem((PyObject*)list, index);
	if(listitem and PySortedSet_Equal(listitem, item))
		return PySortedSet_ERASE_INDEX((PyObject*)list, index) == 0 ? 1 : -1;
	else
		return 0;

}
int PySortedSet_Erase(PyObject* self, PyObject* item)
{
	if(not item)
	{
		PyErr_BadArgument();
		return -1;
	}
	if(self and PySortedSet_ThrowIfBadType(self) and item)
		return PySortedSet_ERASE(self, item);
	else
		return -1;
}
int PySortedSet_LEX_COMPARE(PyObject* self, PyObject* other)
{
	if(PySortedSet_FINALIZE(self) == -1)
	{
		PyErr_SetString(PyExc_TypeError, "Problem encountered while trying to compare SortedSet instances.");
		return -1;
	}
	else if(PySortedSet_FINALIZE(other) == -1)
	{
		PyErr_SetString(PyExc_TypeError, "Problem encountered while trying to compare SortedSet instances.");
		return -1;
	}
	try
	{
		return std::lexicographical_compare(PY_SORTED_SET_BEGIN(self), PY_SORTED_SET_END(self),
										  PY_SORTED_SET_BEGIN(other), PY_SORTED_SET_END(other),
										  PySortedSet_Equal);
	}
	catch(const std::exception & e)
	{
		PyErr_SetString(PyExc_RuntimeError, e.what());
		return -1;
	}
}
int PySortedSet_LexCompare(PyObject* self, PyObject* other)
{
	if(not other)
	{
		PyErr_BadArgument();
		return -1;
	}
	if(self and PySortedSet_ThrowIfBadType(self) and other)
		return PySortedSet_LEX_COMPARE(self, other);
	else
		return -1;
}











} /* extern "C" */

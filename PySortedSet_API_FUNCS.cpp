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
#include <vector>
#include <array>



static PyObject* PySortedSet_Merge(PyObject* self)
{
	
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

extern "C" {


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
	int errcode = PyList_Append(set_list, item);
	if(errcode == 0)
		Py_INCREF(item);
	return errcode;	
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
	else if(PY_SORTED_SET_SIZE(self))
	{
		try
		{
			PyObject** begin = PY_SORTED_SET_BEGIN(self);
			PyObject** back_begin = PY_SORTED_SET_SORTED_END(self);
			PyObject** end = PY_SORTED_SET_END(self);
			PyObject** stop = NULL;
			auto pos = back_begin;
			auto obj_in_front = [&](PyObject* obj){return std::binary_search(begin, back_begin, obj, PySortedSet_LessThan);};

			end = std::stable_partition(pos, end, [&](auto item){return not obj_in_front(item);});
			std::stable_sort(pos, end, PySortedSet_LessThan);
			PyObject* current_value = NULL;
			while(pos < end)
			{
				current_value = *pos;
				stop = std::find_if(++pos, end, [&](auto item){ return PySortedSet_LessThan(current_value, item); });
				end = std::rotate(pos, stop, end);
			}

			std::inplace_merge(begin, back_begin, end, PySortedSet_LessThan);
			PyListObject* list = PY_SORTED_SET_GET_LIST(self);
			int errcode = PySequence_DelSlice((PyObject*)list,
							std::distance(begin, end),
							std::distance(begin, list->ob_item + PyList_GET_SIZE((PyObject*)list)));
			PY_SORTED_SET_SORTED_COUNT(self) = PyList_GET_SIZE((PyObject*)list);
			if(errcode < 0)
				return -1;
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
		--PY_SORTED_SET_SORTED_COUNT(self);
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




PyAPI_FUNC(PyObject*) PySortedSet_Intersection(PyObject* sources)
{
	if(not PyTuple_Check(sources))
	{
		PyErr_BadArgument();
		return NULL;
	}
	Py_ssize_t len = PyTuple_GET_SIZE(sources);
	std::vector<PyObject*> iter_objects;
	std::vector<PyObject*> iter_values;
	try
	{
		iter_objects.reserve(len);
		iter_values.reserve(len);
	}
	catch(const std::exception & e)
	{
		PyErr_BadInternalCall();
		return NULL;
	}

	PyObject* current_item = sources;
	PySortedSetObject* set = NULL;
	PyObject* max_item = NULL;
	Py_ssize_t i = 0;
	for(i = 0; i < len; ++i)
	{
		current_item = PyTuple_GET_ITEM(sources, i);
		if(not (current_item->ob_type->tp_iter))
		{
			PyErr_SetString(PyExc_TypeError, "Attempt to build set from non-iterable.");
			goto set_intersection_return_proc;
		}
		else
		{
			iter_objects.push_back(PyObject_GetIter(current_item));
			if(not iter_objects.back())
			{
				PyErr_BadInternalCall();
				goto set_intersection_return_proc;
			}
			iter_values.push_back(PyIter_Next(iter_objects.back()));
			if(not iter_values.back())
			{
				set = (PySortedSetObject*)PySortedSet_new(_PySortedSet_TypeObject(), NULL, NULL);
				if(not set)
					PyErr_BadInternalCall();
				goto set_intersection_return_proc;
			}
		}
	}
	set = (PySortedSetObject*)(PySortedSet_new(_PySortedSet_TypeObject(), NULL, NULL));
	if(not set)
	{
		PyErr_BadInternalCall();
		goto set_intersection_return_proc;
	}
	max_item = *std::max_element(iter_values.begin(), iter_values.end(), PySortedSet_LessThan);
	while(1)
	{
		for(i = 0; i < len; ++i)
		{
			while(iter_values[i] and PySortedSet_LessThan(iter_values[i], max_item))
			{
				Py_DECREF(iter_values[i]);
				iter_values[i] = PyIter_Next(iter_objects[i]);
			}
			if(not iter_values[i])
	                        goto set_intersection_return_proc;
			else if(PySortedSet_LessThan(max_item, iter_values[i]))
			{
				max_item = iter_values[i];
				i = 0;
			}
		}
		if(PySortedSet_ADD_ITEM((PyObject*)set, max_item) != 0)
		{
			PyErr_BadInternalCall();
			Py_XDECREF(set);
			set = NULL;
			goto set_intersection_return_proc;
		}
		else
		{
			for(Py_ssize_t i = 0; i < len; ++i)
			{
				iter_values[i] = PyIter_Next(iter_objects[i]);
				if(not iter_values[i])
					goto set_intersection_return_proc;
			}
			max_item = *std::max_element(iter_values.begin(), iter_values.end(), PySortedSet_LessThan);
		}
	}
	set_intersection_return_proc:
		for(auto iter:iter_objects)
			Py_XDECREF(iter);
		for(auto iter_value:iter_values)
			Py_XDECREF(iter_value);
		return (PyObject*)set;
}

PyAPI_FUNC(PyObject*) PySortedSet_Difference(PyObject* sources)
{
        if(not PyTuple_Check(sources))
        {
                PyErr_BadArgument();
                return NULL;
        }
        Py_ssize_t len = PyTuple_GET_SIZE(sources);
        std::vector<std::pair<PyObject*, PyObject*>> iters;
        try
        {
                iters.reserve(len);
        }
        catch(const std::exception & e)
        {
                PyErr_BadInternalCall();
                return NULL;
        }
	auto sort_iters = [&]()
	{
		std::stable_sort(iters.begin(), iters.end(), [&](const auto & a, const auto & b){ return PySortedSet_LessThan(a.second, b.second);});
	};
	auto iter_pair_next = [](auto & iter_pair){ iter_pair.second = PyIter_Next(iter_pair.first);};
        PyObject* current_item = sources;
        PyObject* min_item = NULL;
        Py_ssize_t i = 0;
        PySortedSetObject* set = (PySortedSetObject*)(PySortedSet_new(_PySortedSet_TypeObject(), NULL, NULL));
        if(not set)
        {
                PyErr_BadInternalCall();
                goto set_difference_return_proc;
        }

        for(i = 0; i < len; ++i)
        {
                current_item = PyTuple_GET_ITEM(sources, i);
                if(not (current_item->ob_type->tp_iter))
                {
                        PyErr_SetString(PyExc_TypeError, "Attempt to build set from non-iterable object");
                        goto set_difference_return_proc;
                }
                else
                {
			iters.emplace_back();
                        iters.back().first = PyObject_GetIter(current_item);
                        if(not iters.back().first)
                        {
                                PyErr_BadInternalCall();
                                goto set_difference_return_proc;
                        }
                        iters.back().second = PyIter_Next(iters.back().first);
                        if(not iters.back().second)
                        {
				Py_DECREF(iters.back().first);
				iters.pop_back();
                        }
                }
        }
        while(iters.size())
        {
		sort_iters();
		min_item = iters.front().second;
		auto next_largest = std::find_if(iters.begin() + 1, iters.end(),
						 [&](const auto& iter_pair){ return PySortedSet_LessThan(min_item, iter_pair.second); });
		Py_ssize_t count = std::distance(iters.begin(), next_largest);
		if(count > 1)
		{
			std::for_each(iters.begin(), next_largest, iter_pair_next);
			for(auto it = iters.begin(); it < next_largest; ++it)
			{
				if(not it->second)
				{
					it = iters.erase(it);
					--count;
					next_largest = std::next(iters.begin(), count);
				}
			}
		}
		else
		{
			int errcode = PySortedSet_ADD_ITEM((PyObject*)set, (PyObject*)min_item);
			// TODO:  remove later
			Py_INCREF(min_item);
			if(errcode != 0)
			{
				PyErr_BadInternalCall();
				Py_XDECREF(set);
				set = NULL;
				goto set_difference_return_proc;
			}
			iter_pair_next(iters.front());
			if(not iters.front().second)
			{
				iters.erase(iters.begin());
			}
		}
        }





        set_difference_return_proc:
                for(auto & iter:iters)
                {
			Py_XDECREF(iter.first);
			Py_XDECREF(iter.second);
		}
                return (PyObject*)set;

}
PyAPI_FUNC(PyObject*) PySortedSet_FROM_ITERABLE(PyObject* iterable)
{
	PyObject* set = PySortedSet_new(_PySortedSet_TypeObject(), NULL, NULL);
	if(not set)
		return NULL;
	return _PyList_Extend(PY_SORTED_SET_GET_LIST(set), iterable);
}


PyAPI_FUNC(PyObject*) PySortedSet_FromIterable(PyObject* iterable)
{
	// changed so that work is outsourced to the internal list object.
	// type checking is done there.
	return PySortedSet_FROM_ITERABLE(iterable);
}











PyAPI_FUNC(PyObject*) PySortedSet_UpdateFromIterable(PyObject* self, PyObject* iterable)
{
	if(iterable == self)
		Py_RETURN_NONE;
		
	PyObject* result = _PyList_Extend(PY_SORTED_SET_GET_LIST(self), iterable);
	if(not result)
		return NULL;

	Py_RETURN_NONE;	
}



} /* extern "C" */

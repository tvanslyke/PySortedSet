/*
 * PySortedSet_API_FUNCS.c
 *
 *  Created on: Sep 3, 2017
 *      Author: tim
 */
extern "C"{
#include "sortedsetobject.h"
}

#include <iostream>
#include <iterator>
#include <algorithm>
#include <vector>
#include <tuple>



static constexpr const size_t imm_type_count = 8;
static std::array<PyTypeObject*, imm_type_count> init_immutable_type_array()
{
	std::array<PyTypeObject*, imm_type_count> imm_types
	{
		&PyFrozenSet_Type,
		&PyBytes_Type,
		&PyBool_Type,
		&PyUnicode_Type,
		&PyLong_Type,
		&PyInt_Type,
		&PyFloat_Type,
		&PyComplex_Type
	};
	std::sort(imm_types.begin(), imm_types.end());
	return imm_types;
}

static bool PyTypeIsImmutable(PyTypeObject* typeobj)
{
	static const std::array<PyTypeObject*, imm_type_count> imm_types = init_immutable_type_array();
	return std::binary_search(imm_types.begin(), imm_types.end(), typeobj);
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


int PySortedSet_ObjectIsImmutable(PyObject* obj)
{
	bool is_immutable = PyTypeIsImmutable(obj->ob_type);
	if(is_immutable)
		return true;
	else if(PyTuple_CheckExact(obj))
	{
		PyObject** tup_begin = (PyObject**)(((PyTupleObject*)obj)->ob_item);
		PyObject** tup_end = (PyObject**)(((PyTupleObject*)obj)->ob_item + PyTuple_GET_SIZE(obj));
		/* NOTE:  This can recurse infinitely if (if and only if)  a tuple is passed in that contains itself.  
		 *	  This can't happen in Python userland, but perhaps a questionable C extension could generate
		 *        such a situation.
		 */
		return std::find_if(tup_begin, tup_end, PySortedSet_ObjectIsImmutable) == tup_end; 
	}
	return false;
}




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



PyAPI_FUNC(PyObject*) PySortedSet_MultiIntersection(PyObject* sources)
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



PyAPI_FUNC(int) PySortedSet_MultiDifferenceUpdate(PyObject* self, PyObject* others)
{
	std::vector<std::pair<PyObject**, PyObject**>> search_bounds;
	try
	{
		search_bounds.reserve(PyTuple_GET_SIZE(others));
		for(Py_ssize_t i = 0; i < PyTuple_GET_SIZE(others); ++i)
			search_bounds.emplace_back(PY_SORTED_SET_BEGIN(PyTuple_GET_ITEM(others, i)), 
				       		   PY_SORTED_SET_END(PyTuple_GET_ITEM(others, i))); 
	}
	catch(const std::exception& e)
	{
		PyErr_BadInternalCall();
		return -1;
	}
	PyObject** pos = PY_SORTED_SET_BEGIN(self);
	PyObject** end = PY_SORTED_SET_END(self);
	bool found = false;
	while(pos < end and search_bounds.size())
	{	
		found = false;
		for(Py_ssize_t i = 0; i < search_bounds.size(); ++i)
		{
			search_bounds[i].first = std::lower_bound(search_bounds[i].first, search_bounds[i].second, *pos, PySortedSet_LessThan);
			if(search_bounds[i].first == search_bounds[i].second)
				search_bounds.erase(search_bounds.begin() + i);
			else
				found = found or (not PySortedSet_LessThan(*pos, *(search_bounds[i].first))); 
		}
		if(found)
		{
			end = std::rotate(pos, pos + 1, end);
			PY_SORTED_SET_SORTED_COUNT(self) -= 1;
		}
		else 
			++pos;
	}
	if(PySequence_DelSlice(PY_SORTED_SET_GET_LIST_AS_OBJ(self), PY_SORTED_SET_SORTED_COUNT(self), PY_SORTED_SET_SIZE(self)) != 0)
		return -1;
	return 0;

}

PyObject* PySortedSet_MultiUnion(PyObject* sources)
{
        if(not PyTuple_Check(sources))
        {
                PyErr_BadArgument();
                return NULL;
        }
        Py_ssize_t len = PyTuple_GET_SIZE(sources);
	
        PyObject* iter = NULL;
        Py_ssize_t i = 0;
        PySortedSetObject* set = (PySortedSetObject*)(PySortedSet_new(_PySortedSet_TypeObject(), NULL, NULL));
        if(not set)
        {
        	return NULL;
	}
        for(i = 0; i < len; ++i)
        {
	
		iter = PyObject_GetIter(PyTuple_GET_ITEM(sources, i));
                if(not iter)
			return NULL;
		else
		{	
			PyObject* value = PyIter_Next(iter);
			if(value)
			{
				while(value)
				{
					PySortedSet_ADD_ITEM((PyObject*)set, value);
					value = PyIter_Next(iter);
				}
				if(PySortedSet_FINALIZE((PyObject*)set) != 0)
				{
					Py_DECREF(set);
					return NULL;	
				}
			}
			Py_DECREF(iter);
		}
	}
	return (PyObject*)set;
	
}



PyAPI_FUNC(PyObject*) PySortedSet_FROM_ITERABLE(PyObject* iterable)
{
	PyObject* set = PySortedSet_new(_PySortedSet_TypeObject(), NULL, NULL);
	if(not set)
		return NULL;
	if(not _PyList_Extend(PY_SORTED_SET_GET_LIST(set), iterable))
		return NULL;	
	return set;
	
}


PyAPI_FUNC(PyObject*) PySortedSet_FromIterable(PyObject* iterable)
{
	// changed so that work is outsourced to the internal list object.
	// type checking is done there.
	return PySortedSet_FROM_ITERABLE(iterable);
}



int PySortedSet_Resort(PyObject* self)
{
	PyObject** begin = PY_SORTED_SET_BEGIN(self);
	PyObject** back_begin = PY_SORTED_SET_SORTED_END(self);
	PyObject** pos = begin;
	auto adj_comp_func = [&](PyObject* a, PyObject* b){ return not PySortedSet_LessThan(a, b); };
	auto find_disordered_position = [&](PyObject** posn)
	{
		// assumes 'pos' > 'begin'
		while (1)
		{
			// only mutable objects will be out-of-order
			posn = std::find_if(posn, back_begin, [&](PyObject* obj){ return not PySortedSet_ObjectIsImmutable(obj); });
			if (posn < back_begin)
				if (not PySortedSet_LessThan(posn[-1], posn[0]))
					// 'posn[0]' is out-of-order with 'posn[-1]'
					return posn;
				else if ((posn + 1) < back_begin)
					// 'posn[0]' may still be out-of-order with posn[1]  
					if (PySortedSet_ObjectIsImmutable(posn[1]))
						// we can elide an exra comparison if 'posn[1]' is immutable, which is (usually) cheap to check.
						if (not PySortedSet_LessThan(posn[0], posn[1]))
							// if the next item is immutable, we know for a fact that 'posn[0]' is the out-of-order element
							return posn;
						else 
							// not out-of-order, proceed to next position
							++posn;
					else if (not PySortedSet_LessThan(posn[0], posn[1]))
						// return 'posn' if 'posn[1]' belongs before 'posn[0]' but after 'posn[-1]'
						return posn + (not PySortedSet_LessThan(posn[-1], posn[1]));
					else 
						// not out-of-order, proceed to next position
						++posn;			
				else 
					// nothing else is out-of-order
					return back_begin;
			else 
				// return immediately if posn == back_begin
				return posn;
		}
	};
	
	pos = find_disordered_position(pos);
	while(pos < back_begin)
	{
		back_begin = std::rotate(pos, std::next(pos, 1), back_begin);
		pos = find_disordered_position(pos);
	}
	PY_SORTED_SET_SORTED_COUNT(self) = std::distance(begin, back_begin);
	if(PySortedSet_FINALIZE(self) != 0)
		return -1;	
	
	return 0;
}



static int PySortedSet_ArithInit(PyObject* self, PyObject* other,
				PyObject** left_iter, PyObject** left_value,
				PyObject** right_iter, PyObject** right_value)
{
	if(!PySortedSet_Check(self) or !PySortedSet_Check(other))
	{
		PyErr_SetString(PyExc_TypeError, "Attempt to perform SortedSet arithmetic on non SortedSet instances.");
		return -1;
	}
	if(other == self)
		return 1;	
	
	else if(PySortedSet_FINALIZE(self) != 0 || PySortedSet_FINALIZE(other) != 0)
		return -1;

	*left_iter = PyObject_GetIter(self);
	*right_iter = PyObject_GetIter(other);
	if(!(*left_iter) || !(*right_iter))
	{
		Py_XDECREF(*left_iter);
		Py_XDECREF(*right_iter);
		return -1;
	}
	*left_value = PyIter_Next(*left_iter);
	*right_value = PyIter_Next(*right_iter);
	return 0;		
}










int PySortedSet_IsSubset(PyObject* self, PyObject* other)
{
	PyObject** lbegin = PY_SORTED_SET_BEGIN(self);
	PyObject** lend = PY_SORTED_SET_END(self);
	PyObject** rbegin = PY_SORTED_SET_BEGIN(other);
	PyObject** rend = PY_SORTED_SET_END(other);
	return std::includes(lbegin, lend, rbegin, rend, PySortedSet_LessThan);
}




PyObject* PySortedSet_Union(PyObject* self, PyObject* other)
{
	PyObject* resultant_set = PySortedSet_new(_PySortedSet_TypeObject(), NULL, NULL);
       	if(!resultant_set)
		return NULL;
	Py_ssize_t reserve_size = PY_SORTED_SET_SIZE(self) + PY_SORTED_SET_SIZE(other);

	PyObject** lbegin = PY_SORTED_SET_BEGIN(self);
	PyObject** lend = PY_SORTED_SET_END(self);
	PyObject** rbegin = PY_SORTED_SET_BEGIN(other);
	PyObject** rend = PY_SORTED_SET_END(other);
	
	PyObject* list = PyList_New(reserve_size);
	if(!list)
	{
		Py_DECREF(resultant_set);
		return NULL;
	}
	auto & listlen = ((PyListObject*)(list))->ob_size;
	PyObject** listitems = _PyList_ITEMS(list);
	listlen = std::set_union(lbegin, lend, rbegin, rend, listitems, PySortedSet_LessThan) - listitems;
	std::for_each(listitems, listitems + listlen, [](PyObject* obj){Py_INCREF(obj);});
	PySortedSet_MOVE_ASSIGN_LIST(resultant_set, list);
	PY_SORTED_SET_SORTED_COUNT(resultant_set) = listlen;
	return resultant_set;
}









PyObject* PySortedSet_Difference(PyObject* self, PyObject* other)
{
	PyObject* resultant_set = PySortedSet_new(_PySortedSet_TypeObject(), NULL, NULL);
       	if(!resultant_set)
		return NULL;
	Py_ssize_t reserve_size = PY_SORTED_SET_SIZE(self);

	PyObject** lbegin = PY_SORTED_SET_BEGIN(self);
	PyObject** lend = PY_SORTED_SET_END(self);
	PyObject** rbegin = PY_SORTED_SET_BEGIN(other);
	PyObject** rend = PY_SORTED_SET_END(other);
	
	PyObject* list = PyList_New(reserve_size);
	if(!list)
	{
		Py_DECREF(resultant_set);
		return NULL;
	}
	auto & listlen = ((PyListObject*)(list))->ob_size;
	PyObject** listitems = _PyList_ITEMS(list);
	listlen = std::set_difference(lbegin, lend, rbegin, rend, listitems, PySortedSet_LessThan) - listitems;
	std::for_each(listitems, listitems + listlen, [](PyObject* obj){Py_INCREF(obj);});
	PySortedSet_MOVE_ASSIGN_LIST(resultant_set, list);
	PY_SORTED_SET_SORTED_COUNT(resultant_set) = listlen;
	return resultant_set;
}



PyObject* PySortedSet_SymmetricDifference(PyObject* self, PyObject* other)
{
	PyObject* resultant_set = PySortedSet_new(_PySortedSet_TypeObject(), NULL, NULL);
       	if(!resultant_set)
		return NULL;
	Py_ssize_t reserve_size = PY_SORTED_SET_SIZE(self) + PY_SORTED_SET_SIZE(other);

	PyObject** lbegin = PY_SORTED_SET_BEGIN(self);
	PyObject** lend = PY_SORTED_SET_END(self);
	PyObject** rbegin = PY_SORTED_SET_BEGIN(other);
	PyObject** rend = PY_SORTED_SET_END(other);
	
	PyObject* list = PyList_New(reserve_size);
	if(!list)
	{
		Py_DECREF(resultant_set);
		return NULL;
	}
	auto & listlen = ((PyListObject*)(list))->ob_size;
	PyObject** listitems = _PyList_ITEMS(list);
	listlen = std::set_symmetric_difference(lbegin, lend, rbegin, rend, listitems, PySortedSet_LessThan) - listitems;
	std::for_each(listitems, listitems + listlen, [](PyObject* obj){Py_INCREF(obj);});
	PySortedSet_MOVE_ASSIGN_LIST(resultant_set, list);
	PY_SORTED_SET_SORTED_COUNT(resultant_set) = listlen;
	return resultant_set;
}


PyAPI_FUNC(PyObject*) PySortedSet_Intersection(PyObject* self, PyObject* other)
{

	PyObject* resultant_set = PySortedSet_new(_PySortedSet_TypeObject(), NULL, NULL);
       	if(!resultant_set)
		return NULL;
	Py_ssize_t reserve_size = std::min(PY_SORTED_SET_SIZE(self), PY_SORTED_SET_SIZE(other));

	PyObject** lbegin = PY_SORTED_SET_BEGIN(self);
	PyObject** lend = PY_SORTED_SET_END(self);
	PyObject** rbegin = PY_SORTED_SET_BEGIN(other);
	PyObject** rend = PY_SORTED_SET_END(other);
	
	PyObject* list = PyList_New(reserve_size);
	if(!list)
	{
		Py_DECREF(resultant_set);
		return NULL;
	}
	auto & listlen = ((PyListObject*)(list))->ob_size;
	PyObject** listitems = _PyList_ITEMS(list);
	listlen = std::set_intersection(lbegin, lend, rbegin, rend, listitems, PySortedSet_LessThan) - listitems;
	std::for_each(listitems, listitems + listlen, [](PyObject* obj){Py_INCREF(obj);});
	PySortedSet_MOVE_ASSIGN_LIST(resultant_set, list);
	PY_SORTED_SET_SORTED_COUNT(resultant_set) = listlen;
	return resultant_set;
}




PyAPI_FUNC(int) PySortedSet_HasIntersection(PyObject* self, PyObject* other)
{
	PyObject** lpos = PY_SORTED_SET_BEGIN(self);
	PyObject** lend = PY_SORTED_SET_END(self);
	PyObject** rpos = PY_SORTED_SET_BEGIN(other);
	PyObject** rend = PY_SORTED_SET_END(other);
	if(not (std::distance(lpos, lend) and std::distance(rpos, rend)))
		return false;

	if(PySortedSet_LessThan(*rpos, *lpos))
	{
		std::swap(lpos, rpos);
		std::swap(lend, rend);
	}

	while(lpos < lend and rpos < rend)
	{
		lpos = std::lower_bound(lpos, lend, *rpos, PySortedSet_LessThan);
		if(lpos == lend)
			return false;
		else if(not PySortedSet_LessThan(*rpos, *lpos))
			return true;
		rpos = std::lower_bound(rpos, rend, *lpos, PySortedSet_LessThan);
		if(rpos == rend)
			return false;
		else if(not PySortedSet_LessThan(*lpos, *rpos))
			return true;
	}
	return false;
	
}








/* 'self' should be finalized before calling this function. */
PyAPI_FUNC(PyObject*) PySortedSet_HandleSlice(PyObject* self, PyObject* slc)
{
	Py_ssize_t len = PY_SORTED_SET_SIZE(self);
	Py_ssize_t start, stop, step, slicelen;
	int errcode = PySlice_GetIndicesEx((PySliceObject*)slc, len, &start, &stop, &step, &slicelen);
	if(errcode != 0)
		return NULL;
	PyObject* listslc = PyList_New(slicelen);
	if(not listslc)
		return NULL;
	
	PyObject** begin = PY_SORTED_SET_BEGIN(self);
	PyObject* tmp = NULL;
	for(Py_ssize_t i = 0; i < slicelen; ++i)
	{
		tmp = begin[start + step * i];
		Py_INCREF(tmp);
		PyList_SET_ITEM(listslc, i, tmp);
	}
	PyObject** end = PY_SORTED_SET_END(self);
	PyObject** pending_begin = begin;
	PyObject** pending_end = begin;
	Py_ssize_t slc_idx = 0;
	
	for(Py_ssize_t i = 0; i < slicelen; ++i)
	{		
		slc_idx = start + step * i;
		tmp = begin[slc_idx];
		if(not PySortedSet_ObjectIsImmutable(tmp))
		{
			pending_begin = std::rotate(pending_begin, pending_end, begin + slc_idx); 
			pending_end = begin + slc_idx + 1;
		}
	}
	end = std::rotate(pending_begin, pending_end, end);
	PY_SORTED_SET_SORTED_COUNT(self) = std::distance(begin, end);
	return listslc;
}


PyAPI_FUNC(PyObject*) PySortedSet_HandleIndex(PyObject* self, PyObject* idx)
{
	Py_ssize_t len = PY_SORTED_SET_SIZE(self);
	Py_ssize_t index = PyNumber_AsSsize_t(idx, PyExc_IndexError);
	if(index == -1 and PyErr_Occurred())
		return NULL;
	index += (index < 0) * len;
	if(index < 0 or index >= len)
	{
		PyErr_SetString(PyExc_IndexError, "Index out-of-bounds on SortedSet() instance.");
		return NULL;
	}
	PyObject* elem = PyList_GET_ITEM((PyObject*)PY_SORTED_SET_GET_LIST(self), index);
	if(not PySortedSet_ObjectIsImmutable(elem))
	{
		PyObject** begin = PY_SORTED_SET_BEGIN(self);
		PyObject** end = PY_SORTED_SET_END(self);
		PY_SORTED_SET_SORTED_COUNT(self) = (std::rotate(begin + index, begin + index + 1, end) - begin);		
	}
	return elem;
}



PyAPI_FUNC(int) PySortedSet_HandleIndexAssignment(PyObject* self, PyObject* idx, PyObject* value)
{
        Py_ssize_t len = PY_SORTED_SET_SIZE(self);
        Py_ssize_t index = PyNumber_AsSsize_t(idx, PyExc_IndexError);
        if(index == -1 and PyErr_Occurred())
                return -1;
        index += (index < 0) * len;
        if(index < 0 or index >= len)
        {
                PyErr_SetString(PyExc_IndexError, "Index out-of-bounds on SortedSet() instance.");
                return -1;
        }
	
	Py_XINCREF(value);	
	int errcode = PyList_SetItem((PyObject*)PY_SORTED_SET_GET_LIST(self), index, value);
	if(errcode != 0)
		return errcode;
	PyObject** begin = PY_SORTED_SET_BEGIN(self);
	PyObject** end = PY_SORTED_SET_END(self);
	Py_ssize_t last_item = (std::rotate(begin + index, begin + index + 1, end) - begin);
	PY_SORTED_SET_SORTED_COUNT(self) = last_item;
	return 0;
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

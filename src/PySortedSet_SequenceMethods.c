
#include "sortedsetobject.h"

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

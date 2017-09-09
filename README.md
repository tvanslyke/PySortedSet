# PySortedSet
Currently still in development.

Lazy ordered set class written in C and C++14 for CPython.  This module offers an alternative to CPython's `set` class.

While Python's built-in `set` uses hashing with open addressing to search for elements in a set, `SortedSet` uses binary search.  Additionally, `SortedSet` will not emit an exception when you attempt to insert a mutable object (although, technically, you can put a mutable object in a `set`, so long as it is hashable).  This has a few consequences:
* `SortedSet`s can, and do, offer random access.  The implementation is an array internally (rather than a binary search tree, which offers poor cache performance), and `SortedSet` instances can be accessed through indices and slices just like `list` objects.
* `SortedSet`s are, as a rule, are slower to query than `set`s. (this is in part because of the O(1) complexity of hashing vs O(log(N))for binary search, but also because branching is particularly slow in Python).
* The barrier to entry into a `SortedSet` is very low.  All that is required is that `cmp(my_object, something_else)` is defined for any given type (and by default, this is the case).
* The implementation can be "broken" by mutating an object in a `SortedSet` from an external reference.  That is, you can break the ordering by changing the value of a set element without the set object knowing.  Since [https://mail.python.org/pipermail/tutor/2003-October/025932.html]("we're all adults here"), I won't stop you.  There is some functionality to deal with this quite easily.
* `SortedSet` is usually lighter on memory usage than `set`.  Sparse hash tables, by definition, over-allocate memory.  `SortedSet` behaves exactly like a `list` object of the same length.


Overall, `SortedSet` is a (almost certainly - I haven't *actually* measured) slower, but more flexible alternative to Python's built-in `set`.  If it helps you write better code, awesome!

# Demo
The following example shows some of the functionality:
```
# you can create a sorted set just like a normal 'set()' object
my_set = Set([1,2,3])
print my_set

# however, with SortedSet's, you can add mutable objects like 'list()'s and 'dict()'s
my_list1 = [1,2,4]
my_list2 = [1,2,3]
my_set.update((my_list1, my_list2))
print my_set

# you can mutate these objects in-place, and set will adjust their positions accordingly
# (for non-mutable types, the implementation elides this operation)
my_set[-1][-1] = -100
print my_set

# however, there's no way to detect out-of-place mutations (mutating an reference to an
# item in the set from outside of the set's '__getitem__()' method), so be careful when
# you do things like this:
my_list2[0] = -9
print my_set

# now, "we're all adults here", so rather than disallowing mutable objects, you can fix
# any issues that you create by calling the .resort() method
my_set.resort()
print my_set
```
### Output:
SortedSet([1, 2, 3])
SortedSet([1, 2, 3, [1, 2, 3], [1, 2, 4]])
SortedSet([1, 2, 3, [1, 2, -100], [1, 2, 3]])
SortedSet([1, 2, 3, [1, 2, -100], [-9, 2, 3]])
SortedSet([1, 2, 3, [-9, 2, 3], [1, 2, -100]])


# Implementation
`SortedSet` is just a wrapper around a `list` object under the hood.  `SortedSet` lazily sorts its internal `list` by using the `<`/`cmp()` operators to impose a strict, weak ordering.  Lets say we currently have a `SortedSet` with three elements:
``` 
my_set = SortedSet([1,2,3])
``` 
If you call the `.add()` method, say 5 times:
```
for item in [4, 2, -1, -1, 20]:
	my_set.add(item) # or 'my_set.update([4, 2, -1, -1, 20])'
```
and don't trigger the sorting operation, then internally, the structure actually looks like this:
```
SortedSet:
	internal_list: [1, 2, 3 | 4, 2, -1, -1, 20]
	num_sorted: 3
```
Once the sorting operation is triggered, the unsorted region is sorted and filtered of any repeats (either repeats of it's own elements or repeats of elements already in the sorted region):

```
SortedSet:
	#               sorted    filtered    extras
	internal_list: [1, 2, 3 | -1, 4, 20 | -1, 2]
	num_sorted: 3
```
Then the two regions are merged and the trailing extraneous elements are deallocated:

```
SortedSet:
	internal_list: [-1, 1, 2, 3, 4, 20]
	num_sorted: 6
```
This means that calls to `.add()` and `.update()` are O(1) in the size of the set.  This has the consequence that many of the methods which trigger the sorting operation are or O(1) amortized.

Other implementation details will expanded on in the future:
* Speed of some operations can be sped up significantly if the operation involves access to an immutable element (`str`, `float`, `bool`, sometimes `tuple`)
* How `.resort()` works.

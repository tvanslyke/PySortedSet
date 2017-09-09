# PySortedSet
Lazy ordered set class written in C and C++14 for CPython.  Still in development, but it compiles and works properly (until it doesn't).


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



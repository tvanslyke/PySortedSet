# PySortedSet
Lazy ordered set class written in C and C++14 for CPython.  Still in development, but it compiles and works properly (until it doesn't).


# Demo
The following example shows some of the functionality:

>>> from SortedSet import SortedSet                                                                                                               
>>> my_set = SortedSet([1, 2, 3])                                                                                                                 
>>> my_set                                                                                                                                        
SortedSet([1, 2, 3])                                                                                                                              
>>> my_set.add([1, 2, 3])                                                                                                                         
>>> my_set.add([2, 1, 0])                                                                                                              
>>> my_set                                                                                                                                        
SortedSet([1, 2, 3, [1, 2, 3], [2, 1, 0]])
>>> my_set[-1][0] = -100
>>> my_set
SortedSet([1, 2, 3, [-100, 1, 0], [1, 2, 3]])
>>> my_set.add(my_set)
>>> my_set.add(my_set)
>>> my_set
SortedSet([1, 2, 3, SortedSet([...]), [-100, 1, 0], [1, 2, 3]])
>>> for item in my_set:
...     print item
... 
1
2
3
SortedSet([1, 2, 3, SortedSet([...]), [-100, 1, 0], [1, 2, 3]])
[-100, 1, 0]
[1, 2, 3]
>>> 
 

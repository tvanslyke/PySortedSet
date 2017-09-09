from distutils.core import setup, Extension
from os import getcwd
sorted_set = Extension('SortedSet',
                    sources = ['src/PySortedSet.c', 
                               'src/PySortedSet_API_FUNCS.cpp', 
                               'src/PySortedSet_NamedMethods.c', 
                               'src/PySortedSet_NumberMethods.c',
                               'src/PySortedSet_SequenceMethods.c',
                               'src/PySortedSet_MappingMethods.c',
                               'src/PySortedSet_Internal.c'],
                    extra_compile_args = ['-std=c++14', '-std=c11', '-I./Include'],
                    )

setup (name  = 'SortedSet',
       version = '0.1',
       description = 'SortedSet',
       author = 'Timothy Van Slyke',
       author_email = 'vanslyke.t@husky.neu.edu',
       ext_modules = [sorted_set])

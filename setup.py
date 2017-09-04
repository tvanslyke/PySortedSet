from distutils.core import setup, Extension

sorted_set = Extension('SortedSet',
                    sources = ['PySortedSet.c', 'PySortedSet_API_FUNCS.cpp'])

setup (name  = 'SortedSet',
       version = '1.0',
       description = 'This is a demo package',
       author = 'Timothy Van Slyke',
       author_email = 'vanslyke.t@husky.neu.edu',
       ext_modules = [sorted_set])

# Rcpp - Rusts Rc type in C++

This repository contains a implementation of Rusts Rc type in C++.
It is just a thought experiment to see how transferring such a concept would work.

The most important differences to C++ shared_ptr:

* No atomic reference counting, like Rc, it should only be used in single-threaded contexts
* Only a single pointer in size, but does not support polymorphism (use PRc for polymorpic usage)

Compared to Rusts Rc type, the C++ Rc type can however be dangling, as C++ move semantics encourage a "nullptr" variant.
This could be fixed by disabling the move constructor on this type.

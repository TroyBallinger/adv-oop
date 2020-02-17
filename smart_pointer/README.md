# CS 540 Programming Assignment 3: Smart Pointer

Implement a non-intrusive, thread-safe, exception-safe, reference-counting smart pointer named cs540::SharedPtr. Our model for this will be std::shared_ptr, so you can read up on that to understand how it should behave.

SharedPtr must allow different smart pointers in different threads to be safely assigned and unassigned to the same shared objects. You may use either locks or atomic increment operations to ensure thread-safety. You do not need to (and in fact should not) ensure that the same SharedPtr can be used concurrently.

Note that when the smart pointer determines that the object should be deleted, it must delete the object via a pointer to the original type, even if the template argument of the final smart pointer is of a base type. This is to avoid object slicing for non-virtual destructors.
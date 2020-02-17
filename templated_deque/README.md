# CS 440/540: Container in C

You will use a macro to implement a “template” for a double-ended queue container, known as a deque, in C. As a “template”, it will be able to contain any type without using void * (which violates type safety). The requirements are given via the test.cpp program, linked below. You are to write the macro so that this program will compile and run properly. You must implement the deque as a circular, dynamic array. In other words, you are not allowed to use a linked list.

You should use two struct types: one to represent the container itself and a another for the iterator. The struct type for the container itself would contain any bookkeeping information that you need. The iterator struct type would contain any bookkeeping information that you need to maintain the iterator.

Since the container must have slightly different source code for each type, you cannot code it directly. Instead, you need to write a long macro, named Deque_DEFINE(). The macro takes one argument, which is the contained type, provided as a typedef if it is not already single word. This macro will contain all the definitions needed for the container. By making the name of the contained type part of the generated classes and functions, a separate instance of the source code is generated for each contained type.

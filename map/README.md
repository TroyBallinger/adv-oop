# CS 540 Programming Assignment 2: Container

Implement a container class template named Map similar to the std::map class from the C++ Standard Library. Such containers implement key-value pairs, where key and value may be any types, including class types. As in std::map, the mapped type values themselves must be in your map, not pointers to the values.

You may assume that the Key_T and Mapped_T are copy constructible and destructible. You may assume that Key_T has a less-than operator (<), and an equality operator (==), as free-standing functions (not member functions). You may also assume that Mapped_T has an equality comparison (==). If operator< is invoked on Map objects, you may also assume that Mapped_T has operator<. You may not assume that either class has a default constructor or an assignment operator. You may only assume that a Mapped_T that is used with operator[] may be default initialized. You may not make any other assumptions. 

Your Map class must expose three nested classes: Iterator, ConstIterator, and ReverseIterator. None of these classes should permit default construction.

Additionally, your class must meet the following time complexity requirements: O(lg(N)) for key lookup, insertion, and deletion; O(1) for all iterator increments and decrements; and O(N) for copy construction and assignment. 
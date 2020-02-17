#ifndef Deque_DEFINE
#define STARTING_SIZE 40									
#define Deque_DEFINE(type)									\
struct Deque_##type##_Iterator{									\
	type* current;										\
	void (*inc)(Deque_##type##_Iterator*);							\
	void (*dec)(Deque_##type##_Iterator*);							\
	type &(*deref)(Deque_##type##_Iterator*);						\
};												\
struct Deque_##type {										\
	type *array;										\
	size_t length, capacity, ignored;							\
	bool (*compare)(const type&, const type&);						\
	type &(*at)(Deque_##type *, int);							\
	size_t (*size)(Deque_##type *);								\
	bool (*empty)(Deque_##type *);								\
	void (*clear)(Deque_##type *);								\
	void (*dtor)(Deque_##type *);								\
	type &(*front)(Deque_##type *);								\
	type &(*back)(Deque_##type *);								\
	Deque_##type##_Iterator (*begin)(Deque_##type *);					\
	Deque_##type##_Iterator (*end)(Deque_##type *);						\
	void (*push_front)(Deque_##type *, type);						\
	void (*push_back)(Deque_##type *, type);						\
	void (*pop_front)(Deque_##type *);							\
	void (*pop_back)(Deque_##type *);							\
	void (*sort)(Deque_##type *, Deque_##type##_Iterator, Deque_##type##_Iterator);		\
	char type_name[strlen(#type) + 7] = "Deque_" #type;					\
};												\
void Deque_##type##_clear(Deque_##type *deque) {						\
	memset(deque->array, 0, deque->capacity * sizeof(type));				\
}												\
void Deque_##type##_dtor(Deque_##type *deque) {							\
	free(deque->array);									\
}												\
type &Deque_##type##_front(Deque_##type *deque) { return deque->array[deque->ignored]; }	\
type &Deque_##type##_back(Deque_##type *deque) { 						\
	return deque->array[deque->ignored + deque->length - 1]; 				\
}												\
void Deque_##type##_push_front(Deque_##type *deque, type insert) {				\
	if (deque->ignored + deque->length == deque->capacity) {  				\
		deque->capacity *= 2;								\
		deque->array = (type*)realloc(deque->array, deque->capacity * sizeof(type));	\
	}											\
	if (deque->ignored > 0) { 								\
		deque->array[deque->ignored] = insert;						\
		deque->ignored--;								\
	} else {										\
		for (int i_move = deque->length; i_move > 0; i_move--) {			\
			deque->array[i_move] = deque->array[i_move-1];				\
		}										\
		deque->array[0] = insert;							\
	}											\
	deque->length++;									\
}												\
void Deque_##type##_push_back(Deque_##type *deque, type insert) {				\
	if (deque->ignored + deque->length == deque->capacity) {  				\
		deque->capacity *= 2;								\
		deque->array = (type*)realloc(deque->array, deque->capacity * sizeof(type));	\
	}											\
	deque->array[deque->ignored + deque->length] = insert;					\
	deque->length++;									\
}												\
void Deque_##type##_pop_front(Deque_##type *deque) {						\
	deque->length--;									\
	deque->ignored++;									\
}												\
void Deque_##type##_pop_back(Deque_##type *deque) { deque->length--; }				\
bool Deque_##type##_equal(Deque_##type d1, Deque_##type d2) {					\
	if (d1.length != d2.length) { return false; }						\
	for (int i_move = 0; i_move < d1.length; i_move++) {					\
		if (d1.compare(d1.array[i_move], d2.array[i_move])				\
		|| d2.compare(d2.array[i_move], d1.array[i_move])) { return false; }		\
	}											\
	return true;										\
}												\
type &Deque_##type##_at(Deque_##type *deque, int i) {						\
	assert(i > -1 && i < deque->length);							\
	return deque->array[i + deque->ignored];						\
}												\
size_t Deque_##type##_size(Deque_##type *deque) { return deque->length; }			\
bool Deque_##type##_empty(Deque_##type *deque) { return (deque->size(deque)==0); }		\
type &Deque_##type##_Iterator_deref(Deque_##type##_Iterator* iter) {				\
	return iter->current[0];								\
}												\
void Deque_##type##_Iterator_inc(Deque_##type##_Iterator* iter) {				\
	iter->current++;									\
}												\
void Deque_##type##_Iterator_dec(Deque_##type##_Iterator* iter) {				\
	iter->current--;									\
}												\
bool Deque_##type##_Iterator_equal(Deque_##type##_Iterator it1, Deque_##type##_Iterator it2) {	\
	return it1.current == it2.current;							\
}												\
Deque_##type##_Iterator Deque_##type##_begin(Deque_##type *deque) {				\
	Deque_##type##_Iterator iter;								\
	iter.current = &(deque->array[deque->ignored]);						\
	iter.inc = &Deque_##type##_Iterator_inc;						\
	iter.dec = &Deque_##type##_Iterator_dec;						\
	iter.deref = &Deque_##type##_Iterator_deref;						\
	return iter;										\
}												\
Deque_##type##_Iterator Deque_##type##_end(Deque_##type *deque) {				\
	Deque_##type##_Iterator iter; 								\
	iter.current = &(deque->array[deque->ignored + deque->length]); 			\
	iter.inc = &Deque_##type##_Iterator_inc;						\
	iter.dec = &Deque_##type##_Iterator_dec;						\
	iter.deref = &Deque_##type##_Iterator_deref;						\
	return iter;										\
}												\
void Deque_##type##_sort(Deque_##type * deque, Deque_##type##_Iterator i1, 			\
Deque_##type##_Iterator i2) {									\
	type* lower_bound = i1.current;								\
	while((lower_bound + 1) != i2.current) {						\
		type* curr = lower_bound;							\
		while((curr + 1) != i2.current) {						\
			if (deque->compare(curr[1], lower_bound[0])) {				\
				type holder = curr[1];						\
				curr[1] = lower_bound[0];					\
				lower_bound[0] = holder;					\
			}									\
			curr++;									\
		}										\
		lower_bound++;									\
	}											\
}												\
void Deque_##type##_ctor(Deque_##type *deque, bool (*comp)(const type&, const type&)) {		\
	deque->array = (type*)malloc(STARTING_SIZE * sizeof(type));				\
	deque->length = 0;									\
	deque->capacity = STARTING_SIZE;							\
	deque->ignored = 0;									\
	deque->compare = (*comp);								\
	deque->at = &Deque_##type##_at;								\
	deque->size = &Deque_##type##_size;							\
	deque->empty = &Deque_##type##_empty;							\
	deque->clear = &Deque_##type##_clear;							\
	deque->dtor = &Deque_##type##_dtor;							\
	deque->front = &Deque_##type##_front;							\
	deque->back = &Deque_##type##_back;							\
	deque->push_front = &Deque_##type##_push_front;						\
	deque->push_back = &Deque_##type##_push_back;						\
	deque->pop_front = &Deque_##type##_pop_front;						\
	deque->pop_back = &Deque_##type##_pop_back;						\
	deque->begin = &Deque_##type##_begin;							\
	deque->end = &Deque_##type##_end;							\
	deque->sort = &Deque_##type##_sort;							\
}												
#endif

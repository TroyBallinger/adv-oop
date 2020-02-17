#ifndef CS540_MAP_HPP
#define CS540_MAP_HPP
#include <cstddef>
#include <utility>
#include <vector>
#include <cstdlib>
#include <stdexcept>

#define INDEXING 0

namespace cs540 {
	template <typename Key_T, typename Mapped_T> 
	class Map {
		private:
			/* Define skip list */
			struct Skip_Node {
				Skip_Node() : sentinel(1), next(1, nullptr), prev(1, nullptr), insertion_tracker(0) {}	/* Sentinel constructor */
				Skip_Node(int level) : sentinel(0), next(level + 1, nullptr), prev(level + 1, nullptr), insertion_tracker(0) {}	/* Data node constructor */
				virtual ~Skip_Node() {} /* Need 1 virtual function to make polymorphic */
				bool sentinel;
				std::vector<Skip_Node*> next;
				std::vector<Skip_Node*> prev;
				size_t insertion_tracker; /* Tracks how many nodes inserted after this node (INDEXING = 1) */
			};
			struct Data_Skip_Node : Skip_Node {	/* Data (non-sentinel) nodes, notice copy constructor in data() */
				Data_Skip_Node(Key_T k, Mapped_T v, int level) : Skip_Node(level), data(k, Mapped_T(v)) {}
				std::pair<const Key_T, Mapped_T> data;
			};
			struct Skip_List {
				Skip_List() : length(0) {	/* Empty constructor */
					head = new Skip_Node();
					tail = new Skip_Node();
					head->next[0] = tail;
					tail->prev[0] = head;
				}
				~Skip_List() {
					clear();
					delete head;
					delete tail;
				}
				Skip_List& operator=(const Skip_List& m) {
					clear();
					Skip_Node* curr = m.head->next[0];
					while (curr != m.tail) {
						insert(dynamic_cast<Data_Skip_Node*>(curr)->data.first, dynamic_cast<Data_Skip_Node*>(curr)->data.second);
						curr = curr->next[0];
					}
					return *this;
				}
				void clear() {
					Skip_Node* curr = head->next[0];
					while(curr != tail) {
						Skip_Node* next = curr->next[0];
						delete curr;
						curr = next;
					}
					head->next = std::vector<Skip_Node*>(1, tail);
					head->prev = std::vector<Skip_Node*>(1, nullptr);
					tail->next = std::vector<Skip_Node*>(1, nullptr);
					tail->prev = std::vector<Skip_Node*>(1, head);
					length = 0;
				}
				Skip_Node* find(Key_T key) const {
					Skip_Node* curr = head;
					/* Starting at the highest level, skip thru forward pointers */
					for (int i = head->next.size() - 1; 0 <= i; i--) {
						/* While current key < search key, go right (until sentinel) */
						while (curr->next[i]->sentinel == 0 && dynamic_cast<Data_Skip_Node*>(curr->next[i])->data.first < key) {
							curr = curr->next[i];
						}
						if (curr->next[i]->sentinel == 0 && dynamic_cast<Data_Skip_Node*>(curr->next[i])->data.first == key) { 
							return curr->next[i]; /* Found */
						}
					}
					return tail;
				}
				Data_Skip_Node* insert(Key_T key, Mapped_T value) { /* Does not check if key already exists */
					Skip_Node* curr = head;
					std::vector<Skip_Node*> node_path; /* Nodes to (potentially) update next[] values on */
					int level = 0, adjusted_level = 0;
					/* Traverse until the end, or until a larger key is found */
					for (int i = head->next.size() - 1; 0 <= i; i--) { /* head->next.size() - 1 = the highest level */
						while (curr->next[i]->sentinel == 0 && dynamic_cast<Data_Skip_Node*>(curr->next[i])->data.first < key) {
							curr = curr->next[i];
						}
						node_path.emplace_back(curr); /* Down a level, track last node on i level */
					}
					/* Remember the node that will come after inserted_node on level 0 */
					curr = curr->next[0];
					/* Generate a random level */
					while(rand() % 2 == 0) {
						level++;
					}
					Data_Skip_Node* inserted_node = new Data_Skip_Node(key, value, level);
					length++;
					adjusted_level = level;
					/* Update sentinels if a new max level exists */
					for (int i = head->next.size(); i <= level; i++) {
						inserted_node->prev[i] = head; 
						head->next.emplace_back(inserted_node);
						head->prev.emplace_back(nullptr);
						tail->next.emplace_back(nullptr);
						tail->prev.emplace_back(inserted_node);
						inserted_node->next[i] = tail; 
						adjusted_level--;
					}
					/* Set previous node's nexts to point to inserted_node, but only up to its own level - created levels */
					for (int i = 0; i <= adjusted_level && i < node_path.size(); i++) {
						node_path[(node_path.size() - 1) - i]->next[i] = inserted_node;
						inserted_node->prev[i] = node_path[(node_path.size() - 1) - i];
					}
					/* Set nexts of new node and prevs of subsequent nodes */
					int i = 0;
					while (i <= adjusted_level) {
						/* Fill up to new node's level, or subsequent node's level, whichever is larger */
						while (i < curr->next.size() && i <= adjusted_level) {
							inserted_node->next[i] = curr;
							curr->prev[i] = inserted_node;
							i++;
						}
						curr = curr->next[0];
					}
					#if INDEXING
					curr = head;
					head->insertion_tracker++;
					for (int i = head->next.size() - 1; 0 <= i; i--) {
						while (curr->next[i]->sentinel == 0 && dynamic_cast<Data_Skip_Node*>(curr->next[i])->data.first <= key) {
							if (dynamic_cast<Data_Skip_Node*>(curr->next[i])->data.first == key) { i = -1; break;}
							curr->insertion_tracker++;
							curr = curr->next[i];
						}
					}
					#endif
					return inserted_node;
				}
				void erase(Skip_Node* er) {
					if (er->sentinel == 0) {
						#if INDEXING
						Skip_Node* curr = head;
						head->insertion_tracker--;
						for (int i = head->next.size() - 1; 0 <= i; i--) {
						while (curr->next[i]->sentinel == 0 && dynamic_cast<Data_Skip_Node*>(curr->next[i])->data.first <= dynamic_cast<Data_Skip_Node*>(er)->data.first) {
							if (dynamic_cast<Data_Skip_Node*>(curr->next[i])->data.first == dynamic_cast<Data_Skip_Node*>(er)->data.first) { i = -1; break;}
								curr->insertion_tracker--;
								curr = curr->next[i];
							}
						}
						#endif
						for (int i = 0; i < er->next.size(); i++) {
							er->prev[i]->next[i] = er->next[i];
							er->next[i]->prev[i] = er->prev[i];
						}
						length--;
						delete er;
					}
				}
				#if INDEXING
				Data_Skip_Node* index(size_t index) {
					if (index < 0 || index > length) return nullptr;
					Skip_Node* curr = head;
					index++;
					int i = head->next.size() - 1;
					while (0 < i) {
						if (curr->next[i]->sentinel == 0 && (curr->insertion_tracker - curr->next[i]->insertion_tracker) <= index) {
							index -= (curr->insertion_tracker - curr->next[i]->insertion_tracker);
							curr = curr->next[i];
						} else {
							i--;
						}
					}
					while (index != 0) {
						curr = curr->next[0];
						index--;
					}
					return dynamic_cast<Data_Skip_Node*>(curr);
				}
				Data_Skip_Node* index_ON(size_t index) {
					if (index < 0 || index > length) return nullptr;
					Skip_Node* curr = head;
					while (index >= 0) {
						curr = curr->next[0];
						index--;
					}
					return dynamic_cast<Data_Skip_Node*>(curr);
				}
				#endif
				Skip_Node* head;
				Skip_Node* tail;
				size_t length;
			};
			/* End skip list definition */
			Skip_List list;
		public:
			/* Define map public functions */
			Map() : list() {}
			Map(const Map& m) {
				list = m.list;
			}
			Map& operator=(const Map& m) {
				if (this != &m) {
					list = m.list;
				}
				return *this;
			}
			Map(std::initializer_list<std::pair<const Key_T, Mapped_T>> items) : list() {
				for (auto i : items) {
					list.insert(i.first, i.second);
				}
			}
			~Map() {}
			size_t size() const {
				return list.length;						
			}
			bool empty() const {
				return (list.length == 0);
			}
			/* Define iterators */
			class Iterator {
				public:
					Iterator() = delete;
					Iterator(const Iterator& it) = default;
					~Iterator() = default;
					Iterator& operator=(const Iterator &) = default;
					Iterator &operator++() {
						pointer = pointer->next[0];
						return *this;
					}
					Iterator operator++(int) {
						Iterator temp(*this);
						pointer = pointer->next[0];
						return temp;
					}
					Iterator &operator--() {
						pointer = pointer->prev[0];
						return *this;
					}
					Iterator operator--(int) {
						Iterator temp(*this);
						pointer = pointer->prev[0];
						return temp;
					}
					std::pair<const Key_T, Mapped_T> &operator*() const {
						return dynamic_cast<Data_Skip_Node*>(pointer)->data;
					}
					std::pair<const Key_T, Mapped_T> *operator->() const {
						return &(dynamic_cast<Data_Skip_Node*>(pointer)->data);
					}
					friend bool operator==(const Iterator & it1, const Iterator & it2) { /* Was too much trouble to make free functions */
						return (it1.pointer == it2.pointer);
					}
					friend bool operator!=(const Iterator & it1, const Iterator & it2) {
						return !(it1 == it2);
					}
				protected:
					Skip_Node* pointer;
					Iterator(Skip_Node* node) : pointer(node) {}
					friend class Map;
			};
			class ConstIterator : public Iterator {
				public:
					ConstIterator() = delete;
					ConstIterator(const Iterator& it) : Iterator(it) {}
					const std::pair<const Key_T, Mapped_T> &operator*() const {
						return dynamic_cast<Data_Skip_Node*>(this->pointer)->data;
					}
					const std::pair<const Key_T, Mapped_T> *operator->() const {
						return &(dynamic_cast<Data_Skip_Node*>(this->pointer)->data);
					}
				private:
					ConstIterator(Skip_Node* node) : Iterator(node) {}
					friend class Map;
			};
			class ReverseIterator : public Iterator {
				public:
					ReverseIterator() = delete;
					ReverseIterator(const ReverseIterator& it) = default;
					~ReverseIterator() = default;
					ReverseIterator& operator=(const ReverseIterator &) = default;
					ReverseIterator &operator++() {
						this->pointer = this->pointer->prev[0];
						return *this;
					}
					ReverseIterator operator++(int) {
						ReverseIterator temp(*this);
						this->pointer = this->pointer->prev[0];
						return temp;
					}
					ReverseIterator &operator--() {
						this->pointer = this->pointer->next[0];
						return *this;
					}
					ReverseIterator operator--(int) {
						ReverseIterator temp(*this);
						this->pointer = this->pointer->next[0];
						return temp;
					}
				private:
					ReverseIterator(Skip_Node* node) : Iterator(node) {}
					friend class Map;
			};
			/* End iterators definition */
			Iterator begin() {
				return Iterator(list.head->next[0]);
			}
			Iterator end() {
				return Iterator(list.tail);
			}
			ConstIterator begin() const {
				return ConstIterator(list.head->next[0]);
			}
			ConstIterator end() const {
				return ConstIterator(list.tail);
			}
			ReverseIterator rbegin() {
				return ReverseIterator(list.tail->prev[0]);
			}
			ReverseIterator rend() {
				return ReverseIterator(list.head);
			}
			Iterator find(const Key_T & k) {
				return Iterator(list.find(k));
			}
			ConstIterator find(const Key_T & k) const {
				return ConstIterator(list.find(k));
			}
			Mapped_T &at(const Key_T & k) {
				Skip_Node* node = list.find(k);
				if (node == list.tail) {
					throw std::out_of_range("Element not in range");
				}
				return dynamic_cast<Data_Skip_Node*>(node)->data.second;
			}
			const Mapped_T &at(const Key_T & k) const {
				Skip_Node* node = list.find(k);
				if (node == list.tail) {
					throw std::out_of_range("Element not in range");
				}
				return dynamic_cast<Data_Skip_Node*>(node)->data.second;
			}
			Mapped_T &operator[](const Key_T & k) {
				Iterator it = insert(std::make_pair(k, Mapped_T())).first;
				return (*it).second;
			}
			std::pair<Iterator, bool> insert(const std::pair<const Key_T, Mapped_T> & p) {
				Skip_Node* ptr = list.find(p.first);
				bool not_found = (ptr == list.tail);
				if (not_found) {
					ptr = list.insert(p.first, p.second);
				}
				return std::pair<Iterator, bool>(Iterator(ptr), not_found);
			}
			template <typename IT_T>
			void insert(IT_T range_beg, IT_T range_end) {
				while (range_beg != range_end) {
					insert((*range_beg).first, (*range_beg).second);
					range_beg++;
				}
			}
			void erase(Iterator pos) {
				list.erase(pos.pointer);
			}
			void erase(const Key_T & k) {
				list.erase(list.find(k));
			}
			void clear() {
				list.clear();
			}
			#if INDEXING
			Mapped_T &index(size_t ind) {
				return (list.index(ind)->data.second);
			}
			Mapped_T &index_ON(size_t ind) {
				return (list.index_ON(ind)->data.second);
			}
			#endif
			/* End map public functions definitions */
	};
	/* Compare functions */
	template <typename Key_T, typename Mapped_T> 
	bool operator==(const Map<Key_T, Mapped_T> & map1, const Map<Key_T, Mapped_T> & map2) {
		typename Map<Key_T, Mapped_T>::Iterator i1 = map1.begin(), i2 = map2.begin();
		while (i1 != map1.end()) {
			if (i2 == map2.end() || !((*i1).first == (*i2).first) || !((*i1).second == (*i2).second)) {
				return false;
			}
			i1++;
			i2++;
		}
		return (i2 == map2.end()) ? true : false;
	}
	template <typename Key_T, typename Mapped_T> 
	bool operator!=(const Map<Key_T, Mapped_T> & map1, const Map<Key_T, Mapped_T> & map2) {
		return !(map1 == map2);
	}
	template <typename Key_T, typename Mapped_T> 
	bool operator<(const Map<Key_T, Mapped_T> & map1, const Map<Key_T, Mapped_T> & map2) {
		typename Map<Key_T, Mapped_T>::Iterator i1 = map1.begin(), i2 = map2.begin();
		while (i1 != map1.end() && i2 != map2.end() && (*i1).first < (*i2).first && (*i1).second < (*i2).second) {
			i1++;
			i2++;
		}
		return (i1 == map1.end());
	}
}

#endif

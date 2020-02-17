#ifndef SHAREDPTR_HPP
#define SHAREDPTR_HPP
#include <mutex>

namespace cs540 {
	std::mutex ref_counter_mutex;

	// Holds the original type of an object and, by extention, its destructor
	struct DeleterBase {
		virtual ~DeleterBase() {}
	};
	template <typename U>
	struct Deleter : DeleterBase {
		Deleter(U* obj) : object(obj) {}
		~Deleter() {
			delete object;
		}
		U* object;
	};

	// Tracks how many pointers an object has and holds its deleter
	class ObjectTracker {
		public:
			ObjectTracker(DeleterBase* d) : deleter(d), reference_count(1) {}
			void increment_count() {
				std::lock_guard<std::mutex> lock(ref_counter_mutex);
				reference_count++;
			}
			bool decrement_count() {
				std::lock_guard<std::mutex> lock(ref_counter_mutex);
				reference_count--;
				return (reference_count <= 0);
			}
			~ObjectTracker() {
				delete deleter;
			}
		private:
			DeleterBase *deleter;
			int reference_count;
	};

	// A thread-safe, exception-safe, reference-counting smart pointer
	template <typename T>
	struct SharedPtr {
		SharedPtr() : pointer(nullptr), obj_tracker(nullptr) {}
		template <typename U>
		explicit SharedPtr(U* u) : pointer(u), obj_tracker(new ObjectTracker(new Deleter<U>(u))) {}
		SharedPtr(const SharedPtr &p) {
			if (p.obj_tracker != nullptr)
				p.obj_tracker->increment_count();
			obj_tracker = p.obj_tracker;
			pointer = p.pointer;
		}
		template <typename U>
		SharedPtr(const SharedPtr<U> &p) {
			if (p.obj_tracker != nullptr)
				p.obj_tracker->increment_count();
			obj_tracker = p.obj_tracker;
			pointer = p.pointer;
		}
		SharedPtr(SharedPtr &&p) {
			obj_tracker = p.obj_tracker;
			pointer = p.pointer;
			p.obj_tracker = nullptr;
			p.pointer = nullptr;
		}
		template <typename U>
		SharedPtr(SharedPtr<U> &&p) {
			obj_tracker = p.obj_tracker;
			pointer = p.pointer;
			p.obj_tracker = nullptr;
			p.pointer = nullptr;
		}
		SharedPtr& operator=(const SharedPtr &p) {
			if (pointer != p.pointer) {
				reset();
				if (p.obj_tracker != nullptr) 
					p.obj_tracker->increment_count();
				obj_tracker = p.obj_tracker;
				pointer = p.pointer;
			}
			return *this;
		}
		template <typename U>
		SharedPtr& operator=(const SharedPtr<U> &p) {
			if (pointer != p.pointer)  {
				reset();
				if (p.obj_tracker != nullptr)
					p.obj_tracker->increment_count();
				obj_tracker = p.obj_tracker;
				pointer = p.pointer;
			}
			return *this;
		}
		SharedPtr& operator=(SharedPtr &&p) {
			reset();
			obj_tracker = p.obj_tracker;
			pointer = p.pointer;
			p.obj_tracker = nullptr;
			p.pointer = nullptr;
			return *this;
		}
		template <typename U>
		SharedPtr& operator=(SharedPtr<U> &&p) {
			reset();
			obj_tracker = p.obj_tracker;
			pointer = p.pointer;
			p.obj_tracker = nullptr;
			p.pointer = nullptr;
			return *this;
		}
		~SharedPtr() {
			reset();
		}
		void reset() {;
			if (obj_tracker != nullptr) {
				if (obj_tracker->decrement_count()) {
					delete obj_tracker;
				}
				obj_tracker = nullptr;
			}
			pointer = nullptr;
		}
		template <typename U>
		void reset(U* p) {
			if (obj_tracker != nullptr && obj_tracker->decrement_count()) {
				delete obj_tracker;
			}
			obj_tracker = new ObjectTracker(new Deleter<U>(p));
			pointer = p;
		}
		T* get() const {
			return pointer;
		}
		T &operator*() const {
			return *(pointer);
		}
		T *operator->() const {
			return pointer;
		}
		explicit operator bool() const {
			return (pointer != nullptr);
		}

		T* pointer;
		ObjectTracker* obj_tracker;
	};
	template <typename T1, typename T2>
	bool operator==(const SharedPtr<T1> &sp1, const SharedPtr<T2> &sp2) {
		return (sp1.pointer == sp2.pointer);
	}
	template <typename T>
	bool operator==(const SharedPtr<T> &sp, std::nullptr_t) {
		return (sp.pointer == nullptr);
	}
	template <typename T>
	bool operator==(std::nullptr_t, const SharedPtr<T> &sp) {
		return (sp.pointer == nullptr);
	}
	template <typename T1, typename T2>
	bool operator!=(const SharedPtr<T1> &sp1, const SharedPtr<T2> &sp2) {
		return !(sp1 == sp2);
	}
	template <typename T>
	bool operator!=(const SharedPtr<T> &sp, std::nullptr_t) {
		return !(sp == nullptr);
	}
	template <typename T>
	bool operator!=(std::nullptr_t, const SharedPtr<T> &sp) {
		return !(sp == nullptr);
	}
	template <typename T, typename U>
	SharedPtr<T> static_pointer_cast(const SharedPtr<U> &sp) {
		SharedPtr<T> sp2;
		if (sp.obj_tracker != nullptr) {
			sp.obj_tracker->increment_count();
			sp2.obj_tracker = sp.obj_tracker;
		}
		sp2.pointer = static_cast<T*>(sp.pointer);
		return sp2;
	}
	template <typename T, typename U>
	SharedPtr<T> dynamic_pointer_cast(const SharedPtr<U> &sp) {
		SharedPtr<T> sp2;
		if (sp.obj_tracker != nullptr) {
			sp.obj_tracker->increment_count();
			sp2.obj_tracker = sp.obj_tracker;
		}
		sp2.pointer = dynamic_cast<T*>(sp.pointer);
		return sp2;
	}
}

#endif

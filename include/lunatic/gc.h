#pragma once
#include "value.h"
#include <atomic>
namespace lunatic {
	class GC;
	class GCObjectRef;
	class GCObject {
		friend class GC;
		friend class GCObjectRef;
		bool marked = false;
		GCObjectRef* ref = nullptr;
		virtual size_t nBytes()const = 0;
	protected:
		std::atomic<int64_t> refCount;
	public:
		GCObject() :ref(nullptr), refCount(0) {}
		GCObject(const GCObject& o) :refCount(0), ref(nullptr) {}
		virtual void markReferences(GC*)const = 0;

		virtual ~GCObject() = default;
		void retain() {
			refCount.fetch_add(1);
		}
		void release();
		int64_t getRefCount() {
			return refCount;
		}
	};

	class GCObjectRef {
		friend class GC;
		friend class GCObject;
		GCObject* object = nullptr;
	public:
		GCObjectRef(GCObject* o) :object(o) {
			object->ref = this;
		}
		bool isValid()const {
			return object != nullptr;
		}

	};

	class GC {
		std::list<GCObjectRef> values;
		std::set<GCObject*> visited;
		void unmarkAll();
		mutable size_t allocatedBytes = 0;
		void free(GCObject*);
	public:
		void mark(GCObject*);
		template<class T, class...Args>
		T* alloc(Args&& ...args) {
			T* v = new T(args...);
			GCObject* object = static_cast<GCObject*>(v);
			values.emplace_back(object);
			return v;
		}
		void prepareForCollect();
		void mark(const Value&);
		void sweep();
		size_t getMemoryUsage(bool forceRescan = false)const {
			if (forceRescan) {
				allocatedBytes = 0;
				for (auto i : values) {
					if (i.isValid())
						allocatedBytes += i.object->nBytes();
				}
			}
			return allocatedBytes;
		}
	};
}
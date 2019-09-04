#pragma once
#include <lunatic/value.h>
#include <lunatic/atomic>
namespace lunatic {
	class GC;

	class GCObject {
		friend class GC;

		bool marked = false;

		virtual size_t nBytes()const = 0;

	public:
		virtual void markReferences(GC*)const = 0;
		
		virtual ~GCObject() = default;
	};



	class GC {
		std::list<GCObject*> values;
		std::set<GCObject*> visited;
		void unmarkAll();
		mutable size_t allocatedBytes = 0;
		void free(GCObject*);
	public:
		void mark(GCObject*);
		template<class T,class...Args>
		T* alloc(Args&&...args) {
			T* v = new T(args...);
			GCObject* object = static_cast<GCObject*>(v);
			values.emplace_back(object);
			return v;
		}
		void prepareForCollect();
		void mark(const Value&);
		void sweep();
		size_t getMemoryUsage(bool forceRescan = false)const {
			if(forceRescan){
				allocatedBytes = 0;
				for(auto i: values){
						allocatedBytes += i->nBytes();
				}
			}
			return allocatedBytes;
		}
	};
}
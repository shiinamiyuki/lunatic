#pragma once
#include "value.h"
namespace lunatic {
	class GC;
	class GCObject {
		friend class GC;
		bool marked = false;
	public:
		virtual void markReferences(GC*)const = 0;
		virtual ~GCObject() = default;
	};

	class GC {
		std::list<GCObject*> values;
	public:
		void mark(GCObject*);
		template<class T,class...Args>
		T* alloc(Args&&...args) {
			T* v = new T(args...);
			GCObject* object = static_cast<GCObject*>(v);
			values.emplace_back(object);
			return v;
		}
	};
}
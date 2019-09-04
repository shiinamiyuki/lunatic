#include "gc.h"
#include "lstring.h"
#include "table.h"
#include "closure.h"
#include "lformat.h"
namespace lunatic {
	void GC::mark(GCObject* object) {
		if (!object)return;
		if (visited.find(object) != visited.end())return;
		visited.insert(object);
		object->marked = true;
		object->markReferences(this);
	}
	void GC::unmarkAll() {
		for (auto& i : values) {
			i->marked = false;
		}
	}
	void GC::mark(const Value& v) {
		if (v.isString()) {
			mark(v.getString());
		}
		else if (v.isTable()) {
			mark(v.getTable());
		}
		else if (v.isClosure()) {
			mark(v.getClosure());
		}
		else if (v.isNativeFunction()) {
			mark(v.getNativeFunction());
		}
		mark(v.getMetatable());
	}
	void GC::prepareForCollect() {
		visited.clear();
		allocatedBytes = 0;
	}
	void GC::free(GCObject* o) {
		delete o;

	}
	void GC::sweep() {
		for (auto iter = values.begin(); iter != values.end();) {
			auto o = *iter;
			if (!o->marked) {
				iter = values.erase(iter);
				//println("Delected {}", (size_t)o);
				//std::cout << "deleted" << (size_t)o <<std::endl;
				delete o;
			}
			else {
				++iter;
				o->marked = false;
				allocatedBytes += o->nBytes();
			}
		}
	}

}
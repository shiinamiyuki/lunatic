#include "gc.h"
#include "lstring.h"
#include "table.h"
#include "closure.h"
#include "format.h"
namespace lunatic {
	void GC::mark(GCObject* object) {
		if(visited.find(object)!=visited.end())return;
		visited.insert(object);
		object->marked = true;
		object->markReferences(this);
	}
	void GC::unmarkAll(){
		for(auto & i:values){
			i.object->marked = false;
		}
	}
	void GC::mark(const Value & v){
		if(v.isString()){
			mark(v.getString());
		}else if (v.isTable()){
			mark(v.getTable());
		}else if(v.isClosure()){
			mark(v.getClosure());
		}
	}
	void GC::prepareForCollect(){
		visited.clear();
		allocatedBytes = 0;
	}
	void GC::free(GCObject * o){
		delete o;
		
	}
	void GC::sweep(){
		for(auto iter = values.begin();iter!=values.end();){
			auto o = *iter;
			if(o.isValid() && !o.object->marked){
				iter = values.erase(iter);
				//println("Delected {}", (size_t)o);
				//std::cout << "deleted" << (size_t)o <<std::endl;
				delete o.object;
			}else if(!o.isValid()){
				iter = values.erase(iter);				
			}else{
				++iter;
				o.object->marked = false;
				allocatedBytes += o.object->nBytes();
			}
		}
	}
	void GCObject::release(){
		refCount.fetch_sub(1);
		if(refCount == 0){
			if(ref){
				ref->object = nullptr;
				delete this;
			}
		}
	}
}
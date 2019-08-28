#include "gc.h"
#include "lstring.h"
#include "table.h"
#include "closure.h"
namespace lunatic {
	void GC::mark(GCObject* object) {
		if(visited.find(object)!=visited.end())return;
		visited.insert(object);
		object->marked = true;
		object->markReferences(this);
	}
	void GC::unmarkAll(){
		for(auto & i:values){
			i->marked = false;
		}
	}
	void GC::mark(Value & v){
		if(v.isString()){
			mark(v.getString());
		}else if (v.isTable()){
			mark(v.getTable());
		}else if(v.isClosure()){
			mark(v.getTable());
		}
	}
	void GC::prepareForCollect(){
		visited.clear();
		unmarkAll();
	}
}
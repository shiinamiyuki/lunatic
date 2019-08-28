#include "gc.h"

namespace lunatic {
	void GC::mark(GCObject* object) {
		object->marked = true;
	}
}
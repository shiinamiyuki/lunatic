#include "gc.h"
#include "closure.h"

#include "value.h"

namespace lunatic {
	class UpValue;
	Closure::Closure(int a, int c) {
		addr = a;
		argCount = c;
	}

	void Closure::setUpvalue(UpValue* up) {

	}
}
#include <lunatic/gc.h>
#include <lunatic/closure.h>

#include <lunatic/value.h>
#include <lunatic/upvalue.h>
#include <lunatic/vm.h>

namespace lunatic {
	class UpValue;
	Closure::Closure(int a, int c) {
		addr = a;
		argCount = c;
	}

	void Closure::setUpvalue(UpValue* up) {
		upValue = up;
	}

	void  Closure::markReferences(GC* gc)const {
		if (upValue)
			gc->mark(upValue);
		if (parent)
			gc->mark(parent);
	}
	void Closure::call(const CallContext&ctx) {
		ctx.vm->call(this, ctx.nArgs);
	}
}
#include "gc.h"
#include "closure.h"

#include "value.h"
#include "upvalue.h"

namespace lunatic {
    class UpValue;
    Closure::Closure(int a, int c) {
        addr = a;
        argCount = c;
    }

    void Closure::setUpvalue(UpValue *up) {

    }
#if 1
    Value Closure::get(int i) {
        return (*upvalue.get<UpValue>())[i];
    }

    void Closure::set(int i, const Value &v) {

        (*upvalue.get<UpValue>())[i] = v;
    }
#endif
}

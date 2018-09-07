#ifndef CLOSURE_H
#define CLOSURE_H

#include "lunatic.h"

namespace lunatic {
    class GCPtr;
    class Value;
    class UpValue;
    class Closure {
        int addr;
        int argCount;
        GCPtr upvalue;
      //  std::unordered_map<unsigned int, Value> upValue;
    public:
        Closure(int a, int c);

        inline int getAddress() const { return addr; }

        inline int getArgCount() const { return argCount; }

        inline void setArgCount(int i) { argCount = i; }
        void setUpvalue(UpValue * up);
        Value get(int);
        void set(int,const Value&);
      //  inline Value& getUpValue(unsigned int i)const{return upValue[i];}
     //   inline void setUpValue(unsigned int i, const Value &v){upValue[i] = v;}

    };
}
#endif // CLOSURE_H

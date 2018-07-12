#ifndef CLOSURE_H
#define CLOSURE_H
#include "lunatic.h"
namespace lunatic{
class Closure
{
     int addr;
     int argCount;
public:
    Closure(int a,int c);
    inline int getAddress()const{return addr;}
    inline int getArgCount()const{return argCount;}
    inline void setArgCount(int i){argCount = i;}
};
}
#endif // CLOSURE_H

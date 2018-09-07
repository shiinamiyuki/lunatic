//
// Created by xiaoc on 2018/9/7.
//

#ifndef LUNATIC_UPVALUE_H
#define LUNATIC_UPVALUE_H

#include "lunatic.h"
#include "value.h"
namespace lunatic {
    class Value;

    class UpValue : public std::unordered_map<int, Value> {
    public:
        UpValue();
    };
}
#endif //LUNATIC_UPVALUE_H

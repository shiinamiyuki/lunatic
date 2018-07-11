/*
 * lib.cc
 *
 *  Created on: 2018Äê6ÔÂ30ÈÕ
 *      Author: xiaoc
 */

#include "lib.h"
#include "value.h"

namespace lunatic{
void ListLength(VM* vm) {
    auto arg = vm->getLocal(0);
    if (arg.isList()) {
        int len = arg.getList().size();
        Value r;
        r.setInt(len);
        vm->storeReturn(0, r);
    } else {
        throw std::runtime_error("is not list object");
    }
}
void print(VM*vm) {
    int cnt = vm->getArgCount();
    for(int i = 0;i<cnt;i++){
        auto v = vm->getLocal(i);
        std::cout << v.str() << " ";
    }
    std::cout <<std::endl;
}
void ListAppend(VM* vm) {
    auto arg = vm->getLocal(0);
    if (arg.isList()) {
        auto v = vm->getLocal(1);
        arg.getList().push_back(v);
        vm->storeReturn(0, arg);
    } else {
        throw std::runtime_error("is not list object");
    }
}
void StringtoList(VM* vm) {
    auto arg = vm->getLocal(0);
    if (arg.isString()) {
        List * vec = new List();
        for (auto i : arg.getString()) {
            Value ch;
            ch.setInt(i);
            vec->push_back(ch);
        }
        Value v;
        v.setList(vec);
        vm->storeReturn(0, v);
    } else {
        throw std::runtime_error("is not string object");
    }
}

void ListtoString(VM* vm) {
    auto arg = vm->getLocal(0);
    if (arg.isList()) {
        GCPtr str;
        str.reset(new std::string());
        for (auto i : arg.getList()) {
            char c = i.getInt();
            std::string s = "";
            s += c;
            str.get<std::string>()->append(s);
        }
        Value v;
        v.setString(str);
        vm->storeReturn(0, v);
    } else {
        throw std::runtime_error("is not list object");
    }
}

}



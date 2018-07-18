/*
 * lib.cc
 *
 *  Created on: 2018Äê6ÔÂ30ÈÕ
 *      Author: xiaoc
 */

#include "lib.h"
#include "value.h"
#include "table.h"
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
void tonumber(VM *vm){
    auto x = vm->getLocal(0);
    Value ret;
    if(x.isString()){
        std::istringstream s(x.getString());
        double d;
        s>>d;
        ret.setFloat(d);
    }
    vm->storeReturn(0, ret);
}
void tostring(VM*vm){
    auto x = vm->getLocal(0);
    Value ret;
    if(x.isInt()){
        std::ostringstream s;
        s << x.getInt();
        GCPtr p;
        p.reset(new std::string(s.str()));
        ret.setString(p);
    }else if(x.isFloat()){
        std::ostringstream s;
        s << x.getFloat();
        GCPtr p;
        p.reset(new std::string(s.str()));
        ret.setString(p);
    }
    vm->storeReturn(0, ret);
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
        Table * tab = new Table();
        int idx = 1;
        for (auto i : arg.getString()) {
            Value ch;
            ch.setInt(i);
            tab->set(idx,ch);
            idx++;
        }
        Value v;
        v.setTable(tab);
        vm->storeReturn(0, v);
    } else {
        throw std::runtime_error("is not string object");
    }
}

void ListtoString(VM* vm) {
    auto arg = vm->getLocal(0);
    if (arg.isTable()) {
        GCPtr str;
        str.reset(new std::string());
        int idx =0 ;
        for (auto i : arg.getTable().getList()) {
            if(idx++ ==0)continue;
            char c = i.getInt();
            std::string s = "";
            s += c;
            str.get<std::string>()->append(s);
        }
        Value v;
        v.setString(str);
        vm->storeReturn(0, v);
    } else {
        throw std::runtime_error("is not table object");
    }
}

void TableLib::clone(VM *vm)
{
    auto arg = vm->getLocal(0);
    Value v;
    v.setTable(new Table(arg.getTable()));
    vm->storeReturn(0,v);
}

}



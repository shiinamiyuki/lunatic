/*
 * lib.cc
 *
 *  Created on: 2018��6��30��
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
    arg.checkTable();
    Value v;
    v.setTable(new Table(arg.getTable()));
    vm->storeReturn(0,v);
}

void setmetatable(VM *vm)
{
    auto arg1 = vm->getLocal(0);
    auto arg2 = vm->getLocal(1);
    arg1.checkTable();
    arg2.checkTable();
    arg1.setMetaTable(arg2);
}
void getmetatable(VM *vm){
    auto arg1 = vm->getLocal(0);
    arg1.checkTable();
    Value v;
    v.setTable(arg1.getMetatable());
    vm->storeReturn(0,v);
}

void _getline(VM *vm)
{
    Value v;
    std::string s;
    std::getline(std::cin,s);
    v.setString(s);
    vm->storeReturn(0,v);
}

    void StringLib::length(VM *vm) {
        auto arg1 = vm->getLocal(0);
        arg1.checkString();
        Value v;
        v.setInt((int)arg1.getString().length());
        vm->storeReturn(0,v);
    }

    void StringLib::sub(VM *vm) {
        int c = vm->getArgCount();
        auto arg1 = vm->getLocal(0);
        arg1.checkString();

    }

    void StringLib::Char(VM *vm) {
        auto arg1 = vm->getLocal(0);
        arg1.checkInt();
        Value v;
        std::string s;
        s+=arg1.getInt();
        v.setString(s);
        vm->storeReturn(0,v);
    }

    void StringLib::byte(VM *vm) {
        auto arg1 = vm->getLocal(0);
        arg1.checkString();
        Value v;
        v.setInt((int)arg1.getString()[0]);
        vm->storeReturn(0,v);
    }

    void FileLib::open(VM *vm) {
        auto arg1 = vm->getLocal(0);
        arg1.checkString();
        auto arg2 = vm->getLocal(1);
        arg2.checkString();
        FILE * f = fopen(arg1.getString().c_str(),arg2.getString().c_str());
        Value file;
        file.setUserData(f);
        Value v;
        v.setTable(new Table());
        v.set("fp",file);
        vm->storeReturn(0,v);
    }

    void FileLib::read(VM *vm) {
        auto arg1 = vm->getLocal(0);
        arg1.checkTable();
        auto file = arg1.get("fp");
        file.checkUserData();
        FILE *f = static_cast<FILE*>(file.getUserData());
        std::string s;
        while(!feof(f)){
            char c = fgetc(f);
            if(!c||c==EOF)break;
            s += c;
        }
        Value v;
        v.setString(s);
        vm->storeReturn(0,v);
    }

    void FileLib::write(VM *vm) {
        auto arg1 = vm->getLocal(0);
        arg1.checkTable();
        auto file = arg1.get("fp");
        file.checkUserData();
        FILE *f = static_cast<FILE*>(file.getUserData());
        auto arg2 = vm->getLocal(1);
        const std::string s= arg2.str();
        fprintf(f,"%s",s.c_str());
    }

    void FileLib::close(VM *vm) {
        auto arg1 = vm->getLocal(0);
        arg1.checkTable();
        auto file = arg1.get("fp");
        file.checkUserData();
        FILE *f = static_cast<FILE*>(file.getUserData());
        fclose(f);
    }

    void GLFWLib::createWindow(VM *vm) {
        auto w = vm->getLocal(0);
        auto h = vm->getLocal(1);
        auto title = vm->getLocal(2);
        w.checkInt();
        h.checkInt();
        title.checkString();
        Value v;
        v.setUserData(::glfwCreateWindow(w.getInt(),h.getInt(),title.getString().c_str(),nullptr,nullptr));
        vm->storeReturn(0,v);
    }

    void GLFWLib::glfwMakeContextCurrent(VM *vm) {


    }
}



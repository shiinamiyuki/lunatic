#include "value.h"
#include "table.h"
#include "lstring.h"
#include "closure.h"
#include "lformat.h"
#include "vm.h"

namespace lunatic {
    size_t Value::Hash::operator()(const Value &v) const {
        switch (v.type) {
            case Value::TTable:
            case Value::TClosure:
            case Value::TUserData:
            case Value::TNativeFunction:
                return std::hash<void *>()(v.getTable());
            case Value::TNil:
                return -1;
            case Value::TString:
                return std::hash<std::string>()(v.getString()->str());
            case Value::TInt:
            case Value::TBool:
                return std::hash<std::int64_t>()(v.asInt);
            case Value::TFloat:
                return std::hash<double>()(v.asFloat);
        }
        return -1;
    }

    const char *printstr(Value::Type type) {
        switch (type) {
            case Value::TTable:
                return "table";
            case Value::TNil:
                return "nil";
            case Value::TString:
                return "string";
            case Value::TClosure:
                return "function";
            case Value::TInt:
                return "int";
            case Value::TFloat:
                return "float";
            case Value::TBool:
                return "bool";
            case Value::TUserData:
                return "userdata";
        }
        return "unkown";
    }

    bool Value::operator==(const Value &rhs) {
        if (rhs.type != type) {
            return false;
        } else if (isArithmetic()) {
            return getFloat() == rhs.getFloat();
        } else {
            return false;
        }
    }

    void Value::add(Value *a, Value *b, Value *c) {
        if (a->isBoolInt() && b->isBoolInt()) {
            c->setInt(a->getInt() + b->getInt());
        } else {
            c->setFloat(a->getFloat() + b->getFloat());
        }
    }

    void Value::sub(Value *a, Value *b, Value *c) {
        if (a->isBoolInt() && b->isBoolInt()) {
            c->setInt(a->getInt() - b->getInt());
        } else {
            c->setFloat(a->getFloat() - b->getFloat());
        }
    }

    void Value::mul(Value *a, Value *b, Value *c) {
        if (a->isBoolInt() && b->isBoolInt()) {
            c->setInt(a->getInt() * b->getInt());
        } else {
            c->setFloat(a->getFloat() * b->getFloat());
        }
    }

    void Value::mod(Value *a, Value *b, Value *c) {
        c->setInt(a->getInt() % b->getInt());
    }

    void Value::div(Value *a, Value *b, Value *c) {
        c->setFloat(a->getFloat() / b->getFloat());
    }

    void Value::logicAnd(Value *a, Value *b, Value *c) {
        if (!a->isTrue())
            *c = *a;
        else
            *c = *b;
    }

    void Value::logicOr(Value *a, Value *b, Value *c) {
        if (a->isTrue())
            *c = *a;
        else
            *c = *b;
    }

    void Value::neg(Value *a, Value *b) {
        if (a->isBoolInt()) {
            b->setInt(-a->getInt());
        } else if (a->isFloat()) {
            b->setFloat(-a->getFloat());
        } else {
            throw RuntimException(format("attempt to perform __neg on {}", a->type));
        }
    }

    void Value::logicNot(Value *a, Value *b) {
        b->setBool(!a->getBool());
    }

    void Value::idiv(Value *a, Value *b, Value *c) {
        if (a->isBoolInt() && b->isBoolInt()) {
            c->setBool(a->getInt() / b->getInt());
        } else {
            c->setBool(a->getFloat() / b->getFloat());
        }
    }

    void Value::lt(Value *a, Value *b, Value *c) {
        if (a->isBoolInt() && b->isBoolInt()) {
            c->setBool(a->getInt() < b->getInt());
        } else {
            c->setBool(a->getFloat() < b->getFloat());
        }
    }

    void Value::gt(Value *a, Value *b, Value *c) {
        if (a->isBoolInt() && b->isBoolInt()) {
            c->setBool(a->getInt() > b->getInt());
        } else {
            c->setBool(a->getFloat() > b->getFloat());
        }
    }

    void Value::le(Value *a, Value *b, Value *c) {
        if (a->isBoolInt() && b->isBoolInt()) {
            c->setBool(a->getInt() <= b->getInt());
        } else {
            c->setBool(a->getFloat() <= b->getFloat());
        }
    }

    void Value::ge(Value *a, Value *b, Value *c) {
        if (a->isBoolInt() && b->isBoolInt()) {
            c->setBool(a->getInt() >= b->getInt());
        } else {
            c->setBool(a->getFloat() >= b->getFloat());
        }
    }

    bool Value::operator==(const Value &rhs) const {
        if (isNil() && rhs.isNil()) {
            return true;
        } else if (isArithmetic() && rhs.isArithmetic()) {
            if (isBoolInt() && rhs.isBoolInt()) {
                return getInt() == rhs.getInt();
            } else {
                return getFloat() == rhs.getFloat();
            }
        } else if (type != rhs.type) {
            return false;
        } else {
            return getTable() == rhs.getTable();
        }
    }

    void Value::eq(Value *a, Value *b, Value *c) {
        c->setBool((*a) == *b);
    }

    void Value::ne(Value *a, Value *b, Value *c) {
        c->setBool(!((*a) == *b));
    }

    Value::Value() : type(TNil) {}

    void Value::setNil() {

        type = TNil;
    }

    void Value::setInt(int64_t val) {

        type = TInt;
        asInt = val;
    }

    void Value::setFloat(double val) {

        type = TFloat;
        asFloat = val;
    }

    void Value::setTable(Table *val) {

        type = TTable;
        asTable = val;

    }

    void Value::setNativeFunction(Callable *callable) {
        type = TNativeFunction;
        asNativeFunction = callable;
    }

    void Value::setClosure(Closure *val) {

        type = TClosure;
        asClosure = val;

    }

    void Value::setBool(bool val) {

        type = TBool;
        asInt = val;
    }

    void Value::setString(String *s) {

        asString = s;
        type = TString;

    }

    void Value::setString(const std::string &) {
        type = TString;
    }

    void Value::checkArithmetic() const {
        if (!isArithmetic())
            throw RuntimException("arithmetic object expected!");
    }

    void Value::checkInt() const {
        if (!isBoolInt())
            throw RuntimException("int object expected!");
    }

    void Value::checkFloat() const {
        if (!isFloat() && !isBoolInt()) {
            throw RuntimException("float object expected!");
        }
    }

    void Value::checkClosure() const {
        if (!isClosure()) {
            throw RuntimException("closure object expected!");
        }
    }

    void Value::checkTable() const {
        if (!isTable()) {
            throw RuntimException("table object expected!");
        }
    }

    void Value::checkString() const {
        if (!isString()) {
            throw RuntimException("string object expected!");
        }
    }

    void Value::checkUserData() const {
        if (!isUserData()) {
            throw RuntimException("user data expected!");
        }
    }

    bool Value::getBool() const {
        if (isBoolInt()) {
            return getInt();
        } else if (isFloat()) {
            return getFloat();
        } else if (type == TNil) {
            return false;
        }
        return asTable;
    }

    std::string Value::dump() const {
        std::ostringstream out;
        if (isBoolInt()) {
            out << getInt();
        } else if (isFloat()) {
            out << getFloat();
        } else if (isTable()) {
            auto &tab = *getTable();
            out << "{ ";
            if (!tab.getList()[0].isNil()) {
                out << "{0:" << tab.getList()[0].str() << std::endl;
            }
            for (auto iter = tab.getList().begin() + 1; iter != tab.getList().end(); iter++) {
                out << iter->dump() << ", ";
            }
            for (auto i : tab.map) {
                out << "[" << i.first.dump() << "]=" << i.second.dump() << ",";
            }
            out << "}";
        } else if (isClosure()) {
            out << "<closure at " << reinterpret_cast<long long>(asClosure) << ">";
        } else if (isNativeFunction()) {
            out << "<closure at " << reinterpret_cast<long long>(getNativeFunction()) << ">";
        } else if (isString()) {
            out << "\"" << getString()->str() << "\"";
        } else if (isNil()) {
            out << "nil";
        } else {
            out << "unsupported type " << (size_t) type << std::endl;
        }
        return out.str();
    }

    std::string Value::str() const {
        std::ostringstream out;
        if (isInt()) {
            out << getInt();
        } else if (isBool()) {
            out << (asInt ? "true" : "false");
        } else if (isFloat()) {
            out << getFloat();
        } else if (isTable()) {
            out << "table " << std::hex << (size_t) getTable() << std::ends;
        } else if (isClosure()) {
            out << "function 0x" << std::hex << (size_t) getTable() << std::ends;
        } else if (isString()) {
            out << getString()->str();
        } else if (isNil()) {
            out << "nil";
        } else {
            out << "unsupported type " << (size_t) type << std::ends;
        }
        return out.str();
    }

    void Value::len(Value *a, Value *b) {
        b->setInt(a->len());
    }

    Value Value::get(const std::string &k, VM *vm) {
        Value v;
        v.setString(vm->alloc<String>(k));
        return getTable()->get(v);
    }

    void Value::set(const std::string &k, const Value &v, VM *vm) {
        Value key;
        key.setString(vm->alloc<String>(k));
        getTable()->set(key, v);
    }

    int Value::len() const {
        if (isArithmetic()) {
            throw RuntimException("attemp to get length of a number value");
        } else if (isTable()) {
            return getTable()->len();
        } else if (isString()) {
            return getString()->str().length();
        } else {
            throw RuntimException(format("attempt to get length of {}", type));
        }
    }

    Value Value::get(Value &k) {
        return getTable()->get(k);
    }

    void Value::set(Value &k, const Value &v) {
        getTable()->set(k, v);
    }

    void Value::setMetaTable(Value *a, Value *b) {
        if (a->isTable()) {
            b->metatable = a->getTable();
        } else {
            throw RuntimException(format("{} expected but found when setting metatable", TTable, a->type));
        }
    }


    void Value::setMetaTable(const Value &v) {
        if (v.isTable()) {
            metatable = v.getTable();
        } else {
            throw RuntimException(format("{} expected but found setting metatable", TTable, v.type));
        }
    }


    Table *Value::getMetatable() const {
        return metatable;
    }

    void Value::clone(GC &gc, Value *a, Value *b) {
        if (a->isTable()) {
            b->setTable(gc.alloc<Table>(*a->getTable()));//TODO: set proto instead of direct copy
            //	b->asObject.get<Table>()->proto = a->asObject;
        }
    }

    int Value::getClosureAddr() const {
        return getClosure()->getAddress();
    }

    void Value::setArgCount(int i) {
        getClosure()->setArgCount(i);
    }

    bool Value::isTrue() {
        if (isBool()) {
            return asInt != 0;
        } else {
            return !isNil();
        }
    }

    void Value::concat(Value *a, Value *b, Value *c, VM *vm) {
        a->checkString();
        b->checkString();
        c->setString(vm->alloc<String>(a->getString()->str() + b->getString()->str()));
    }
}
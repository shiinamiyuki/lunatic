#include "value.h"
#include "table.h"
#include "lstring.h"
#include "closure.h"
#include "format.h"
namespace lunatic {
	const char* printstr(Value::Type type) {
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

	bool Value::operator==(const Value& rhs) {
		if (rhs.type != type) {
			return false;
		}
		else if (isArithmetic()) {
			return getFloat() == rhs.getFloat();
		}
		else {
			return false;
		}
	}

	void Value::add(Value* a, Value* b, Value* c) {
		if (a->isBoolInt() && b->isBoolInt()) {
			c->setInt(a->getInt() + b->getInt());
		}
		else {
			c->setFloat(a->getFloat() + b->getFloat());
		}
	}

	void Value::sub(Value* a, Value* b, Value* c) {
		if (a->isBoolInt() && b->isBoolInt()) {
			c->setInt(a->getInt() - b->getInt());
		}
		else {
			c->setFloat(a->getFloat() - b->getFloat());
		}
	}

	void Value::mul(Value* a, Value* b, Value* c) {
		if (a->isBoolInt() && b->isBoolInt()) {
			c->setInt(a->getInt() * b->getInt());
		}
		else {
			c->setFloat(a->getFloat() * b->getFloat());
		}
	}

	void Value::mod(Value* a, Value* b, Value* c) {
		c->setInt(a->getInt() % b->getInt());
	}

	void Value::div(Value* a, Value* b, Value* c) {
		c->setFloat(a->getFloat() / b->getFloat());
	}

	void Value::logicAnd(Value* a, Value* b, Value* c) {
		if (!a->isTrue())
			* c = *a;
		else
			*c = *b;
	}

	void Value::logicOr(Value* a, Value* b, Value* c) {
		if (a->isTrue())
			* c = *a;
		else
			*c = *b;
	}

	void Value::neg(Value* a, Value* b) {
		if (a->isBoolInt()) {
			b->setInt(-a->getInt());
		}
		else if(a->isFloat()){
			b->setFloat(-a->getFloat());
		}
		else {
			throw RuntimException(format("attempt to perform __neg on {}", a->type));
		}
	}

	void Value::logicNot(Value* a, Value* b) {
		b->setBool(!a->getBool());
	}

	void Value::idiv(Value* a, Value* b, Value* c) {
		if (a->isBoolInt() && b->isBoolInt()) {
			c->setBool(a->getInt() / b->getInt());
		}
		else {
			c->setBool(a->getFloat() / b->getFloat());
		}
	}
	void Value::lt(Value* a, Value* b, Value* c) {
		if (a->isBoolInt() && b->isBoolInt()) {
			c->setBool(a->getInt() < b->getInt());
		}
		else {
			c->setBool(a->getFloat() < b->getFloat());
		}
	}

	void Value::gt(Value* a, Value* b, Value* c) {
		if (a->isBoolInt() && b->isBoolInt()) {
			c->setBool(a->getInt() > b->getInt());
		}
		else {
			c->setBool(a->getFloat() > b->getFloat());
		}
	}

	void Value::le(Value* a, Value* b, Value* c) {
		if (a->isBoolInt() && b->isBoolInt()) {
			c->setBool(a->getInt() <= b->getInt());
		}
		else {
			c->setBool(a->getFloat() <= b->getFloat());
		}
	}

	void Value::ge(Value* a, Value* b, Value* c) {
		if (a->isBoolInt() && b->isBoolInt()) {
			c->setBool(a->getInt() >= b->getInt());
		}
		else {
			c->setBool(a->getFloat() >= b->getFloat());
		}
	}

	void Value::eq(Value* a, Value* b, Value* c) {
		if (a->isNil() && b->isNil()) {
			c->setBool(true);
		}else if (a->isArithmetic() && b->isArithmetic()) {
			if (a->isBoolInt() && b->isBoolInt()) {
				c->setBool(a->getInt() == b->getInt());
			}
			else {
				c->setBool(a->getFloat() == b->getFloat());
			}
		}
		else if (a->type != b->type) {
			c->setBool(false);
		}
		else {
			c->setBool(a->getTable() == b->getTable());
		}
	}

	void Value::ne(Value* a, Value* b, Value* c) {
		if (a->isArithmetic() && b->isArithmetic()) {
			if (a->isBoolInt() && b->isBoolInt()) {
				c->setBool(a->getInt() != b->getInt());
			}
			else {
				c->setBool(a->getFloat() != b->getFloat());
			}
		}
		else if (a->type != b->type) {
			c->setBool(true);
		}
		else {
			c->setBool(a->getTable() != b->getTable());
		}
	}

	Value::Value() :type(TNil) {}

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

	void Value::setTable(Table* val) {

		type = TTable;
		asTable = val;

	}

	void Value::setNativeFunction(Callable* callable) {
		type = TNativeFunction;
		asNativeFunction = callable;
	}
	void Value::setClosure(Closure* val) {

		type = TClosure;
		asClosure = val;

	}

	void Value::setBool(bool val) {

		type = TBool;
		asInt = val;
	}
	void Value::setString(String* s) {

		asString = s;
		type = TString;

	}
	void Value::setString(const std::string&) {
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
		}
		else if (isFloat()) {
			return getFloat();
		}
		else if (type == TNil) {
			return false;
		}
		return asTable;
	}
	std::string Value::dump()const {
		std::ostringstream out;
		if (isBoolInt()) {
			out << getInt();
		}
		else if (isFloat()) {
			out << getFloat();
		}
		else if (isTable()) {
			auto& tab = *getTable();
			out << "{ ";
			if (!tab.getList()[0].isNil()) {
				out << "{0:" << tab.getList()[0].str() << std::endl;
			}
			for (auto iter = tab.getList().begin() + 1; iter != tab.getList().end(); iter++) {
				out << iter->dump() << ", ";
			}
			for (auto i : tab.iMap) {
				out << "[" << i.first << "]=" << i.second.dump() << ",";
			}
			for (auto i : tab.sMap) {
				out << "[" << i.first << "]=" << i.second.dump() << ",";
			}
			out << "}";
		}
		else if (isClosure()) {
			out << "<closure at " << reinterpret_cast<long long>(asClosure) << ">";
		}
		else if (isNativeFunction()) {
			out << "<closure at " << reinterpret_cast<long long>(getNativeFunction()) << ">";
		}
		else if (isString()) {
			out << "\"" << getString()->str() << "\"";
		}
		else if (isNil()) {
			out << "nil";
		}
		else {
			out << "unsupported type " << (size_t)type << std::endl;
		}
		return out.str();
	}
	std::string Value::str() const {
		std::ostringstream out;
		if (isInt()) {
			out << getInt();
		}
		else if (isBool()) {
			out << (asInt ? "true" : "false");
		}
		else if (isFloat()) {
			out << getFloat();
		}
		else if (isTable()) {
			out << "table " << std::hex << (size_t)getTable() << std::endl;
		}
		else if (isClosure()) {
			out << "function 0x" << std::hex << (size_t)getTable() << std::endl;
		}
		else if (isString()) {
			out << getString()->str();
		}
		else if (isNil()) {
			out << "nil";
		}
		else {
			out << "unsupported type " << (size_t)type << std::endl;
		}
		return out.str();
	}
	void Value::len(Value* a, Value* b) {
		b->setInt(a->len());
	}
	Value Value::get(int i) {
		if (isTable()) {
			return getTable()->get(i);
		}
		else {
			throw RuntimException(format("attempt to get item of {}", type));
		}
	}
	int Value::len() const {
		if (isArithmetic()) {
			throw RuntimException("attemp to get length of a number value");
		}
		else if (isTable()) {
			return getTable()->len();
		}
		else if (isString()) {
			return getString()->str().length();
		}
		else {
			throw RuntimException(format("attempt to get length of {}", type));
		}
	}
	Value Value::get(const std::string& s) {
		if (isTable()) {
			return getTable()->get(s);
		}
		else {
			throw RuntimException(format("attempt to get item of {}", type));
		}
	}

	void Value::set(int i, const Value& v) {
		if (isTable()) {
			getTable()->set(i, v);
		}
		else {
			throw RuntimException(format("attempt to set item of {}", type));
		}
	}

	void Value::set(const std::string& s, const Value& v) {
		if (isTable()) {
			getTable()->set(s, v);
		}
		else {
			throw RuntimException(format("attempt to set item of {}", type));
		}
	}

	Value Value::get(Value& k) {
		if (k.isArithmetic()) {
			return get(k.getInt());
		}
		else if (k.isString()) {
			return get(k.getString()->str());
		}
		else {
			throw RuntimException("invalid type of key");
		}
	}

	void Value::set(Value& k, const Value& v) {
		if (k.isArithmetic()) {
			set(k.getInt(), v);
		}
		else if (k.isString()) {
			set(k.getString()->str(), v);
		}
		else {
			throw RuntimException("invalid type of key");
		}
	}
	void Value::setMetaTable(Value* a, Value* b) {
		if (a->isTable()) {
			b->metatable = a->getTable();
		}
		else {
			throw RuntimException(format("{} expected but found when setting metatable", TTable, a->type));
		}
	}


	void Value::setMetaTable(const Value& v) {
		if (v.isTable()) {
			metatable = v.getTable();
		}
		else {
			throw RuntimException(format("{} expected but found setting metatable", TTable, v.type));
		}
	}


	Table* Value::getMetatable() const {
		return metatable;
	}

	void Value::clone(GC& gc, Value* a, Value* b) {
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
		}
		else {
			return !isNil();
		}
	}
}
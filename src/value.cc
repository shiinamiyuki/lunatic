#include "value.h"
#include "table.h"
#include "lstring.h"
#include "closure.h"
namespace lunatic {
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
		if (a->isInt() && b->isInt()) {
			c->setInt(a->getInt() + b->getInt());
		}
		else {
			c->setFloat(a->getFloat() + b->getFloat());
		}
	}

	void Value::sub(Value* a, Value* b, Value* c) {
		if (a->isInt() && b->isInt()) {
			c->setInt(a->getInt() - b->getInt());
		}
		else {
			c->setFloat(a->getFloat() - b->getFloat());
		}
	}

	void Value::mul(Value* a, Value* b, Value* c) {
		if (a->isInt() && b->isInt()) {
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
		if (!a->toBool())
			* c = *a;
		else
			*c = *b;
	}

	void Value::logicOr(Value* a, Value* b, Value* c) {
		if (a->toBool())
			* c = *a;
		else
			*c = *b;
	}

	void Value::neg(Value* a, Value* b) {
		if (a->isInt()) {
			b->setInt(-a->getInt());
		}
		else {
			b->setFloat(-a->getFloat());
		}
	}

	void Value::logicNot(Value* a, Value* b) {
		b->setInt(!a->toBool());
	}

	void Value::idiv(Value* a, Value* b, Value* c) {
		if (a->isInt() && b->isInt()) {
			c->setInt(a->getInt() / b->getInt());
		}
		else {
			c->setInt(a->getFloat() / b->getFloat());
		}
	}
	void Value::lt(Value* a, Value* b, Value* c) {
		if (a->isInt() && b->isInt()) {
			c->setInt(a->getInt() < b->getInt());
		}
		else {
			c->setInt(a->getFloat() < b->getFloat());
		}
	}

	void Value::gt(Value* a, Value* b, Value* c) {
		if (a->isInt() && b->isInt()) {
			c->setInt(a->getInt() > b->getInt());
		}
		else {
			c->setInt(a->getFloat() > b->getFloat());
		}
	}

	void Value::le(Value* a, Value* b, Value* c) {
		if (a->isInt() && b->isInt()) {
			c->setInt(a->getInt() <= b->getInt());
		}
		else {
			c->setInt(a->getFloat() <= b->getFloat());
		}
	}

	void Value::ge(Value* a, Value* b, Value* c) {
		if (a->isInt() && b->isInt()) {
			c->setInt(a->getInt() >= b->getInt());
		}
		else {
			c->setInt(a->getFloat() >= b->getFloat());
		}
	}

	void Value::eq(Value* a, Value* b, Value* c) {
		if (a->isInt() && b->isInt()) {
			c->setInt(a->getInt() == b->getInt());
		}
		else {
			c->setInt(a->getFloat() == b->getFloat());
		}
	}

	void Value::ne(Value* a, Value* b, Value* c) {
		if (a->isInt() && b->isInt()) {
			c->setInt(a->getInt() != b->getInt());
		}
		else {
			c->setInt(a->getFloat() != b->getFloat());
		}
	}

	Value::Value() :type(TNil) {}
	void Value::release(){
		if(isTable()){
			getTable()->release();
		}else if(isString()){
			getString()->release();
		}
	}
	void Value::setNil() {
		release();
		type = TNil;
	}
	void Value::setInt(int64_t val) {
		release();
		type = TInt;
		asInt = val;
	}
	void Value::setFloat(double val) {
		release();
		type = TFloat;
		asFloat = val;
	}

	void Value::setTable(Table* val) {
		val->retain();
		release();
		type = TTable;
		asTable = val;

	}

	void Value::setClosure(Closure* val) {
		val->retain();
		release();
		type = TClosure;
		asClosure = val;
	
	}

	void Value::setBool(bool val) {
		release();
		type = TInt;
		asInt = val;
	}
	void Value::setString(String * s){
		s->retain();
		release();
		asString = s;
		type = TString;
			
	}
	void Value::setString(const std::string&) {
		release();
		type = TString;
	}
	Value& Value::operator = (const Value& v) {
		release();
		type = v.type;
		asTable = v.asTable;
		if (isTable()) {
			asTable->retain();
		}
		else if (isString()) {
			asString->retain();
		}
		else if (isClosure()) {
			asClosure->retain();
		}
		return *this;
				
	}
	void Value::checkArithmetic() const {
		if (!isArithmetic())
			throw std::runtime_error("arithmetic object expected!");
	}
	void Value::checkInt() const {
		if (!isInt())
			throw std::runtime_error("int object expected!");
	}

	void Value::checkFloat() const {
		if (!isFloat() && !isInt()) {
			throw std::runtime_error("float object expected!");
		}
	}

	void Value::checkClosure() const {
		if (!isClosure()) {
			throw std::runtime_error("closure object expected!");
		}
	}

	void Value::checkTable() const {
		if (!isTable()) {
			throw std::runtime_error("table object expected!");
		}
	}
	void Value::checkString() const {
		if (!isString()) {
			throw std::runtime_error("string object expected!");
		}
	}
	void Value::checkUserData() const {
		if (!isUserData()) {
			throw std::runtime_error("user data expected!");
		}
	}

	bool Value::toBool() const {
		if (isInt()) {
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
	std::string Value::str() const {
		std::ostringstream out;
		if (isInt()) {
			out << getInt();
		}
		else if (isFloat()) {
			out << getFloat();
		}
		else if (isTable()) {
			auto& tab = *getTable();
			out << "{ ";
			for (auto i : tab.list) {
				out << i.str() << ", ";
			}
			for (auto i : tab.iMap) {
				out << "{" << i.first << ":" << i.second.str() << "},";
			}
			for (auto i : tab.sMap) {
				out << "{" << i.first << ":" << i.second.str() << "},";
			}
			out << "}";
		}
		else if (isClosure()) {
			out << "<closure at " << reinterpret_cast<long long>(asClosure) << ">";
		}
		else if (isString()) {
			out << getString()->str();
		}
		else if (type == TNil) {
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
			throw std::runtime_error("is not a table object");
		}
	}
	int Value::len() const {
		if (isArithmetic()) {
			throw std::runtime_error("attemp to get length of a number value");
		}
		else if (isTable()) {
			return getTable()->len();
		}
		else if (isString()) {
			return getString()->str().length();
		}
		else {
			throw std::runtime_error("cannot get length of the value");
		}
	}
	Value Value::get(const std::string& s) {
		if (isTable()) {
			return getTable()->get(s);
		}
		else {
			throw std::runtime_error("is not a table object");
		}
	}

	void Value::set(int i, const Value& v) {
		if (isTable()) {
			getTable()->set(i, v);
		}
		else {
			throw std::runtime_error("is not a table object");
		}
	}

	void Value::set(const std::string& s, const Value& v) {
		if (isTable()) {
			getTable()->set(s, v);
		}
		else {
			throw std::runtime_error("is not a table object");
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
			throw std::runtime_error("invalid type of key");
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
			throw std::runtime_error("invalid type of key");
		}
	}
	void Value::setMetaTable(Value* a, Value* b) {
		if (a->isTable()) {
			b->getTable()->metatable = a->getTable();
		}
		else {
			throw std::runtime_error("attemp to set the metatable of a non-table value");
		}
	}


	void Value::setMetaTable(const Value& v) {
		if (isTable()) {
			getTable()->metatable = v.getTable();
		}
		else {
			throw std::runtime_error("attemp to set the metatable of a non-table value");
		}
	}

	Table* Value::getMetatable() const {
		if (isTable()) {
			return getTable()->metatable;
		}
		else {
			throw std::runtime_error("attemp to get the metatable of a non-table value");
		}
	}

	void Value::clone(GC&gc,Value* a, Value* b) {
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
}
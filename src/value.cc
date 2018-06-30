/*
 * value.cc
 *
 *  Created on: 2018Äê6ÔÂ21ÈÕ
 *      Author: XiaochunTong
 */
#include "value.h"
#include "table.h"
SPEKA_BEGIN

bool Value::operator ==(const Value& rhs) {
	if (rhs.type != type) {
		return false;
	} else if(isArithmetic()){
		return getFloat() == rhs.getFloat();
	}
}
void Value::add(Value* a, Value* b, Value* c) {
	if (a->isInt() && b->isInt()) {
		c->setInt(a->getInt() + b->getInt());
	} else {
		c->setFloat(a->getFloat() + b->getFloat());
	}
}

void Value::sub(Value* a, Value* b, Value* c) {
	if (a->isInt() && b->isInt()) {
		c->setInt(a->getInt() - b->getInt());
	} else {
		c->setFloat(a->getFloat() - b->getFloat());
	}
}

void Value::mul(Value* a, Value* b, Value* c) {
	if (a->isInt() && b->isInt()) {
		c->setInt(a->getInt() * b->getInt());
	} else {
		c->setFloat(a->getFloat() * b->getFloat());
	}
}

void Value::mod(Value* a, Value* b, Value* c) {

}
void Value::div(Value* a, Value* b, Value* c) {
	c->setFloat(a->getFloat() / b->getFloat());
}

void Value::idiv(Value* a, Value* b, Value* c) {
	if (a->isInt() && b->isInt()) {
		c->setInt(a->getInt() / b->getInt());
	} else {
		c->setInt(a->getFloat() / b->getFloat());
	}
}
void Value::setInt(int i) {
	type = ValueType::intType;
	data.asInt = i;
	asObject.reset();
}

void Value::setFloat(double f) {
	type = ValueType::floatType;
	data.asFloat = f;
	asObject.reset();
}

void Value::setTable(Table* tab) {
	asObject.reset(tab);
	type = table;
}
void Value::setList(List*_list) {
	asObject.reset(_list);
	type = list;
}
void Value::setString(GCPtr s) {
	//assert(s);
	assert(!s.isNull());
	type = string;
	asObject = s;
}

Value Value::get(int i) {
	if (isTable()) {
		return getTable().get(i);
	} else if (isList()) {
		return getList().get(i);
	} else {
		throw std::runtime_error("is not a table object");
	}
}

Value Value::get(const std::string& s) {
	if (isTable()) {
		return getTable().get(s);
	} else {
		throw std::runtime_error("is not a table object");
	}
}

void Value::set(int i, const Value&v) {
	if (isTable()) {
		getTable().set(i, v);
	} else if(isList()){
		getList().set(i,v);
	}else {
		throw std::runtime_error("is not a table object");
	}
}

void Value::set(const std::string& s, const Value&v) {
	if (isTable()) {
		getTable().set(s, v);
	} else {
		throw std::runtime_error("is not a table object");
	}
}

void Value::setBool(bool b) {
	data.asInt = b;
}

Value Value::get(Value& k) {
	if (k.isArithmetic()) {
		return get(k.getInt());
	} else if (k.isString()) {
		return get(k.getString());
	} else {
		throw std::runtime_error("invalid type of key");
	}
}

void Value::set(Value&k, const Value&v) {
	if (k.isArithmetic()) {
		set(k.getInt(), v);
	} else if (k.isString()) {
		set(k.getString(), v);
	} else {
		throw std::runtime_error("invalid type of key");
	}
}

Value::Value() {
	setNil();
}

void Value::setNil() {
	type = nil;
}

void Value::setUserData(void*p) {
	data.asUserData = p;
	type = userData;
}



std::string Value::str() const {
	std::ostringstream out;
	if (isInt()) {
		out << getInt();
	} else if (isFloat()) {
		out << getFloat();
	} else if (isTable()) {
		auto& tab = *(asObject.get<Table>());
		out <<"{ ";
		for(auto i:tab.iMap){
			out << "{" << i.first <<":"<<i.second.str()<<"},";
		}
		for(auto i:tab.sMap){
			out << "{" << i.first << ":" << i.second.str() << "},";
		}
		out <<"}";
	} else if (isClosure()) {
		out << "<closure at " << getClosureAddr() <<">";
	} else if(isString()){
		out << *(asObject.get<std::string>());
	}else if(isList()){
		auto& l = getList();
		out <<"[ ";
		for(auto i:l){
			out << i.str()<<", ";
		}
		out <<"]";
	}else {
		out << "unsupported type" << std::endl;
	}
	return out.str();
}

void Value::lt(Value* a, Value* b, Value* c) {
	if (a->isInt() && b->isInt()) {
		c->setBool(a->getInt() < b->getInt());
	} else {
		c->setInt(a->getFloat() < b->getFloat());
	}
}

void Value::gt(Value* a, Value* b, Value* c) {
	if (a->isInt() && b->isInt()) {
		c->setInt(a->getInt() > b->getInt());
	} else {
		c->setInt(a->getFloat() > b->getFloat());
	}
}

void Value::le(Value* a, Value* b, Value* c) {
	if (a->isInt() && b->isInt()) {
		c->setInt(a->getInt() <= b->getInt());
	} else {
		c->setInt(a->getFloat() <= b->getFloat());
	}
}

void Value::ge(Value* a, Value* b, Value* c) {
	if (a->isInt() && b->isInt()) {
		c->setInt(a->getInt() >= b->getInt());
	} else {
		c->setInt(a->getFloat() >= b->getFloat());
	}
}

void Value::eq(Value* a, Value* b, Value* c) {
	if (a->isInt() && b->isInt()) {
		c->setInt(a->getInt() == b->getInt());
	} else {
		c->setInt(a->getFloat() == b->getFloat());
	}
}

void Value::ne(Value* a, Value* b, Value* c) {
	if (a->isInt() && b->isInt()) {
		c->setInt(a->getInt() != b->getInt());
	} else {
		c->setInt(a->getFloat() != b->getFloat());
	}
}
void Value::clone(Value* a, Value* b) {
	if(a->isTable()){
		b->setTable(new Table(a->getTable()));//TODO: set proto instead of direct copy
	//	b->asObject.get<Table>()->proto = a->asObject;
	}
}
void Value::setProto(Value* a, Value* b) {
	if (a->isTable()) {
		b->asObject.get<Table>()->proto = a->asObject;
	}
}

void Value::setClosure(int i) {
	data.asInt = i;
	type = closure;
}
Value List::get(int i) {
	return at(i);
}

void List::set(int i, const Value&v) {
	at(i)= v;
}
SPEKA_END



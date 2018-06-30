/*
 * value.h
 *
 *  Created on: 2018Äê6ÔÂ21ÈÕ
 *      Author: XiaochunTong
 */

#ifndef VALUE_H_
#define VALUE_H_
#include "speka.h"
#include "gc.h"
SPEKA_BEGIN
class Table;
class Value;
class List:public std::vector<Value>
{
public:
	Value get(int i);
	void set(int i,const Value &);
};
class Value{
	union Data{
		int asInt;
		double asFloat;
		void * asUserData;
	} data;
	enum ValueType {
		nil, intType, floatType, closure, list,table, string, userData
	};
	GCPtr asObject;
	ValueType type;
public:
	Value();
	void setNil();
	void setInt(int);
	void setFloat(double);
	void setTable(Table*);
	void setClosure(int);
	void setBool(bool);
	void setString(GCPtr);
	void setUserData(void*);
	void setList(List*);
	bool operator ==(const Value&rhs);
	static void add(Value*a, Value*b, Value *c);
	static void sub(Value*a, Value*b, Value*c);
	static void mul(Value*a, Value*b, Value *c);
	static void mod(Value*a, Value*b, Value*c);
	static void div(Value*a, Value*b, Value*c);
	static void idiv(Value*a, Value*b, Value*c);
	static void lt(Value*a, Value*b, Value*c);
	static void gt(Value*a, Value*b, Value*c);
	static void le(Value*a, Value*b, Value*c);
	static void ge(Value*a, Value*b, Value*c);
	static void eq(Value*a, Value*b, Value*c);
	static void ne(Value*a, Value*b, Value*c);
	static void clone(Value *a,Value* b);
	static void setProto(Value*a,Value*v);
	Value get(Value&);
	void set(Value&,const Value&);
	Value get(int i);
	Value get(const std::string&);
	void set(int i,const Value&);
	void set(const std::string&,const Value&);
	std::string str() const;
	inline Table& getTable() const {
		return *asObject.get<Table>();
	}
	inline List& getList()const{
		return *asObject.get<List>();
	}
	inline int getClosureAddr() const {
		return data.asInt;
	}
	inline bool isString() const{
		return type == string;
	}
	inline bool isClosure() const {
		return type == closure;
	}
	inline bool isInt() const {
		return type == intType;
	}
	inline bool isFloat() const {
		return type == floatType;
	}
	inline bool isArithmetic() const {
		return isInt() || isFloat();
	}
	inline bool isTable() const {
		return type == table;
	}
	inline bool isList()const{
		return type == list;
	}
	inline bool isUserData()const{return type == userData;}
	inline void* getUserData()const{return data.asUserData;}
	inline int getInt() const {
		if (isInt()) {
			return data.asInt;
		} else {
			return data.asFloat;
		}
	}
	inline double getFloat() const {
		if (isInt()) {
			return data.asInt;
		} else {
			return data.asFloat;
		}
	}
	inline static bool checkArithmetic(const Value*a, const Value * b) {
		return a->isArithmetic() && b->isArithmetic();
	}
	inline bool toBool() const {
		if (isInt()) {
			return getInt();
		} else if (isFloat()) {
			return getFloat();
		} else {
			return !asObject.isNull();
		}
	}
	inline Value & operator = (const Value &v){
		type = v.type;
		data = v.data;
		asObject = v.asObject;
	}
	inline const std::string& getString()const{
		return *asObject.get<std::string>();
	}
};
SPEKA_END
#endif /* VALUE_H_ */

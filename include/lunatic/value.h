#pragma once
#include "common.h"

namespace lunatic {
	class Table;
	class Closure;	
	class GC;
	class String;
	class Value {
	public:
		enum Type {
			TNil,
			TFloat,
			TInt,
			TTable,
			TString,
			TClosure,
			TUserData
		};
		Value();
	private:
		Type type;
		union {
			Table* asTable;
			Closure* asClosure;
			String* asString;
			int64_t asInt;
			double asFloat;
			void* asUserData;
		};
	public:
		void setNil();

		void setInt(int64_t);

		void setFloat(double);

		void setTable(Table*);

		void setClosure(Closure*);

		void setBool(bool);

		void setUserData(void* p) {
			type = TUserData;
			asUserData = p;
		}

		void setString(const std::string&);
		void setString(String* s) {
			asString = s;
			type = TString;
		}
		bool isString() const {
			return type == TString;
		}

		bool isClosure() const {
			return type == TClosure;
		}

		bool isInt() const {
			return type == TInt;
		}

		bool isFloat() const {
			return type == TFloat;
		}
		bool isUserData()const {
			return type == TUserData;
		}
		bool isArithmetic() const {
			return isInt() || isFloat();
		}

		bool isManaged()const {
			return isTable();
		}

		bool isTable() const {
			return type == TTable;
		}
		void checkInt() const;

		void checkFloat() const;

		void checkClosure() const;

		void checkTable() const;

		void checkString() const;

		void checkArithmetic() const;

		void checkUserData()const;

		bool operator==(const Value& rhs);

		static void add(Value* a, Value* b, Value* c);

		static void sub(Value* a, Value* b, Value* c);

		static void mul(Value* a, Value* b, Value* c);

		static void mod(Value* a, Value* b, Value* c);

		static void div(Value* a, Value* b, Value* c);

		static void logicAnd(Value* a, Value* b, Value* c);

		static void logicOr(Value* a, Value* b, Value* c);

		static void idiv(Value* a, Value* b, Value* c);

		static void lt(Value* a, Value* b, Value* c);

		static void gt(Value* a, Value* b, Value* c);

		static void le(Value* a, Value* b, Value* c);

		static void ge(Value* a, Value* b, Value* c);

		static void eq(Value* a, Value* b, Value* c);

		static void ne(Value* a, Value* b, Value* c);

		static void neg(Value* a, Value* b);

		static void logicNot(Value* a, Value* b);

		static bool checkArithmetic(Value* a, Value* b) {
			return a->isArithmetic() && b->isArithmetic();
		}
		static void len(Value* a, Value* b);
		static void clone(GC& ,Value* a, Value* b);
		Value get(Value&);

		void set(Value&, const Value&);

		Value get(int i);

		Value get(const std::string&);

		void set(int i, const Value&);

		void set(const std::string&, const Value&);
		int len() const;

		inline int64_t getInt() const {
			if (isInt()) {
				return asInt;
			}
			else {
				return asFloat;
			}
		}

		inline double getFloat() const {
			if (isInt()) {
				return asInt;
			}
			else {
				return asFloat;
			}
		}
		Table* getTable()const {
			return asTable;
		}
		String* getString()const {
			return asString;
		}
		Closure* getClosure()const {
			return asClosure;
		}
		void* getUserData()const {
			return asUserData;
		}
		bool toBool()const;
		std::string str() const;

		static void setMetaTable(Value* a, Value* v);

		void setMetaTable(const Value&);

		Table* getMetatable() const;

		void setArgCount(int i);
		int getClosureAddr() const;
	};
}
#pragma once
#include "common.h"

namespace lunatic {
	class Table;
	class Closure;
	class GC;
	class String;
	class __SerializeContextImpl;
	class VM;
	class SerializeContext {
		std::shared_ptr<VM> vm;
	public:
		SerializeContext(std::shared_ptr<VM>);

		String* newString(const std::string&);
		Table* newTable();
	};
	template<class T>
	struct Serializer;
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
		void release();
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

		void setString(String* s);
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

		bool isNil()const{return type == TNil;}
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
		static void clone(GC&, Value* a, Value* b);
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

		template<class T>
		T load(SerializeContext* ctx = nullptr)const {
			T tmp;
			Serializer<T>::deserialize(*this, tmp, ctx);
			return tmp;
		}

		template<class T>
		void store(const T& v, SerializeContext* ctx = nullptr) {
			Serializer<T>::serialize(*this, v, ctx);
		}
		Value& operator = (const Value& v);
	};

	template<class T>
	struct Serializer {
		static void serialize( Value& v, const T& out, SerializeContext* ctx) {
			toLuaValue(v, out, ctx);
		}
		static void deserialize(const Value& v, T& out, SerializeContext* ctx) {
			fromLuaValue(v, out, ctx);
		}
	};

	inline void toLuaValue(Value& v, int i, SerializeContext* ctx) {
		v.setInt(i);
	}

	inline void toLuaValue(Value& v, size_t i, SerializeContext* ctx) {
		v.setInt(i);
	}

	inline void toLuaValue(Value& v, double f, SerializeContext* ctx) {
		v.setFloat(f);
	}

	inline void fromLuaValue(Value& v, int& i, SerializeContext* ctx) {
		v.checkInt();
		i = v.getInt();
	}

	inline void fromLuaValue(Value& v, size_t& i, SerializeContext* ctx) {
		v.checkInt();
		i = v.getInt();
	}

	inline void fromLuaValue(Value& v, double& f, SerializeContext* ctx) {
		v.checkFloat();
		f = v.getFloat();
	}
	void fromLuaValue(const Value& v, std::string& ,SerializeContext* ctx);
	void fromLuaValue(const Value& v, const char*& , SerializeContext* ctx);
	void toLuaValue(Value& v, const std::string& ,SerializeContext* ctx);
	void toLuaValue(Value& v, const char* , SerializeContext* ctx);
}
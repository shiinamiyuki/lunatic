#pragma once

#include "gc.h"

namespace lunatic {
	class Value;
	class UpValue;
	class VM;
	struct CallContext {
		VM* vm;
		size_t nArgs;
		CallContext(VM* vm, size_t n) :vm(vm), nArgs(n) {}
	};
	class Callable : public GCObject {
	public:
		virtual void call(const CallContext&) = 0;
	};

	class Closure : public Callable {
		int addr;
		int argCount;
		UpValue* upValue=nullptr;
		//  std::unordered_map<unsigned int, Value> upValue;
		UpValue* parent= nullptr;
	public:
		void setParentUpValue(UpValue* p) {
			parent = p;
		}
		UpValue* getParentUpValue() {
			return parent;
		}
		Closure(int a, int c);

		inline int getAddress() const { return addr; }

		inline int getArgCount() const { return argCount; }

		inline void setArgCount(int i) { argCount = i; }
		void setUpvalue(UpValue* up);
		UpValue* getUpValue()const {
			return upValue;
		}
		Value get(int);
		void set(int, const Value&);
		void markReferences(GC* gc)const override;
		//  inline Value& getUpValue(unsigned int i)const{return upValue[i];}
	   //   inline void setUpValue(unsigned int i, const Value &v){upValue[i] = v;}
		virtual size_t nBytes()const{
			return sizeof(*this);
		}

		// this creates new native stack frame (larger overhead)
		void call(const CallContext&)override;
	};

	class LightFunction : public Callable{
		std::function<void(const CallContext&)> func;
	public:
		LightFunction(std::function<void(const CallContext&)> f):func(std::move(f)){}
		void call(const CallContext&ctx)override {
			func(ctx);
		}
		void markReferences(GC* gc)const override {}
		virtual size_t nBytes()const {
			return sizeof(*this);
		}
	};
}
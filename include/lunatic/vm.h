#pragma once
#include "common.h"
#include "opcode.h"
#include "value.h"
#include "table.h"
#include "lstring.h"
#include "closure.h"
namespace lunatic {
	class VM;
	class Table;
	typedef std::function<void(const CallContext&)> NativeHandle;
	struct CallInfo {
		Closure* closure = nullptr;
		int pc;
		int bp;
		int sp;
		int argCount;
		CallInfo(Closure* c, int _pc, int _bp, int _sp, int a = 0) {
			closure = c;
			pc = _pc;
			bp = _bp;
			sp = _sp;
			argCount = a;
		}
	};
	struct State {
		std::vector<Value> locals;
		std::vector<Value> retReg;
		//CallInfo* callInfo;
		std::vector<CallInfo>callStack;
		State* parent = nullptr;
		int pc, bp, sp;
		Value* registers;
		Closure* closure = nullptr;
		bool ok;
		inline void next() {
			pc++;
		}
		void reset();
		Value* getTop() {
			return locals.data() + sp;
		}
		State(Closure * closure = nullptr, size_t localSize = REG_MAX * 24):closure(closure) {
			locals.resize(localSize);
			retReg.resize(REG_MAX);
			pc = 0;
			bp = 0;
			sp = 0;
			updateReg();
			//callInfo = nullptr;
			callStack.reserve(1024);
			ok = true;
		}
		inline void updateReg() {
			registers = locals.data() + bp;
		}
		inline void push(const Value& v) {
			*(registers + REG_MAX + sp) = v;
			push();
		}
		inline void push() {
			sp++;
		}
		inline void call(Closure* closure, int n) {
			int addr = closure->getAddress();
			callStack.push_back(CallInfo(this->closure, pc, bp, sp - n, n));
			//	CallInfo* info = new CallInfo(callInfo,pc,bp,sp - n);//TODO : sp- number of args
			this->closure = closure;
			pc = addr;
			bp += REG_MAX;
			sp = 0;
			if (bp + REG_MAX >= locals.size()) {
				locals.resize(2 * locals.size());
			}
			updateReg();
			//callInfo = info;
			for (auto& i : retReg) {
				i.setNil();
			}
		}
		CallInfo save()const {
			return CallInfo(nullptr, pc, bp, sp, 0);
		}
		void restoreFrom(const CallInfo& callInfo) {
			pc = callInfo.pc;
			bp = callInfo.bp;
			sp = callInfo.sp;
			closure = callInfo.closure;
		}
		inline void ret() {
			if (!callStack.empty()) {
				auto& callInfo = callStack.back();
				for (int i = bp; i < bp + REG_MAX; i++) {
					locals[i].setNil();
				}
				restoreFrom(callInfo);
				callStack.pop_back();
			}
			else {
				ok = false;
			}
			updateReg();
			//	popSelf();
		}

		inline int getArgCount()const {
			return callStack.back().argCount;
		}
		Closure* getCurrentClosure()const {
			return closure;
		}
		Closure* getParentClosure()const {
			if (!callStack.empty()) {
				return callStack.back().closure;
			}
			else if(parent){
				return parent->getCurrentClosure();
			}
			else {
				return nullptr;
			}
		}
	};
	class ScriptEngine;
	class VM {
		std::vector<String*> stringPool;
		std::vector<Value> globals;
		std::vector<Instruction> program;
		std::vector<std::unique_ptr<Callable>> natives;
		std::list<State> states;
		State* cur;
		State _state;
		void loadProgram(const std::vector<Instruction>& p);
		void loadStringPool(const std::vector<std::string>& p);
		GC gc;

		void eval(State* state);
		void mark(State* state);
	public:
		void forceRecurse() {
			eval(getCurrentState());
		}
		void collect();
		template<class T, class... Args>
		T* alloc(Args&& ...args) {
			return gc.alloc<T>(args...);
		}
		friend class ScriptEngine;
		VM() {
			reset();
			cur = &_state;
		}

		void reset();

		void invokeMetaMethod(Value* a, Value* b, Value* c, const char* key, int n = 0);

		void addNative(std::unique_ptr<Callable>&&);
		Value& getLocal(int i);
		void storeReturn(int i, const Value&);

		void call(Closure*, int n);
		inline State* getCurrentState()const { return cur; }
		inline bool checkArithmetic(Value* a, Value* b) {
			return Value::checkArithmetic(a, b);
		}

		inline int getArgCount() {
			return getCurrentState()->getArgCount();
		}
		void exec();

		size_t getMemoryUsage(bool force = false)const {
			return gc.getMemoryUsage(force);
		}

	};
}
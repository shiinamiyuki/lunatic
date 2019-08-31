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
	typedef std::function<void(VM*)> NativeHandle;


	struct CallInfo {
		CallInfo* prev;
		Closure* closure = nullptr;
		int pc;
		int bp;
		int sp;
		int argCount;
		CallInfo(CallInfo* p, int _pc, int _bp, int _sp, int a = 0) {
			prev = p;
			pc = _pc;
			bp = _bp;
			sp = _sp;
			argCount = a;
		}
		CallInfo(int _pc, int _bp, int _sp, int a = 0) {
			prev = nullptr;
			pc = _pc;
			bp = _bp;
			sp = _sp;
			argCount = a;
		}
		inline bool canReturn() const {
			return prev;
		}
	};
	struct State {
		std::vector<Value> locals;
		std::vector<Value> retReg;
		//CallInfo* callInfo;
		std::vector<CallInfo>callStack;
		int pc, bp, sp;
		Value* registers;
		bool ok;
		inline void next() {
			pc++;
		}
		void reset();
		Value* getTop() {
			return locals.data() + sp;
		}
		State() {
			for (int i = 0; i < 100000; i++)
				locals.push_back(Value());
			retReg.push_back(Value());
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
		inline void call(Closure *closure, int n) {
			int addr = closure->getAddress();
			callStack.push_back(CallInfo(nullptr, pc, bp, sp - n, n));
			callStack.back().closure = closure;
			//	CallInfo* info = new CallInfo(callInfo,pc,bp,sp - n);//TODO : sp- number of args
			pc = addr;
			bp += REG_MAX;
			sp = 0;
			if(bp + REG_MAX >= locals.size()){
				locals.resize(2 * locals.size());
			}
			updateReg();
			//callInfo = info;
			for(auto & i:retReg){
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
		}
		inline void ret() {
			/*		if(callInfo){
						pc = callInfo->pc;
						bp = callInfo->bp;
						sp = callInfo->sp;
						auto prev = callInfo->prev;
						delete callInfo;
						callInfo = prev;
					}else{
						ok = false;
					}*/
			if (callStack.size()) {
				auto& callInfo = callStack.back();
				for(int i =bp;i<bp + REG_MAX;i++){
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
	};
	class ScriptEngine;
	class VM {
		std::vector<String*> stringPool;
		std::vector<Value> globals;
		std::vector<Instruction> program;
		std::vector<NativeHandle> natives;
		State* cur;
		State _state;
		void loadProgram(const std::vector<Instruction>& p);
		void loadStringPool(const std::vector<std::string>& p);
		GC gc;
		
		void eval(State* state);
		Closure* getCurrentClosure()const {
			auto s = getCurrentState()->callStack.size();
			if (s >= 1)
				return getCurrentState()->callStack[s - 1].closure;
			else
				return nullptr;
		}
		Closure* getParentClosure()const {
			auto s = getCurrentState()->callStack.size();
			if (s >= 2)
				return getCurrentState()->callStack[s - 2].closure;
			else
				return nullptr;
		}
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

		void addNative(NativeHandle);
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

		size_t getMemoryUsage(bool force=false)const{
			return gc.getMemoryUsage(force);
		}

	};
}
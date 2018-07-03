/*
 * vm.h
 *
 *  Created on: 2018Äê6ÔÂ22ÈÕ
 *      Author: XiaochunTong
 */

#ifndef VM_H_
#define VM_H_
#include "opcode.h"
#include "speka.h"
#include "value.h"
SPEKA_BEGIN
class VM;
class Table;
typedef std::function<void(VM*)> NativeHandle;
class VMException: public std::runtime_error {

};

struct CallInfo {
	CallInfo * prev;
	int pc;
	int bp;
	int sp;
	CallInfo(CallInfo *p, int _pc, int _bp, int _sp) {
		prev = p;
		pc = _pc;
		bp = _bp;
		sp = _sp;
	}
	CallInfo(int _pc, int _bp, int _sp) {
		prev = nullptr;
		pc = _pc;
		bp = _bp;
		sp = _sp;
	}
	inline bool canReturn() const {
		return prev;
	}
};
struct State {
	std::vector<Value> locals;
	std::vector<Value> retReg;
	std::vector<Value> selfStack;
	CallInfo* callInfo;
	int pc, bp, sp;
	Value *registers;
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
		callInfo = nullptr;
		ok = true;
	}
	inline void updateReg() {
		registers = locals.data() + bp;
	}
	inline void push() {
		sp++;
	}
	inline void call(int addr ,int n) {
		CallInfo* info = new CallInfo(callInfo,pc,bp,sp - n);//TODO : sp- number of args
		pc = addr;
		bp += REG_MAX;
		sp = 0;
		updateReg();
		callInfo = info;
	}
	inline void ret(){
		if(callInfo){
			pc = callInfo->pc;
			bp = callInfo->bp;
			sp = callInfo->sp;
			auto prev = callInfo->prev;
			delete callInfo;
			callInfo = prev;
		}else{
			ok = false;
		}
		updateReg();
		popSelf();
	}
	inline void pushSelf(const Value &v){
		selfStack.push_back(v);
	}
	inline Value getSelf()const{return selfStack.back();}
	inline void popSelf(){selfStack.pop_back();}
};
class ScriptEngine;
class VM {
	std::vector<GCPtr> stringPool;
	std::vector<Value> globals;
	std::vector<Instruction> program;
	std::vector<NativeHandle> natives;
	State * cur;
	void loadProgram(const std::vector<Instruction>&p);
	void loadStringPool(const std::vector<std::string>&p);
public:
	friend class ScriptEngine;
	VM() {
		reset();
	}

	void reset();
	void eval(State*state);
	void invokeMetaMethod(const char * key);
	inline bool checkArithmetic(Value *a, Value* b) {
		return Value::checkArithmetic(a,b);
	}
	void addNative(NativeHandle);
	Value &getLocal(int i);
	void storeReturn(int i,const Value& );
	inline State* getCurrentState()const{return cur;}
	void call(int addr,int n);
};
SPEKA_END
#endif /* VM_H_ */

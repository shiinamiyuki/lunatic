#include "vm.h"
#include "table.h"
#include "value.h"
#include "closure.h"
namespace lunatic {


	void VM::reset() {
		globals.resize(1024);
	}

#define GetState()  auto state = getCurrentState()
#define GetRet(i)  (state->retReg.data() + (i))
#define GetReg(i)  (state->registers + (i))
#define GetABC(i)  a = GetReg((i).getA()); b = GetReg((i).getB()); c = GetReg((i).getC())
#define DO_OP(op,meta) if(!checkArithmetic(a,b)){ \
	invokeMetaMethod(a,b,c,meta);\
}else{ op(a,b,c);/*std::cout <<"op result :"<<c->str()<<std::endl;*/ }
#define DO_ARITH(op,meta) state->next();GetABC(i);DO_OP(op,meta)
#define PRT(x) std::cout <<(x)->str()<<std::endl
	void VM::eval(State * state) {
		Value* a = nullptr;
		Value* b = nullptr;
		Value* c = nullptr;
		cur = state;
		int i32;
		double f64;
		size_t gcCycle = 0;
		if (state->selfStack.size() == 0)
			state->selfStack.push_back(Value());
		int size = program.size();
		int callsize = cur->callStack.size();
		while (state->pc < size && state->ok) {
			auto& i = program[state->pc];
			gcCycle++;
			if (gcCycle > 1ull << 35)
			{
				gcCycle = 0;
				collect();
			}
			//std::cout << i.str() <<std::endl;
			//	system("pause");
			switch (i.opcode) {
			case Opcode::LoadInt:
				a = GetReg(i.getA());
				i32 = i.getInt();
				a->setInt(i32);
				state->next();
				break;
			case Opcode::LoadFloat:
				a = GetReg(i.getA());
				f64 = i.getFloat();
				a->setFloat(f64);
				state->next();
				break;
			case Opcode::Add:
				DO_ARITH(Value::add, "__add")
					break;
			case Opcode::Sub:
				DO_ARITH(Value::sub, "__sub")
					break;
			case Opcode::Div:
				DO_ARITH(Value::div, "__div")
					break;
			case Opcode::iDiv:
				DO_ARITH(Value::idiv, "__div")
					break;
			case Opcode::Mul:
				DO_ARITH(Value::mul, "__mul")
					break;
			case Opcode::Mod:
				DO_ARITH(Value::mod, "__mod")
					break;
			case Opcode::Neg:
				a = GetReg(i.getA());
				b = GetReg(i.getB());
				Value::neg(a, b);
				state->next();
				break;
			case Opcode::Not:
				a = GetReg(i.getA());
				b = GetReg(i.getB());
				Value::logicNot(a, b);
				state->next();
				break;
			case Opcode::Len:
				a = GetReg(i.getA());
				b = GetReg(i.getB());
				Value::len(a, b);
				state->next();
				break;
			case Opcode::And:
				DO_ARITH(Value::logicAnd, "__and")
					break;
			case Opcode::Or:
				DO_ARITH(Value::logicOr, "__or")
					break;
			case Opcode::LT:
				DO_ARITH(Value::lt, "__lt")
					break;
			case Opcode::GT:
				DO_ARITH(Value::gt, "__gt")
					break;
			case Opcode::LE:
				DO_ARITH(Value::le, "__le")
					break;
			case Opcode::GE:
				DO_ARITH(Value::ge, "__ge")
					break;
			case Opcode::EQ:
				DO_ARITH(Value::eq, "__eq")
					break;
			case Opcode::NE:
				DO_ARITH(Value::ne, "__ne")
					break;
			case Opcode::NewTable:
				a = GetReg(i.getA());
				a->setTable(gc.alloc<Table>(Table()));
				state->next();
				break;
			case Opcode::GetValue:
				GetABC(i);
				*c = a->get(*b);
				state->next();
				break;
			case Opcode::StoreValue:
				GetABC(i);
				a->set(*b, *c);
				state->next();
				break;
			case Opcode::LoadStr:
				a = GetReg(i.getA());
				i32 = i.getBx();
				a->setString(&stringPool[i32]);
				state->next();
				break;
			case Opcode::LoadGlobal:
				a = GetReg(i.getA());
				i32 = i.getBx();
				*a = globals[i32];//here!
				state->next();
				break;
			case Opcode::PushNil:
				state->pushSelf(Value());
				state->next();
				break;
			case Opcode::SetProto:
				a = GetReg(i.getA());
				b = GetReg(i.getB());
				Value::setMetaTable(a, b);
				state->next();
				break;
			case Opcode::Clone:
				a = GetReg(i.getA());
				b = GetReg(i.getB());
				Value::clone(gc, a, b);
				state->next();
				break;
			case Opcode::StoreGlobal:
				a = GetReg(i.getA());
				i32 = i.getBx();
				globals[i32] = *a;
				state->next();
				break;
			case Opcode::Move:
				a = GetReg(i.getA());
				b = GetReg(i.getB());
				*b = *a;
				state->next();
				break;
			case Opcode::BRC:
				state->pc = i.getInt();
				break;
			case Opcode::BZ:
				a = GetReg(i.getA());
				state->pc = a->toBool() ? state->pc + 1 : i.getInt();
				break;
			case Opcode::BNZ:
				a = GetReg(i.getA());
				state->pc = !a->toBool() ? state->pc + 1 : i.getInt();
				break;
			case Opcode::StoreRet:
				a = GetReg(i.getA());
				b = GetRet(i.getB());
				*b = *a;
				state->next();
				break;
			case Opcode::LoadRet:
				a = GetRet(i.getA());
				b = GetReg(i.getB());
				*b = *a;
				state->next();
				break;
			case Opcode::MakeClosure:
				a = GetReg(i.getA());
				a->setClosure(gc.alloc<Closure>(i.getInt(), 0));
				state->next();
				break;
			case Opcode::SetArgCount:
				a = GetReg(i.getA());
				a->setArgCount(i.getInt());
				state->next();
				break;
			case Opcode::Push:
				a = GetReg(i.getA());
				b = GetReg(REG_MAX + state->sp);
				*b = *a;
				state->sp++;
				state->next();
				break;
			case Opcode::fCall:
				a = GetReg(i.getA());
				state->next();
				if (!a->isClosure()) {
					invokeMetaMethod(a, nullptr, nullptr, "__call", i.getB());
				}
				else {
					state->call(a->getClosureAddr(), i.getB());
				}
				break;
			case Opcode::Ret:
				state->ret();
				state->ok = state->ok && callsize <= cur->callStack.size();
				break;
			case Opcode::invoke:
				i32 = i.getInt();
				state->next();
				natives[i32](this);
				break;
			default:
				std::cerr << "unknown opcode " << std::endl;
				state->next();
				break;
			}
		}
		//	std::cout << globals[0].str()<<std::endl;
	}

	void VM::invokeMetaMethod(Value* a, Value* b, Value* c, const char* key, int n) {
		//throw std::runtime_error("metamethod not implemented!");
		if (a && b && c) {
			//     std::cout << a->str() << std::endl;
			a->checkTable();
			auto cur = getCurrentState();
			cur->push(*a);
			cur->push(*b);
			auto meta = a->get(key);
			meta.checkClosure();
			int addr = meta.getClosureAddr();
			call(addr, 2);
			eval(cur);
			cur->ok = true;
			*c = cur->retReg[0];
			//   std::cout << "done"<<std::endl;
		}
		else {
			a->checkTable();
			auto cur = getCurrentState();
			auto meta = a->get(key);
			meta.checkClosure();
			int addr = meta.getClosureAddr();
			call(addr, 2);
			eval(cur);
			cur->ok = true;
		}
	}

	void State::reset() {
		locals.resize(4096);
		pc = 0;
		bp = 0;
		sp = 0;
		registers = locals.data();
	}

	void VM::addNative(NativeHandle h) {
		natives.push_back(h);
	}

	void VM::loadProgram(const std::vector<Instruction>& p) {
		while (p.size() > program.size()) {
			program.push_back(p[program.size()]);
		}
	}

	void VM::loadStringPool(const std::vector<std::string>& p) {
		while (p.size() > stringPool.size()) {
			//stringPool.push_back(p[stringPool.size()]);
			stringPool.emplace_back(p[stringPool.size()]);
		}
	}

	Value& VM::getLocal(int i) {
		GetState();
		return *GetReg(i);
	}

	void VM::storeReturn(int i, const Value& v) {
		GetState();
		auto r = GetRet(i);
		*r = v;
	}

	void VM::call(int addr, int n) {
		auto state = getCurrentState();
		state->call(addr, n);
	}


	void VM::collect(){
		gc.prepareForCollect();
		auto s = getCurrentState();
		for(int i =0;i<s->bp + REG_MAX;i++){
			gc.mark(s->locals[i]);
		}
		for(auto & i:globals){
			gc.mark(i);
		}
		gc.sweep();
	}
	void VM::exec(){
		eval(getCurrentState());
	}
}
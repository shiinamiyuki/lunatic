#include "codegen.h"

namespace lunatic {

	void CodeGen::visit(Number* node) {
		auto& tok = node->getToken().tok;
		std::istringstream outi(tok), outf(tok);
		int i;
		double f;
		outi >> i;
		outf >> f;
		int r = findReg();
		if (i == f) {
			emit(Instruction(Opcode::LoadInt, r, i));
		}
		else {
			emit(Instruction(Opcode::LoadFloat, r, f));
		}

	}

	void CodeGen::addString(const std::string& s) {
		auto iter = (strConst.find(s));
		if (iter == strConst.end()) {
			int i = strConst.size();
			strConst.insert(std::make_pair(s, i));
			strPool.push_back(s);
		}
	}

	void CodeGen::visit(StringLiteral* node) {
		auto s = node->getToken().tok;
		addString(s);
		emit(Instruction(Opcode::LoadStr, findReg(), strConst[s]));
	}

	// This is shit
	// refactor it
	void CodeGen::assign(AST* node) {

		auto first = node->first();
		if (first->type() == Identifier().type()) {
			auto& var = node->first()->getToken();
			if (!hasVar(var.tok)) {
				createGlobal(var); // default globals
			}


			int rhs = popReg();
			if (isGlobal(node->first()->getToken().tok)) {
				int addr = getGlobalAddress(node->first()->getToken());
				emit(Instruction(Opcode::StoreGlobal, rhs, addr));
			}
			else {
				auto& v = getLocal(node->first()->getToken());
				int addr = v.addr;
				if (v.captured) {
					emit(Instruction(Opcode::StoreUpvalue, rhs, addr));
				}
				else {
					emit(Instruction(Opcode::Move, rhs, addr));
				}
			}
		}
		else if (first->type() == Index().type()
			|| first->type() == Colon().type()) {
			auto idx = first;
			auto t = idx->first();
			auto i = idx->second();
			t->accept(this);
			i->accept(this);
			int a, b;
			b = popReg();
			a = popReg();
			int rhs = popReg();
			emit(Instruction(Opcode::StoreValue, a, b, rhs));
		}
		else if (first->type() == Local().type()) {
			auto& var = first->first()->getToken();
			createLocal(var);

			int rhs = popReg();
			auto& v = getLocal(var);
			int addr = v.addr;
			if (v.captured) {
				emit(Instruction(Opcode::StoreUpvalue, rhs, addr));
			}
			else {
				emit(Instruction(Opcode::Move, rhs, addr));
			}

		}
	}

	void CodeGen::visit(Identifier* node) {
		auto& var = node->getToken();
		if (!hasVar(var.tok)) {
			createGlobal(var);
		}
		if (isGlobal(var.tok)) {
			int a = getGlobalAddress(var);
			int r = findReg();
			emit(Instruction(Opcode::LoadGlobal, r, a));
		}
		else {
			auto& v = getLocal(var);
			if (!v.captured) {
				int a = v.addr;
				reg.push_back(a);
			}
			else {
				emit(Instruction(Opcode::LoadUpvalue, findReg(), v.addr));
			}
		}
	}

	void CodeGen::visit(Local* node) {

		if (dynamic_cast<BinaryExpression*>(node->first())) {
			createLocal(node->first()->first()->getToken());
			node->first()->accept(this);
		}
		else if (dynamic_cast<ParallelAssign*>(node->first()))
		{
			auto para = dynamic_cast<ParallelAssign*>(node->first());
			for (auto i : *para->first()) {
				createLocal(i->getToken());
			}
			para->accept(this);
		}
		else {//identifier
			createLocal(node->first()->getToken());
			emit(Instruction(Opcode::LoadNil, getLocalAddress(node->first()->getToken()), 0, 0));
		}

	}

	void CodeGen::visit(BinaryExpression* node) {
		auto op = node->getToken().tok;
		if (op == "=") {
			node->second()->accept(this);
			assign(node);
		}
		else if (op == "and") {
			node->first()->accept(this);
			int a = popReg();
			auto end = program.size();
			emit(Instruction(Opcode::BZ, 0, 0));
			node->second()->accept(this);
			emit(Instruction(Opcode::Move, popReg(), a));
			program[end] = Instruction(Opcode::BZ, a, (int)program.size());
			pushReg(a);

		}
		else if (op == "or") {
			node->first()->accept(this);
			int a = popReg();
			auto end = program.size();
			emit(Instruction(Opcode::BNZ, 0, 0));
			node->second()->accept(this);
			emit(Instruction(Opcode::Move, popReg(), a));
			program[end] = Instruction(Opcode::BNZ, a, (int)program.size());
			pushReg(a);
		}
		else {
			node->first()->accept(this);
			node->second()->accept(this);
			int src1, src2;
			src2 = popReg();
			src1 = popReg();
			Opcode o;
			if (op == "+") {
				o = Opcode::Add;
			}
			else if (op == "-") {
				o = Opcode::Sub;
			}
			else if (op == "*") {
				o = Opcode::Mul;
			}
			else if (op == "/") {
				o = Opcode::Div;
			}
			else if (op == "%") {
				o = Opcode::Mod;
			}
			else if (op == ">") {
				o = Opcode::GT;
			}
			else if (op == ">=") {
				o = Opcode::GE;
			}
			else if (op == "<") {
				o = Opcode::LT;
			}
			else if (op == "<=") {
				o = Opcode::LE;
			}
			else if (op == "==") {
				o = Opcode::EQ;
			}
			else if (op == "~=") {
				o = Opcode::NE;
			}
			else if (op == "//") {
				o = Opcode::iDiv;
			}
			else {
				error("unsupported operator", node->getToken().line,
					node->getToken().col);
			}
			int dest = findReg();
			emit(Instruction(o, src1, src2, dest));
		}
	}

	void CodeGen::visit(Chunk* node) {
		callDepthStack.clear();
		callDepthStack.emplace_back(0);
		for (auto i = node->begin(); i != node->end(); i++) {
			auto n = *i;
			if (n->type() == Func().type()) {
				/*if (n->first()->type() == Identifier().type()) {
					createGlobal(n->first()->getToken());
				}*/
			}

		}
		for (auto i = node->begin(); i != node->end(); i++) {
			(*i)->accept(this);
			//regCheck();
			//forceBalanceReg();
		}
	}

	void CodeGen::visit(Block* node) {
		auto p = node->getParent();
		assert(p);
		bool flag = p->type() != Func().type();
		if (flag)
			pushScope();
		for (auto i = node->begin(); i != node->end(); i++) {
			//regCheck();
			(*i)->accept(this);
			//regCheck();
			//syncRegState();
			//TODO:syncRegState()
		}
		if (flag)
			popScope();
	}

	void CodeGen::visit(UnaryExpression* expr) {
		auto& op = expr->getToken();
		if (op.tok == "-") {
			expr->first()->accept(this);
			int r = popReg();
			emit(Instruction(Opcode::Neg, r, findReg()));
		}
		else if (op.tok == "not") {
			expr->first()->accept(this);
			int r = popReg();
			emit(Instruction(Opcode::Not, r, findReg()));
		}
		else if (op.tok == "#") {
			expr->first()->accept(this);
			int r = popReg();
			emit(Instruction(Opcode::Len, r, findReg()));
		}
	}


	void CodeGen::visit(For* f) {
		// for i = 0,100,1 do .. end
		auto var = f->first()->getToken();

		auto init = f->second();
		auto end = f->third();
		auto step = f->at(3);
		pushScope();
		createLocal(var);
		end->accept(this);
		int end_reg = reg.back();
		step->accept(this);
		int step_reg = reg.back();


		init->accept(this);
		int init_reg = popReg();
		emit(Instruction(Opcode::Move, init_reg, getLocalAddress(var)));
		int loopStart = program.size();
		emit(Instruction(Opcode::LE, getLocalAddress(var), end_reg, findReg()));
		int jmpIdx = program.size();
		int bzReg = popReg();
		emit(Instruction(Opcode::BZ, bzReg, 0));
		f->at(4)->accept(this);
		emit(Instruction(Opcode::Add, getLocalAddress(var), step_reg, getLocalAddress(var)));
		emit(Instruction(Opcode::BRC, 0, loopStart));
		program[jmpIdx] = Instruction(Opcode::BZ, bzReg, (int)program.size());
		for (int i = jmpIdx; i < program.size(); i++) {
			if (program[i].opcode == Opcode::Break) {
				program[i] = Instruction(Opcode::BRC, 0, (int)(program.size()));
			}
		}

		popScope();
		popReg();
		popReg();

	}
	/*

	for var_1, ..., var_n in explist do block end

	do
	  local _f, _s, _var = explist
	  while true do
		local var_1, ... , var_n = _f(_s, _var)
		_var = var_1
		if _var == nil then break end
		block
	  end
	end
	*/
	void CodeGen::visit(GenericFor* f) {
		pushScope();
		int level = locals.size();
		std::string _f = format("@_f{}", level),
			_s = format("@_s{}", level),
			_var = format("@_var{}", level);
		createLocal(_f);
		createLocal(_s);
		createLocal(_var);
		auto varList = static_cast<GenericForVarList*>(f->first());
		auto exprList = static_cast<GenericForExprList*>(f->second());
		auto block = f->third();
		for (int i = 0; i < varList->size(); i++) {
			exprList->at(i)->accept(this);
			emit(Instruction(Opcode::StoreRet, popReg(), i));
		}
		emit(Instruction(Opcode::LoadRet, 0, getLocalAddress(_f)));
		emit(Instruction(Opcode::LoadRet, 1, getLocalAddress(_s)));
		emit(Instruction(Opcode::LoadRet, 2, getLocalAddress(_var)));
		size_t loopStart = program.size();
		pushScope();
		for (auto i : *varList) {
			createLocal(i->getToken());
		}
		emit(Instruction(Opcode::Push, getLocalAddress(_s), 0));
		emit(Instruction(Opcode::Push, getLocalAddress(_var), 0));
		emit(Instruction(Opcode::fCall, getLocalAddress(_f), 2, 0));
		for (int i = varList->size() - 1; i >= 0; i--) {
			emit(Instruction(Opcode::LoadRet, i, getLocalAddress(varList->at(i)->getToken())));
		}
		emit(Instruction(Opcode::Move, getLocalAddress(varList->at(0)->getToken()), getLocalAddress(_var)));
		int jmpIdx = program.size();
		emit(Instruction(Opcode::BZ, 0, 0));
		block->accept(this);
		popScope();
		emit(Instruction(Opcode::BRC, 0, (int)loopStart));
		program[jmpIdx] = Instruction(Opcode::BZ, getLocalAddress(_var), (int)program.size());
		for (int i = jmpIdx; i < program.size(); i++) {
			if (program[i].opcode == Opcode::Break) {
				program[i] = Instruction(Opcode::BRC, 0, (int)(program.size()));
			}
		}
		popScope();
	}

	void CodeGen::visit(ExprList* list) {
		int i = findReg();
		emit(Instruction(Opcode::NewTable, i, (int)0));
		int idx = 1;
		for (auto iter = list->begin(); iter != list->end(); iter++) {
			auto& node = *iter;

			int key, v;
			node->accept(this);
			if (node->type() == KVPair().type()) {

			}
			else {
				emit(Instruction(Opcode::LoadInt, findReg(), idx));
			}
			key = popReg();
			v = popReg();
			emit(Instruction(Opcode::StoreValue, i, key, v));
			idx++;
		}
	}
	void CodeGen::genArgsAndPushSelf(Arg* arg, int r) {
		int argCount = arg->size();
		std::vector<int> vec;
		for (int i = 0; i < argCount; i++) {
			vec.push_back(findReg());
		}
		int idx = 0;
		for (auto iter = arg->begin(); iter != arg->end(); iter++) {
			auto& node = *iter;
			node->accept(this);
			int i = popReg();
			emit(Instruction(Opcode::Move, i, vec[idx]));
			//emit(Instruction(Opcode::Push, i, callDepthStack.back()));
			idx++;
		}
		emit(Instruction(Opcode::Push, r, 0));
		for (int i = 0; i < argCount; i++) {
			emit(Instruction(Opcode::Push, vec[i], 0)); //neat!
			popReg();
		}
	}
	void CodeGen::visit(Arg* arg) {
		int argCount = arg->size();
		std::vector<int> vec;
		for (int i = 0; i < argCount; i++) {
			vec.push_back(findReg());
		}
		int idx = 0;
		for (auto iter = arg->begin(); iter != arg->end(); iter++) {
			auto& node = *iter;
			node->accept(this);
			int i = popReg();
			emit(Instruction(Opcode::Move, i, vec[idx]));
			//emit(Instruction(Opcode::Push, i, callDepthStack.back()));
			idx++;
		}
		for (int i = 0; i < argCount; i++) {
			emit(Instruction(Opcode::Push, vec[i], 0)); //neat!
			popReg();
		}
	}

#define P(x) std::cout << (x)<<std::endl;

	void CodeGen::visit(Call* node) {
		callDepthStack[callDepthStack.size() - 1]++;
		auto arg = node->second();
		auto func = node->first();
		int c = 0;
		int funcReg;
		if (func->type() == Colon().type()) {

			auto idx = func->second();
			idx->accept(this);
			int a, b;
			b = popReg();
			a = popReg();
			emit(Instruction(Opcode::GetValue, a, b, findReg()));
			c = 1;
			funcReg = reg.back();


			auto self = func->first();
			self->accept(this);
			int i = reg.back();
			genArgsAndPushSelf(dynamic_cast<Arg*>(arg), i);



		}
		else {
			func->accept(this);
			funcReg = reg.back();
			arg->accept(this);

		}
		int n = arg->size();
		emit(Instruction(Opcode::fCall, funcReg, n + c, 1));
		popReg();
		if (node->getParent()->type() == Block().type() || node->getParent()->type() == Chunk().type()) {

		}
		else {
			for (int i = 0; i < 1; i++) { //TODO: multiple returns
				int r = findReg();
				emit(Instruction(Opcode::LoadRet, i, r));
			}
		}
		callDepthStack[callDepthStack.size() - 1]--;
	}

	void CodeGen::visit(Index* idx) {
		auto t = idx->first();
		auto i = idx->second();
		t->accept(this);
		i->accept(this);
		int a, b;
		b = popReg();
		a = popReg();
		emit(Instruction(Opcode::GetValue, a, b, findReg()));
	}
	void CodeGen::visit(Repeat* node) {
		int loopStart = program.size();
		node->first()->accept(this);
		node->second()->accept(this);
		emit(Instruction(Opcode::BNZ, popReg(), (int)(program.size()) + 2));
		emit(Instruction(Opcode::BRC, 0, (int)loopStart));
		for (int i = loopStart; i < program.size(); i++) {
			if (program[i].opcode == Opcode::Break) {
				program[i] = Instruction(Opcode::BRC, 0, (int)(program.size()));
			}
		}
	}
	void CodeGen::visit(WhileLoop* node) {
		int jmpIdx = program.size();
		node->first()->accept(this);
		int bzIdx = program.size();
		int cond = popReg();
		emit(Instruction(Opcode::BZ, 0, 0));
		node->second()->accept(this);
		emit(Instruction(Opcode::BRC, 0, jmpIdx));
		program[bzIdx] = Instruction(Opcode::BZ, cond, (int)(program.size()));
		for (int i = jmpIdx; i < program.size(); i++) {
			if (program[i].opcode == Opcode::Break) {
				program[i] = Instruction(Opcode::BRC, 0, (int)(program.size()));
			}
		}
	}

	void CodeGen::visit(Func* func) {
		callDepthStack.emplace_back(0);
		locals.incFuncLevel();
		int funcReg = findReg();
		auto regStackBackUp = reg;
		RegState backup = regState;
		regState.reset();
		reg.clear();
		pushScope();  locals.back().offset = 0;
		AST* arg, * body;
		int i;
		std::string funcName;
		if (func->size() == 3) {
			auto name = func->first();
			if (name->type() == Colon().type()) {
				createLocal(Token(Token::Type::Identifier, "self", -1, -1));
			}
			if (name->type() == Identifier().type()) {
				createGlobal(name->getToken());
				funcName = name->getToken().tok;
			}
			else if (name->type() == Local().type()) {
				auto f = name->first()->getToken();
				createLocal(f);
				funcName = f.tok;
			}
			else {
				auto c = name->first();
				auto m = name->second();
				funcName = format("{}.{}", c->getToken().tok, m->getToken().tok);
			}
			arg = func->second();
			i = arg->type() == Colon().type() ? 1 : 0;
			body = func->third();
		}
		else {
			assert(func->size() == 2);
			arg = func->first();
			body = func->second();
			i = 0;
			funcName = "lambda";
		}

		funcHelper(funcReg, arg, body, i, funcName);
		popScope();

		assign(func);

		regState = backup;
		reg = regStackBackUp;
		reg.push_back(funcReg);
		locals.decFuncLevel();

		callDepthStack.pop_back();
	}

	void CodeGen::funcHelper(int funcReg, AST* arg, AST* body, int i, const std::string& name) {

		arg->accept(this);
		auto jmpIdx = (unsigned int)program.size();
		emit(Instruction(Opcode::BRC, 0, 0));
		if (locals.getFuncLevel() >= 1) {
			emit(Instruction(Opcode::MakeUpvalue, 0, 0, 0));
		}
		body->accept(this);
		emit(Instruction(Opcode::Ret, 0, 0));
		auto end = (unsigned int)program.size();
		program[jmpIdx] = Instruction(Opcode::BRC, 0, (int)end);
		emit(Instruction(Opcode::MakeClosure, funcReg, (int)jmpIdx + 1));
		emit(Instruction(Opcode::SetArgCount, funcReg, arg->size() + i));
		addFuncInfo(jmpIdx + 1, name);
		reg.push_back(funcReg);
	}

	void CodeGen::visit(FuncArg* arg) {
		for (auto iter = arg->begin(); iter != arg->end(); iter++) {
			auto& node = *iter;
			createLocal(node->getToken());
		}
	}

	void CodeGen::visit(Return* ret) {
		if (ret->size() == 0) {
			emit(Instruction(Opcode::Ret, 0, 0));
		}
		else {
			for (int i = 0; i < ret->size(); i++) {
				ret->at(i)->accept(this);
				emit(Instruction(Opcode::StoreRet, popReg(), i));
			}
			emit(Instruction(Opcode::Ret, 0, 0));
		}
	}

	void CodeGen::visit(Cond* node) {
		std::vector<int> jmpVec;
		int cnt = 0;
		auto iter = node->begin();
		while (iter < node->end()) {
			if (cnt != node->size() - 1) {
				auto cond = *iter;
				iter++;
				cnt++;
				auto block = *iter;
				iter++;
				cnt++;
				cond->accept(this);
				int bzIdx = program.size();
				int r = popReg();
				emit(Instruction(Opcode::BZ, 0, 0));
				block->accept(this);
				jmpVec.push_back(program.size());
				emit(Instruction(Opcode::BRC, 0, 0));
				program[bzIdx] = Instruction(Opcode::BZ, r, (int)program.size());
			}
			else {
				auto block = *iter;
				block->accept(this);
				break;
			}
		}
		for (auto i : jmpVec) {
			program[i] = Instruction(Opcode::BRC, 0, (int)program.size());
		}
	}

	void CodeGen::visit(Native* node) {
		auto n = node->getToken().tok;
		auto iter = natives.find(n);
		if (iter != natives.end()) {
			emit(Instruction(Opcode::invoke, 0, iter->second));
		}
		else {
			error(std::string("unregistered native handler ").append(n),
				node->getToken().line, node->getToken().col);
		}
	}

	void CodeGen::visit(BoolConstant* boolConstant) {
		if (boolConstant->getToken().tok == "true") {
			emit(Instruction(Opcode::LoadBool, findReg(), 1));
		}
		else if (boolConstant->getToken().tok == "false") {
			emit(Instruction(Opcode::LoadBool, findReg(), 0));
		}
		else {
			emit(Instruction(Opcode::LoadNil, findReg(), 0));
		}
	}

	void CodeGen::error(const std::string& msg, int line, int col) {
		throw CompilerException(msg, line, col);
	}

	bool CodeGen::isGlobal(const std::string& var) {
		for (auto iter = locals.rbegin(); iter != locals.rend(); iter++) {
			auto& scope = *iter;
			if (scope.dict.find(var) != scope.dict.end()) {
				return false;
			}
		}
		return globals.dict.find(var) != globals.dict.end();
	}

	void CodeGen::createLocal(const std::string& var) {

		auto& dict = locals.back().dict;
		auto v = VarInfo(locals.back().offset + dict.size());
		//dict.insert(std::make_pair(var, v));
		dict[var] = v;
		/*syncRegState();*/
		regState.reg[v.addr] = false;
	}
	void CodeGen::createLocal(const Token& var) {
		if (locals.size() == 0) {
			error(std::string("local scope not initialized!"), var.line, var.col);
		}
		createLocal(var.tok);
	}

	int CodeGen::getGlobalAddress(const Token& var) {
		return getGlobal(var).addr;
	}

	void CodeGen::createGlobal(const Token& var) {
		const auto& n = var.tok;
		if (globals.dict.find(n) == globals.dict.end())
			globals.dict[n] = VarInfo(globals.dict.size());
	}

	int CodeGen::getLocalAddress(const Token& var) {
		return getLocal(var).addr;
	}
	int CodeGen::getLocalAddress(const std::string& var) {
		return getLocal(var).addr;
	}

	VarInfo& CodeGen::getLocal(const std::string& var) {
		int funcLevel = locals.rbegin()->functionLevel;
		for (auto iter = locals.rbegin(); iter != locals.rend(); iter++) {
			auto& scope = *iter;
			if (scope.dict.find(var) != scope.dict.end()) {
				if (scope.functionLevel != funcLevel && !scope.dict[var].captured) {
					auto& v = scope.dict[var];
					v.captured = true;
					//println("captured {}", var.tok);

				}
				return scope.dict[var];
			}
		}
		error("Internal Compiler Error", -1, -1);
	}
	VarInfo& CodeGen::getLocal(const Token& var) {
		int funcLevel = locals.rbegin()->functionLevel;
		for (auto iter = locals.rbegin(); iter != locals.rend(); iter++) {
			auto& scope = *iter;
			if (scope.dict.find(var.tok) != scope.dict.end()) {
				if (scope.functionLevel != funcLevel && !scope.dict[var.tok].captured) {
					auto& v = scope.dict[var.tok];
					v.captured = true;
					//println("captured {}", var.tok);

				}
				return scope.dict[var.tok];
			}
		}
		error(std::string("undefined variable ").append(var.tok), var.line,
			var.col);
	}

	VarInfo& CodeGen::getGlobal(const Token& var) {
		if (globals.dict.find(var.tok) == globals.dict.end()) {
			error(std::string("undefined variable ").append(var.tok), var.line,
				var.col);
		}
		else
			return globals.dict[var.tok];
	}

	void CodeGen::popScope() {
		if (locals.size() == 0) {
			throw std::runtime_error("local scopes are empty!");
		}
		for (auto& i : locals.back().dict) {
			auto& v = i.second;
			regState.free(v.addr);
			//assert(std::find(reg.begin(), reg.end(), v.addr) == reg.end());
		}
		locals.pop_back();
		if (!locals.empty()) {
			for (auto& i : locals.back().dict) {
				auto& v = i.second;
				if (v.captured && !v.loadedToUpValue) {
					emit(Instruction(Opcode::StoreUpvalue, v.addr, v.addr));
					v.loadedToUpValue = true;
					regState.free(v.addr);
				}

			}
		}

	}

	void CodeGen::pushScope() {
		if (locals.size() == 0) {
			Scope v;
			v.offset = 0;
			locals.push_back(v);
			locals.back().functionLevel = locals.getFuncLevel();
		}
		else {
			Scope v;
			v.offset = locals.back().offset + locals.back().dict.size();
			v.functionLevel = locals.getFuncLevel();
			locals.push_back(v);
		}
		//	syncRegState();
	}
	void CodeGen::regCheck() {
		std::set<int> set;
		for (auto& local : locals) {
			for (auto& i : local.dict) {
				set.insert(i.second.addr);
			}
		}
		for (int i = locals.back().offset + locals.back().dict.size(); i < REG_MAX; i++) {
			assert(!(!regState.reg[i] && set.find(i) == set.end()));
			assert(!(regState.reg[i] && set.find(i) != set.end()));

		}
	}
	void CodeGen::forceBalanceReg() {
		if (locals.size() > 0) {
			//	regCheck();
			regState.reset(locals.back().offset + locals.back().dict.size());
		}
		else
			regState.reset();
	}

	void CodeGen::print() {
		for (unsigned int i = 0; i < program.size(); i++) {
			std::cout << i << " " << program[i].str() << std::endl;
		}
	}

	bool CodeGen::hasVar(const std::string& s) {
		for (auto iter = locals.rbegin(); iter != locals.rend(); iter++) {
			auto& scope = *iter;
			if (scope.dict.find(s) != scope.dict.end())
				return true;
		}
		return globals.dict.find(s) != globals.dict.end();
	}


	void CodeGen::addLib(const std::string& s) {
		std::string src = s;
		src.append("={}");
		Scanner scan("", src);
		scan.scan();
		Parser p(scan);
		auto ast = p.parse();
		ast->link();
		ast->accept(this);
		p.free();
	}

	void CodeGen::addLibMethod(const std::string& lib, const std::string& m, int i) {
		if (i == -1)
			i = natives.size();
		auto func = new Func();
		auto index = new Index();
		std::string n = lib;
		n.append("_").append(m);
		index->add(new Identifier(Token(Token::Type::Identifier, lib, -1, -1)));
		index->add(new StringLiteral(Token(Token::Type::Identifier, m, -1, -1)));
		func->add(index);
		auto block = new Block();
		block->add(new Native(Token(Token::Type::Identifier, n, -1, -1)));
		func->add(new Arg());
		func->add(block);
		func->link();
		natives.insert(std::make_pair(n, i));
		func->accept(this);
		delete func;
	}

	void CodeGen::addNative(const std::string& s, int i) {
		if (i == -1)
			i = natives.size();
		auto func = new Func();
		func->add(new Identifier(Token(Token::Type::Identifier, s, -1, -1)));
		auto block = new Block();
		block->add(new Native(Token(Token::Type::Identifier, s, -1, -1)));
		func->add(new Arg());
		func->add(block);
		func->link();
		natives.insert(std::make_pair(s, i));
		func->accept(this);
		delete func;
	}

	void CodeGen::visit(Break*) {
		emit(Instruction(Opcode::Break, 0, 0, 0));
	}

	void CodeGen::defineSymbol(const std::string& s, int i) {
		Token t(Token::Type::Identifier, s, -1, -1);
		createGlobal(t);
		int r = findReg();
		emit(Instruction(Opcode::LoadInt, r, i));
		emit(Instruction(Opcode::StoreGlobal, r, getGlobalAddress(t)));
	}

	void CodeGen::addSourceInfo(const SourcePos& pos) {
		int i = (int)program.size();
		if (sourceMap.find(i) == sourceMap.end()) {
			sourceMap[i] = pos;
		}
	}

	void  CodeGen::addFuncInfo(int i, const std::string& name) {
		//println("added {} at {}\n", name, i);
		funcInfoMap[i] = name;
	}
	void CodeGen::pre(AST* ast) {
		addSourceInfo(ast->pos);
	}

	void CodeGen::visit(KVPair* pair) {
		pair->second()->accept(this);
		pair->first()->accept(this);
	}

	void CodeGen::visit(ParallelAssignEntry* entry) {

	}
	void CodeGen::visitRight(ParallelAssignEntry* entry) {
		for (auto i = 0; i < entry->size(); i++) {
			entry->at(i)->accept(this);
			emit(Instruction(Opcode::StoreRet, popReg(), i));
		}
	}
	void CodeGen::visitLeft(ParallelAssignEntry* entry) {
		for (int i = entry->size() - 1; i >= 0; i--) {
			emit(Instruction(Opcode::LoadRet, i, findReg()));
		}
		for (int i = 0; i < entry->size(); i++) {
			auto node = entry->at(i);
			if (node->type() == Identifier().type()) {
				auto& var = node->getToken();
				if (!hasVar(var.tok)) {
					createGlobal(var); // default globals
				}


				int rhs = popReg();
				if (isGlobal(node->getToken().tok)) {
					int addr = getGlobalAddress(node->getToken());
					emit(Instruction(Opcode::StoreGlobal, rhs, addr));
				}
				else {
					auto& v = getLocal(node->getToken());
					int addr = v.addr;
					if (v.captured) {
						emit(Instruction(Opcode::StoreUpvalue, popReg(), addr));
					}
					else {
						emit(Instruction(Opcode::Move, rhs, addr));
					}
				}
			}
			else if (node->type() == Index().type()
				|| node->type() == Colon().type()) {
				auto idx = node;
				auto t = idx->first();
				auto i = idx->second();
				t->accept(this);
				i->accept(this);
				int a, b;
				b = popReg();
				a = popReg();
				int rhs = popReg();
				emit(Instruction(Opcode::StoreValue, a, b, rhs));
			}
		}
	}
	void CodeGen::visit(ParallelAssign* assign) {
		visitRight(dynamic_cast<ParallelAssignEntry*>(assign->second()));
		visitLeft(dynamic_cast<ParallelAssignEntry*>(assign->first()));
	}
}
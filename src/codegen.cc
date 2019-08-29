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
					emit(Instruction(Opcode::StoreUpvalue, popReg(), addr));
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
	}

	void CodeGen::visit(Identifier* node) {
		auto& var = node->getToken();
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
		if (locals.size()) {
			createLocal(node->first()->getToken());
		}
		else {
			createGlobal(node->first()->getToken());
		}
		node->second()->accept(this);
		assign(node);
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
				if (n->first()->type() == Identifier().type()) {
					createGlobal(n->first()->getToken());
				}
			}

		}
		for (auto i = node->begin(); i != node->end(); i++) {
			(*i)->accept(this);
			syncRegState();
		}
	}

	void CodeGen::visit(Block* node) {
		auto p = node->getParent();
		assert(p);
		bool flag = p->type() != Func().type();
		if (flag)
			pushScope();
		for (auto i = node->begin(); i != node->end(); i++) {
			(*i)->accept(this);
			syncRegState();
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
		if (f->size() == 5) {
			auto step = (*(f->begin() + 3));
		}
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
		if (func->type() == Colon().type()) {
			auto self = func->first();
			self->accept(this);
			int i = reg.back();
			genArgsAndPushSelf(dynamic_cast<Arg*>(arg), i);
			auto idx = func->second();
			idx->accept(this);
			int a, b;
			b = popReg();
			a = popReg();
			emit(Instruction(Opcode::GetValue, a, b, findReg()));
			c = 1;
		}
		else {
			arg->accept(this);
			func->accept(this);
		}
		int n = arg->size();
		emit(Instruction(Opcode::fCall, popReg(), n + c, 1));
		for (int i = 0; i < 1; i++) { //TODO: multiple returns
			int r = findReg();
			emit(Instruction(Opcode::LoadRet, i, r));
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
		pushScope();
		AST* arg, * body;
		int i;
		if (func->size() == 3) {
			auto name = func->first();
			if (name->type() == Colon().type()) {
				createLocal(Token(Token::Type::Identifier, "self", -1, -1));
			}
			if (name->type() == Identifier().type()) {
				createGlobal(name->getToken());
				addFuncInfo(program.size(), name->getToken().tok);
			}
			else {
				auto c = name->first();
				auto m = name->second();
				addFuncInfo(program.size(), format("{}.{}", c->getToken().tok, m->getToken().tok));
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
		}
		funcHelper(arg, body, i);
		assign(func);
		locals.decFuncLevel();
		popScope();
		callDepthStack.pop_back();
	}

	void CodeGen::funcHelper(AST* arg, AST* body, int i) {
	
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
		emit(Instruction(Opcode::MakeClosure, findReg(), (int)jmpIdx + 1));
		emit(Instruction(Opcode::SetArgCount, reg.back(), arg->size() + i));
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
			emit(Instruction(Opcode::LoadInt, findReg(), 1));
		}
		else if (boolConstant->getToken().tok == "false") {
			emit(Instruction(Opcode::LoadInt, findReg(), 0));
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

	void CodeGen::createLocal(const Token& var) {
		if (locals.size() == 0) {
			error(std::string("local scope not initialized!"), var.line, var.col);
		}
		auto& dict = locals.back().dict;
		dict.insert(std::make_pair(var.tok, VarInfo(locals.back().offset + dict.size())));
		syncRegState();
	}

	int CodeGen::getGlobalAddress(const Token& var) {
		return getGlobal(var).addr;
	}

	void CodeGen::createGlobal(const Token& var) {
		auto n = var.tok;
		if (globals.dict.find(n) == globals.dict.end())
			globals.dict[n] = VarInfo(globals.dict.size());
	}

	int CodeGen::getLocalAddress(const Token& var) {
		return getLocal(var).addr;
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
		locals.pop_back();
		if (!locals.empty()) {
			for (auto& i : locals.back().dict) {
				auto& v = i.second;
				if (v.captured) {
					emit(Instruction(Opcode::StoreUpvalue, v.addr, v.addr));
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
		syncRegState();
	}

	void CodeGen::syncRegState() {
		if (locals.size() > 0)
			regState.reset(locals.back().offset + locals.back().dict.size());
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
		for (int i = entry->size() - 1; i >= 0; i--) {
			entry->at(i)->accept(this);
			emit(Instruction(Opcode::StoreRet, popReg(), i));
		}
	}
	void CodeGen::visitLeft(ParallelAssignEntry* entry) {
		for (auto i = 0; i < entry->size(); i++) {
			emit(Instruction(Opcode::LoadRet, i, findReg()));
		}
		for (int i = entry->size() - 1; i >= 0; i--) {
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
					auto& v = getLocal(node->first()->getToken());
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
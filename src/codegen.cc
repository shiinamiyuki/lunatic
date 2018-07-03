/*
 * codegen.cc
 *
 *  Created on: 2018Äê6ÔÂ22ÈÕ
 *      Author: XiaochunTong
 */

#include "codegen.h"
SPEKA_BEGIN

void CodeGen::visit(Number*node) {
	auto& tok = node->getToken().tok;
	std::istringstream outi(tok), outf(tok);
	int i;
	double f;
	outi >> i;
	outf >> f;
	int r = findReg();
	if (i == f) {
		emit(Instruction(Opcode::LoadInt, r, i));
	} else {
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
void CodeGen::visit(String*node) {
	auto s = node->getToken().tok;
	addString(s);
	emit(Instruction(Opcode::LoadStr, findReg(), strConst[s]));
}

void CodeGen::assign(AST*node, bool b) {

	auto first = node->first();
	if (first->type() == Identifier().type()) {
		if (isConst(node->first()->getToken()) && !b) {
			error(
					std::string("cannot assign constant object ").append(
							node->first()->getToken().tok),
					node->first()->getToken().line,
					node->first()->getToken().col);
		}
		int rhs = popReg();
		if (isGlobal(node->first()->getToken().tok)) {
			int addr = getGlobalAddress(node->first()->getToken());
			emit(Instruction(Opcode::StoreGlobal, rhs, addr));
		} else {
			int addr = getLocalAddress(node->first()->getToken());
			emit(Instruction(Opcode::Move, rhs, addr));
		}
	} else if (first->type() == Index().type()) {
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
	} else {
		int a = getLocalAddress(var);
		int r = findReg();
		emit(Instruction(Opcode::Move, a, r));
	}
}
void CodeGen::visit(Let*node) {
	if (locals.size()) {
		createLocal(node->first()->getToken());
	} else {
		createGlobal(node->first()->getToken());
	}
	node->second()->accept(this);
	assign(node);
}

void CodeGen::visit(Const*node) {
	if (locals.size()) {
		createLocal(node->first()->getToken(), true);
	} else {
		createGlobal(node->first()->getToken(), true);
	}
	node->second()->accept(this);
	assign(node, true);
}

void CodeGen::visit(BinaryExpression*node) {
	auto op = node->getToken().tok;
	if (op == "=") {
		node->second()->accept(this);
		assign(node);
	} else {
		node->first()->accept(this);
		node->second()->accept(this);
		int src1, src2;
		src2 = popReg();
		src1 = popReg();
		Opcode o;
		if (op == "+") {
			o = Opcode::Add;
		} else if (op == "-") {
			o = Opcode::Sub;
		} else if (op == "*") {
			o = Opcode::Mul;
		} else if (op == "/") {
			o = Opcode::Div;
		}  else if (op == "%") {
			o = Opcode::Mod;
		} else if (op == ">") {
			o = Opcode::GT;
		} else if (op == ">=") {
			o = Opcode::GE;
		} else if (op == "<") {
			o = Opcode::LT;
		} else if (op == "<=") {
			o = Opcode::LE;
		} else if (op == "==") {
			o = Opcode::EQ;
		} else if (op == "!=") {
			o = Opcode::NE;
		} else if (op == "and") {
			o = Opcode::And;
		} else if (op == "or") {
			o = Opcode::Or;
		} else {
			error("unsupported operator", node->getToken().line,
					node->getToken().col);
		}
		int dest = findReg();
		emit(Instruction(o, src1, src2, dest));
	}
}
void CodeGen::visit(Chunk*node) {
	for (auto i = node->begin(); i != node->end(); i++) {
		auto&node = *i;
		if (node->type() == Class().type()) {
			addClass(node->getToken().tok);
		}
	}
	for (auto i = node->begin(); i != node->end(); i++) {
		(*i)->accept(this);
	}
}

void CodeGen::visit(Block*node) {
	auto p = node->getParent();
	assert(p);
	bool flag = p->type() != Func().type();
	if (flag)
		pushScope();
	for (auto i = node->begin(); i != node->end(); i++) {
		(*i)->accept(this);
		//TODO:syncRegState()
	}
	if (flag)
		popScope();
}

void CodeGen::visit(UnaryExpression* expr) {
	auto& op = expr->getToken();
	if (op.tok == "new") {
		if (expr->first()->type() != Call().type()) {
			error(std::string("invalid 'new' syntax"), op.line, op.col);
		}
		auto call = expr->first();
		auto classname = call->first();
		auto arg = call->second();
		arg->accept(this);
		//get classname.new
		addString("new");
		classname->accept(this);
		int c = popReg();
		emit(Instruction(Opcode::Clone, c, findReg()));
		emit(Instruction(Opcode::PushSelf, reg.back(), 0));
		emit(Instruction(Opcode::LoadStr, findReg(), strConst["new"]));
		int s = popReg(); //new
		int r = reg.back();
		emit(Instruction(Opcode::GetValue, r, s, findReg()));
		int n = arg->size();
		emit(Instruction(Opcode::fCall, popReg(), n, 1));
	}else if(op.tok == "-"){
		expr->first()->accept(this);
		int r = popReg();
		emit(Instruction(Opcode::Neg,r,findReg()));
	}else if(op.tok == "not"){
		expr->first()->accept(this);
		int r = popReg();
		emit(Instruction(Opcode::Not,r,findReg()));
	}
}
void CodeGen::visit(ExprListList*list) {
	int i = findReg();
	emit(Instruction(Opcode::NewList, i, (int) 0));
	int idx = 0;
	for (auto iter = list->begin(); iter != list->end(); iter++) {
		auto& node = *iter;
		node->accept(this);
		auto v = popReg();
		emit(Instruction(Opcode::ListAppend, i, v));
		idx++;
	}
}

void CodeGen::visit(ExprList*list) {
	int i = findReg();
	emit(Instruction(Opcode::NewTable, i, (int) 0));
	int idx = 0;
	for (auto iter = list->begin(); iter != list->end(); iter++) {
		auto& node = *iter;
		node->accept(this);
		int key;
		emit(Instruction(Opcode::LoadInt, findReg(), idx));
		key = popReg();
		auto v = popReg();
		emit(Instruction(Opcode::StoreValue, i, key, v));
		idx++;
	}
}

void CodeGen::visit(Arg*arg) {
	for (auto iter = arg->begin(); iter != arg->end(); iter++) {
		auto& node = *iter;
		node->accept(this);
		int i = popReg();
		emit(Instruction(Opcode::Push, i, 0));
	}
}
#define P(x) std::cout << (x)<<std::endl;
void CodeGen::visit(Call*node) {
	auto arg = node->second();
	arg->accept(this);
	auto func = node->first();
	if (func->type() == Index().type()) {
		auto self = func->first();
		if (self->type() == Identifier().type()
				&& isClass(self->getToken().tok)) {
			emit(Instruction(Opcode::LoadSelf, findReg(), 0));
			emit(Instruction(Opcode::PushSelf, popReg(), 0));
			auto classname = self->getToken();
			emit(
					Instruction(Opcode::LoadGlobal, findReg(),
							getGlobalAddress(classname)));
		} else {
			self->accept(this);
			emit(Instruction(Opcode::PushSelf, reg.back(), 0));
		}
		auto idx = func->second();
		idx->accept(this);
		int a, b;
		b = popReg();
		a = popReg();
		emit(Instruction(Opcode::GetValue, a, b, findReg()));
	} else {
		func->accept(this);
		emit(Instruction(Opcode::PushNil, 0, 0));
	}
	int n = arg->size();
	emit(Instruction(Opcode::fCall, popReg(), n, 1));
	for (int i = 0; i < 1; i++) { //TODO: multiple returns
		int r = findReg();
		emit(Instruction(Opcode::LoadRet, i, r));
	}
}

void CodeGen::visit(Index*idx) {
	auto t = idx->first();
	auto i = idx->second();
	t->accept(this);
	i->accept(this);
	int a, b;
	b = popReg();
	a = popReg();
	emit(Instruction(Opcode::GetValue, a, b, findReg()));
}

void CodeGen::visit(WhileLoop*node) {
	int jmpIdx = program.size();
	node->first()->accept(this);
	int bzIdx = program.size();
	int cond = popReg();
	emit(Instruction(Opcode::BZ, 0, 0));
	node->second()->accept(this);
	emit(Instruction(Opcode::BRC, 0, jmpIdx));
	program[bzIdx] = Instruction(Opcode::BZ, cond, (int) (program.size()));
}
void CodeGen::visit(Class*node) {
	auto classname = node->getToken();
	//TODO: check classname is not defined
//	createGlobal(classname);
	int addr = getGlobalAddress(classname);
	int r = findReg(); //holds the class object
	emit(Instruction(Opcode::NewTable, r, (int) 0));
	auto parent = node->first()->getToken();

	if (classname.tok != "Object") {
		int p = getGlobalAddress(parent);
		emit(Instruction(Opcode::LoadGlobal, findReg(), p));
		emit(Instruction(Opcode::Clone, popReg(), r));
	}
	emit(Instruction(Opcode::StoreGlobal, r, addr));
	for (auto iter = node->begin() + 1; iter != node->end(); iter++) {
		auto&method = *iter;
		method->accept(this);
	}
	emit(Instruction(Opcode::LoadGlobal, r, addr));
	popReg(); // pop r
}

void CodeGen::visit(Self*) {
	emit(Instruction(Opcode::LoadSelf, findReg(), (int) 0));
}

void CodeGen::visit(Method*func) {
	pushScope();

	auto arg = func->first();
	auto body = func->second();
	arg->accept(this);
	int jmpIdx = program.size();
	emit(Instruction(Opcode::BRC, 0, 0));
	body->accept(this);
	emit(Instruction(Opcode::Ret, 0, 0));
	int end = program.size();
	program[jmpIdx] = Instruction(Opcode::BRC, 0, end);
	emit(Instruction(Opcode::MakeClosure, findReg(), jmpIdx + 1));

	if (locals.size() > 1) {
		//TODO: a lot
		throw std::runtime_error("closure not implemented");
	} else {
		auto name = func->getToken().tok;
		addString(name);
		emit(Instruction(Opcode::LoadStr, findReg(), strConst[name]));
		auto classname = func->getParent()->getToken();
		emit(
				Instruction(Opcode::LoadGlobal, findReg(),
						getGlobalAddress(classname)));
		int cls = popReg();
		int nm = popReg();
		int met = popReg();
		emit(Instruction(Opcode::StoreValue, cls, nm, met));
		emit(
				Instruction(Opcode::StoreGlobal, cls,
						getGlobalAddress(classname)));
	}
	popScope();
}

void CodeGen::visit(Func*func) {
	pushScope();
	auto name = func->getToken();
	if (!isGlobal(name.tok))
		createGlobal(name);
	auto arg = func->first();
	auto body = func->second();
	arg->accept(this);
	int jmpIdx = program.size();
	emit(Instruction(Opcode::BRC, 0, 0));
	body->accept(this);
	emit(Instruction(Opcode::Ret, 0, 0));
	int end = program.size();
	program[jmpIdx] = Instruction(Opcode::BRC, 0, end);
	emit(Instruction(Opcode::MakeClosure, findReg(), jmpIdx + 1));

	if (locals.size() > 1) {
		//TODO: a lot
		throw std::runtime_error("closure not implemented");
	} else {

		int addr = getGlobalAddress(name);
		emit(Instruction(Opcode::StoreGlobal, popReg(), addr));
	}
	popScope();
}

void CodeGen::visit(FuncArg*arg) {
	for (auto iter = arg->begin(); iter != arg->end(); iter++) {
		auto &node = *iter;
		createLocal(node->getToken());
	}
}
void CodeGen::visit(Return*ret) {
	if (ret->size() == 0) {
		emit(Instruction(Opcode::Ret, 0, 0));
	} else {
		ret->first()->accept(this);
		emit(Instruction(Opcode::StoreRet, popReg(), 0));
		emit(Instruction(Opcode::Ret, 0, 0));
	}
}
void CodeGen::visit(Cond*node) {
	auto cond = node->first();
	auto ifBlock = node->second();
	cond->accept(this);
	int r = popReg();
	int bzIdx = program.size();
	emit(Instruction(Opcode::BZ, r, 0));
	ifBlock->accept(this);
	if (node->size() == 3) {
		int jmpIdx = program.size();
		emit(Instruction(Opcode::BRC, 0, 0));
		program[bzIdx] = Instruction(Opcode::BZ, r, (int) program.size());
		node->third()->accept(this);
		program[jmpIdx] = Instruction(Opcode::BRC, 0, (int) program.size());
	} else {
		program[bzIdx] = Instruction(Opcode::BZ, r, (int) program.size());
	}
}

void CodeGen::visit(Native*node) {
	auto n = node->getToken().tok;
	auto iter = natives.find(n);
	if (iter != natives.end()) {
		emit(Instruction(Opcode::invoke, 0, iter->second));
	} else {
		error(std::string("unregistered native handler ").append(n),
				node->getToken().line, node->getToken().col);
	}
}

void CodeGen::visit(BoolConstant* boolConstant) {
	if (boolConstant->getToken().tok == "true") {
		emit(Instruction(Opcode::LoadInt, findReg(), 1));
	} else {
		emit(Instruction(Opcode::LoadInt, findReg(), 0));
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
	return global.dict.find(var) != global.dict.end();
}

void CodeGen::createLocal(const Token& var, bool c) {
	if (locals.size() == 0) {
		error(std::string("local scope not initialized!"), var.line, var.col);
	}
	auto & dict = locals.back().dict;
	dict[var.tok] = VarInfo(locals.back().offset + dict.size(), c);
	syncRegState();
}

int CodeGen::getGlobalAddress(const Token& var) {
	return getGlobal(var).addr;
}

void CodeGen::createGlobal(const Token& var, bool c) {
	global.dict[var.tok] = VarInfo(global.dict.size(), c);
}
int CodeGen::getLocalAddress(const Token& var) {
	return getLocal(var).addr;
}

VarInfo& CodeGen::getLocal(const Token& var) {
	for (auto iter = locals.rbegin(); iter != locals.rend(); iter++) {
		auto& scope = *iter;
		if (scope.dict.find(var.tok) != scope.dict.end()) {
			return scope.dict[var.tok];
		}
	}
	error(std::string("undefined variable ").append(var.tok), var.line,
			var.col);

}

bool CodeGen::isConst(const Token& var) {
	VarInfo v;
	if (isGlobal(var.tok)) {
		v = getGlobal(var);
	} else {
		v = getLocal(var);
	}
	return v.isConst;
}

VarInfo& CodeGen::getGlobal(const Token& var) {
	if (global.dict.find(var.tok) == global.dict.end()) {
		error(std::string("undefined variable ").append(var.tok), var.line,
				var.col);
	} else
		return global.dict[var.tok];
}

void CodeGen::popScope() {
	if (locals.size() == 0) {
		throw std::runtime_error("local scopes are empty!");
	}
	locals.pop_back();
}
void CodeGen::pushScope() {
	if (locals.size() == 0) {
		Scope v;
		v.offset = 0;
		locals.push_back(v);
	} else {
		Scope v;
		v.offset = locals.back().offset + locals.back().dict.size();
		locals.push_back(v);
	}
	syncRegState();
}

void CodeGen::syncRegState() {
	regState.reset(locals.back().offset + locals.back().dict.size());
}
void CodeGen::print() {
	for (unsigned int i = 0; i < program.size(); i++) {
		std::cout << i << " " << program[i].str() << std::endl;
	}
}

void CodeGen::addClass(const std::string& s) {
	if (classSet.find(s) == classSet.end()) {
		classSet.insert(s);
		global.dict[s] = VarInfo(global.dict.size(), false);
	}
}

void CodeGen::addNative(const std::string& s, int i) {
	if (i == -1)
		i = natives.size();
	natives.insert(std::make_pair(s, i));
}
void CodeGen::visit(Import*import) {
	auto f = import->getToken().tok;
	std::string src;
	readFile(f.c_str(),src);
	Scanner s(src);
	s.scan();
	Parser p(s);
	auto ast = p.parse();
	ast->link();
	ast->accept(this);
//delete ast;
}

SPEKA_END


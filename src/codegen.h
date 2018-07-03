/*
 * codegen.h
 *
 *  Created on: 2018��6��22��
 *      Author: XiaochunTong
 */

#ifndef CODEGEN_H_
#define CODEGEN_H_
#include "ast.h"
#include "opcode.h"
#include "speka.h"
SPEKA_BEGIN
class AST;
class ScriptEngine;
class Visitor{
public:
	void visit(AST*){}
	virtual void visit(BinaryExpression*)=0;
	virtual void visit(Identifier*)=0;
	virtual void visit(Number*)=0;
	virtual void visit(String*)=0;
	virtual void visit(Chunk*)=0;
	virtual void visit(Block*)=0;
	virtual void visit(UnaryExpression*)=0;
	virtual void visit(Arg*)=0;
	virtual void visit(Call*)=0;
	virtual void visit(Index*)=0;
	virtual void visit(Cond*)=0;
	virtual void visit(WhileLoop*)=0;
	virtual void visit(Return*)=0;
	virtual void visit(BoolConstant*)=0;
	virtual void visit(Func*)=0;
	virtual void visit(FuncArg*)=0;
	virtual void visit(Let*)=0;
	virtual void visit(Const*)=0;
	virtual void visit(Native*)=0;
	virtual void visit(ExprList*)=0;
	virtual void visit(ExprListList*)=0;
	virtual void visit(Class*)=0;
	virtual void visit(Self*)=0;
	virtual void visit(Method*)=0;
	virtual void visit(Import*)=0;
	virtual void visit(Empty*)=0;
	virtual ~Visitor(){}
};



struct RegState{
	bool reg[REG_MAX];
	RegState() {
		reset();
	}
	void reset(int k=0) {
		for(int i = 0;i<k;i++)
			reg[i] = false;
		for (int i = k; i < REG_MAX; i++)
			reg[i] = true;
	}
	int find() {
		for (int i = 0; i < REG_MAX; i++) {
			if (reg[i]) {
				reg[i] = false;
				return i;
			}
		}
		return -1;
	}
	void free(int i){
		reg[i] = true;
	}
	void set(int i){
		reg[i] = false;
	}
};

struct VarInfo{
	bool isConst;
	int addr;
	VarInfo(int i, bool c = false) {
		isConst = c;
		addr = i;
	}
	VarInfo() {
		addr = -1;
		isConst = false;
	}
};
typedef std::unordered_map<std::string,VarInfo> Dict;
struct Scope{
	int offset;
	Dict dict;
};
class CompilerException{
	std::string msg;
public:
	CompilerException(const std::string& message,int line,int col){
		std::ostringstream out;
		out <<"CompilerException: "<< message << " at line "<<line << ":"<<col<<std::endl;
		msg = out.str();
	}
	const char* what(){
		return msg.c_str();
	}

};
class CodeGen: public Visitor{
	std::vector<Scope> locals;
	Scope global;
	std::vector<int> reg;
	std::vector<Instruction> program;
	std::vector<std::string> strPool;
	std::unordered_map<std::string, int> natives;
	std::unordered_map<std::string, int> strConst;
	std::set<std::string> classSet;
	RegState regState;
	void visit(BinaryExpression*);
	void visit(Number*);
	void visit(Identifier*);
	void visit(Chunk*);
	void visit(Block*);
	void visit(UnaryExpression*);
	void visit(Arg*);
	void visit(Call*);
	void visit(Index*);
	void visit(WhileLoop*);
	void visit(Return*);
	void visit(BoolConstant*);
	void visit(Func*);
	void visit(FuncArg*);
	void visit(Let*);
	void visit(Const*);
	void visit(Cond*);
	void visit(Native*);
	void visit(String*);
	void visit(ExprList*);
	void visit(ExprListList*);
	void visit(Class*);
	void visit(Self*);
	void visit(Method*);
	void visit(Import*);
	void visit(Empty*){}
	void assign(AST*, bool b = false);
	int getLocalAddress(const Token&var);
	void createGlobal(const Token&var, bool isConst = false);
	void createLocal(const Token&var, bool isConst = false);
	bool isConst(const Token&var);
	void error(const std::string&msg, int line, int col);
	bool isGlobal(const std::string&var);
	int getGlobalAddress(const Token&var);
	void pushScope();
	void popScope();
	VarInfo& getLocal(const Token&var);
	VarInfo& getGlobal(const Token&var);
	void syncRegState();
	void addString(const std::string&);
	void addClass(const std::string&);
	bool isClass(const std::string&s){return classSet.find(s) != classSet.end();}
public:
	friend class ScriptEngine;

	void print();
	void emit(const Instruction&i) {
		program.push_back(i);
	}
	const std::vector<Instruction>&getProgram() const {
		return program;
	}
	const std::vector<std::string>& getStringPool()const{
		return strPool;
	}
	int popReg() {
		if(reg.size() ==0){
			throw std::runtime_error("reg underflow");
		}
		int i = reg.back();
		reg.pop_back();
		regState.free(i);
		//td::cout<< "pop reg "<< i << std::endl;
		return i;
	}
	int findReg() {
		int i = regState.find();
		//std::cout<< "found reg "<< i << std::endl;
		reg.push_back(i);
		return i;
	}
	void pushReg(int i){
		reg.push_back(i);
	}
	void addNative(const std::string&,int i = -1);
};

SPEKA_END

#endif /* CODEGEN_H_ */

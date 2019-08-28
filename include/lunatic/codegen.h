#pragma once
#include "common.h"
#include "ast.h"
#include "opcode.h"
#include "visitor.h"

namespace lunatic {
	class AST;

	class ScriptEngine;
	
	struct RegState {
		bool reg[REG_MAX];

		RegState() {
			reset();
		}

		void reset(int k = 0) {
			for (int i = 0; i < k; i++)
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

		void free(int i) {
			reg[i] = true;
		}

		void set(int i) {
			reg[i] = false;
		}
	};

	struct VarInfo {
		int addr;
		bool captured;
		VarInfo(int i) {
			addr = i;
			captured = false;
		}

		VarInfo() {
			addr = -1;
		}
	};

	typedef std::unordered_map<std::string, VarInfo> Dict;
	struct Scope {
		int offset;
		int functionLevel;
		Dict dict;
	};

	class CompilerException {
		std::string msg;
	public:
		CompilerException(const std::string& message, int line, int col) {
			std::ostringstream out;
			out << "CompilerException: " << message << " at line " << line << ":"
				<< col << std::endl;
			msg = out.str();
		}

		const char* what() {
			return msg.c_str();
		}

	};

	class SymbolTable : public std::vector<Scope> {
		int funcLevel;
	public:
		SymbolTable() : funcLevel(0) {

		}
		int getFuncLevel()const { return funcLevel; }
		void incFuncLevel() { funcLevel++; }

		void decFuncLevel() {
			funcLevel--;
			assert(funcLevel >= 0);
		}
	};

	typedef std::unordered_map<int, SourcePos> SourceMap;

	class CodeGen : public Visitor {
		SymbolTable locals;
		Scope globals;
		std::vector<int> reg;
		std::vector<Instruction> program;
		std::vector<std::string> strPool;
		std::unordered_map<std::string, int> natives;
		std::unordered_map<std::string, int> strConst;
		std::set<std::string> classSet;
		std::vector<int> callDepthStack;
		RegState regState;
		SourceMap sourceMap;

		void addSourceInfo(const SourcePos&);

		void visit(BinaryExpression*) override;

		void visit(Number*) override;

		void visit(Identifier*) override;

		void visit(Chunk*) override;

		void visit(Block*) override;

		void visit(UnaryExpression*) override;

		void genArgsAndPushSelf(Arg*, int);
		void visit(Arg*) override;

		void visit(Call*) override;

		void visit(Index*) override;

		void visit(WhileLoop*) override;

		void visit(Return*) override;

		void visit(BoolConstant*) override;

		void visit(Func*) override;

		void visit(FuncArg*) override;

		void visit(Local*) override;

		void visit(Cond*) override;

		void visit(Native*) override;

		void visit(StringLiteral*) override;

		void visit(ExprList*) override;

		void visit(Empty*) override {}

		void visit(For*) override;

		void visit(Break*) override;

		void pre(AST*) override;

		void funcHelper(AST* arg, AST* body, int i);

		void assign(AST*, bool b = false);

		int getLocalAddress(const Token& var);

		void createGlobal(const Token& var);

		void createLocal(const Token& var);

		bool isConst(const Token& var);

		void error(const std::string& msg, int line, int col);

		bool isGlobal(const std::string& var);

		int getGlobalAddress(const Token& var);

		void pushScope();

		void popScope();

		VarInfo& getLocal(const Token& var);

		VarInfo& getGlobal(const Token& var);

		void syncRegState();

		void addString(const std::string&);

		void addClass(const std::string&);

		bool isClass(const std::string& s) {
			return classSet.find(s) != classSet.end();
		}

		bool hasVar(const std::string& s);

		void defineSymbol(const std::string& s, int i);

	public:
		friend class ScriptEngine;

		SourcePos getSourcePos(int i) {
			int t = i;
			while (i > 0) {
				auto iter = sourceMap.find(i);
				if (iter != sourceMap.end())
					return iter->second;
				else {
					i--;
				}
			}
			i = t;
			while (i < program.size()) {
				auto iter = sourceMap.find(i);
				if (iter != sourceMap.end())
					return iter->second;
				else {
					i++;
				}
			}
			return SourcePos("", -1, -1);
		}

		void print();

		void emit(const Instruction& i) {
			program.push_back(i);
		}

		const std::vector<Instruction>& getProgram() const {
			return program;
		}

		const std::vector<std::string>& getStringPool() const {
			return strPool;
		}

		int popReg() {
			if (reg.size() == 0) {
				throw std::runtime_error("reg underflow");
			}
			int i = reg.back();
			reg.pop_back();
			if (locals.size() > 0u && i > locals.back().offset + locals.back().dict.size())
				regState.free(i);
			return i;
		}

		int findReg() {
			int i = regState.find();
			//std::cout<< "found reg "<< i << std::endl;
			reg.push_back(i);
			return i;
		}

		void pushReg(int i) {
			reg.push_back(i);
		}

		void addNative(const std::string&, int i = -1);

		void addLib(const std::string&);

		void addLibMethod(const std::string&, const std::string&, int i = -1);
	};

}
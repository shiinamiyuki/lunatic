#pragma once
#include "common.h"
#include "ast.h"
#include "opcode.h"
#include "visitor.h"
#include "format.h"
#include "parse.h"

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
			assert(false);
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
		bool loadedToUpValue = false;
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

		bool isAddressOfVar(int i) {
			for (auto iter = rbegin(); iter != rend() && iter->functionLevel == funcLevel; iter++) {
				for (auto& pair : iter->dict) {
					if (pair.second.addr == i) {
						return true;
					}
				}
			}
			return false;
		}
	};

	typedef std::unordered_map<int, SourcePos> SourceMap;
	typedef std::unordered_map<int, std::string> FuncInfoMap;

	class CodeGen : public Visitor {
		SymbolTable locals;
		Scope globals;
		std::vector<int> reg;
		std::vector<Instruction> program;
		std::vector<std::string> strPool;
		std::unordered_map<std::string, int> natives;
		std::unordered_map<std::string, int> strConst;
		std::vector<int> callDepthStack;
		RegState regState;
		SourceMap sourceMap;
		FuncInfoMap funcInfoMap;
		void addSourceInfo(const SourcePos&);

		void addFuncInfo(int i, const std::string& name);

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

		void visit(Repeat*)override;

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

		void visit(GenericFor*)override;

		void visit(Break*) override;

		void visitRight(ParallelAssignEntry*);

		void visitLeft(ParallelAssignEntry*);

		void visit(ParallelAssignEntry*) override;

		void visit(ParallelAssign*) override;

		void pre(AST*) override;

		void funcHelper(int funcReg, AST* arg, AST* body, int i, const std::string& name);

		void assign(AST*);

		int getLocalAddress(const Token& var);
		int getLocalAddress(const std::string&);
		void createGlobal(const Token& var);

		void createLocal(const std::string& var);
		void createLocal(const Token& var);

		void error(const std::string& msg, int line, int col);

		bool isGlobal(const std::string& var);

		int getGlobalAddress(const Token& var);

		void pushScope();

		void popScope();

		VarInfo& getLocal(const Token& var);
		VarInfo& getLocal(const std::string& var);

		VarInfo& getGlobal(const Token& var);

		void regCheck();
		void forceBalanceReg();

		void addString(const std::string&);

		bool hasVar(const std::string& s);

		void defineSymbol(const std::string& s, int i);

		void visit(KVPair*);

	public:
		friend class ScriptEngine;
		CodeGen() {
			pushScope();
		}
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

			if (i.opcode == Opcode::Move && i.getA() == i.getBx()) {
				return;
			}
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
				throw CompilerException("reg underflow", -1, -1);
			}
			int i = reg.back();
			reg.pop_back();
			/*if (locals.size() > 0u && i >= locals.back().offset + locals.back().dict.size())
				regState.free(i);
			else if (locals.empty()) {
				regState.free(i);
			}*/
			if (!locals.isAddressOfVar(i))
				regState.free(i);
			//regState.free(i);
			return i;
		}
		void popAllReg(){
			while(!reg.empty())popReg();
		}
		int findReg() {
			int i;
			do {
				i = regState.find();
				//std::cout<< "found reg "<< i << std::endl;
				//assert(std::find(reg.begin(),reg.end(), i)== reg.end());
			} while (std::find(reg.begin(), reg.end(), i) != reg.end());
			reg.push_back(i);
			return i;
		}

		void pushReg(int i) {
			assert(std::find(reg.begin(), reg.end(), i) == reg.end());
			reg.push_back(i);
			assert(regState.reg[i]);
			regState.set(i);
		}

		void freeReg(int i) {
			assert(!regState.reg[i]);
			assert(std::find(reg.begin(), reg.end(), i) == reg.end());
		}

		void addNative(const std::string&, int i = -1);

		void addLib(const std::string&);

		void addLibMethod(const std::string&, const std::string&, int i = -1);
	};

}
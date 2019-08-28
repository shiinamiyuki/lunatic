#include "ast.h"
#include "visitor.h"
namespace lunatic {
	AST::AST() {
	}

	void AST::linkRec() {
		for (auto i : children) {
			i->parent = this;
			i->linkRec();
		}
	}

	void AST::link() {
		parent = nullptr;
		linkRec();
	}

	std::string AST::str(int depth) const {
		std::string s = "";
		for (int i = 0; i < depth; i++)
			s.append("\t");
		s.append(type()).append(" ");
		s.append(content.tok).append("\n");
		for (auto i : children) {
			if (i) {
				s.append(i->str(depth + 1));
			}
		}
		return s;
	}

	void AST::accept(Visitor* vis) {
		vis->visit(this);
	}

#define AST_ACCEPT(classname)  void classname::accept(Visitor*vis){vis->pre(this);vis->visit(this);}

	AST_ACCEPT(BinaryExpression);
	AST_ACCEPT(Chunk);
	AST_ACCEPT(Identifier);
	AST_ACCEPT(Number);
	AST_ACCEPT(StringLiteral);
	AST_ACCEPT(UnaryExpression);
	AST_ACCEPT(Cond);
	AST_ACCEPT(WhileLoop);
	AST_ACCEPT(Call);
	AST_ACCEPT(Index);
	AST_ACCEPT(Block);
	AST_ACCEPT(Arg);
	AST_ACCEPT(Return);
	AST_ACCEPT(BoolConstant);
	AST_ACCEPT(Func);
	AST_ACCEPT(FuncArg);
	AST_ACCEPT(Local);
	AST_ACCEPT(Native);
	AST_ACCEPT(ExprList);
	AST_ACCEPT(Empty);
	AST_ACCEPT(Method);
	AST_ACCEPT(Colon);
	AST_ACCEPT(For);
	AST_ACCEPT(Break);
	AST_ACCEPT(KVPair);
}
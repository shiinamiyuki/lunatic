/*
 * ast.cc
 *
 *  Created on: 2018Äê6ÔÂ22ÈÕ
 *      Author: XiaochunTong
 */

#include "ast.h"
#include "codegen.h"

SPEKA_BEGIN
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

#define AST_ACCEPT(classname)  void classname::accept(Visitor*vis){vis->visit(this);}

AST_ACCEPT(BinaryExpression)
AST_ACCEPT(Chunk)
AST_ACCEPT(Identifier)
AST_ACCEPT(Number)
AST_ACCEPT(String)
AST_ACCEPT(UnaryExpression)
AST_ACCEPT(Cond)
AST_ACCEPT(WhileLoop)
AST_ACCEPT(Call)
AST_ACCEPT(Index)
AST_ACCEPT(Block)
AST_ACCEPT(Arg)
AST_ACCEPT(Return)
AST_ACCEPT(BoolConstant)
AST_ACCEPT(Func)
AST_ACCEPT(FuncArg)
AST_ACCEPT(Let)
AST_ACCEPT(Const)
AST_ACCEPT(Native)
AST_ACCEPT(ExprList)
AST_ACCEPT(Class)
AST_ACCEPT(Method)
AST_ACCEPT(Self)
AST_ACCEPT(ExprListList)
AST_ACCEPT(Import)
SPEKA_END




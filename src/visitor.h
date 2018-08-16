//
// Created by xiaoc on 2018/8/16.
//

#ifndef LUNATIC_VISITOR_H
#define LUNATIC_VISITOR_H

#include "ast.h"
namespace  lunatic {
    class Visitor {
    public:
        void visit(AST *) {
        }

        virtual void visit(BinaryExpression *) = 0;

        virtual void visit(Identifier *) = 0;

        virtual void visit(Number *) = 0;

        virtual void visit(String *) = 0;

        virtual void visit(Chunk *) = 0;

        virtual void visit(Block *) = 0;

        virtual void visit(UnaryExpression *) = 0;

        virtual void visit(Arg *) = 0;

        virtual void visit(Call *) = 0;

        virtual void visit(Index *) = 0;

        virtual void visit(Cond *) = 0;

        virtual void visit(WhileLoop *) = 0;

        virtual void visit(Return *) = 0;

        virtual void visit(BoolConstant *) = 0;

        virtual void visit(Func *) = 0;

        virtual void visit(FuncArg *) = 0;

        virtual void visit(Local *) = 0;

        virtual void visit(Const *) = 0;

        virtual void visit(Native *) = 0;

        virtual void visit(ExprList *) = 0;

        virtual void visit(ExprListList *) = 0;

        virtual void visit(Empty *) = 0;

        virtual void visit(For *) = 0;

        virtual void visit(Break *) = 0;

        virtual ~Visitor() {
        }
        virtual void pre(AST*){}
    };
}
#endif //LUNATIC_VISITOR_H

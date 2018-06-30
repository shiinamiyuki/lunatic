/*
 * ast.h
 *
 *  Created on: 2018Äê6ÔÂ20ÈÕ
 *      Author: XiaochunTong
 */

#ifndef AST_H_
#define AST_H_

#include "parse.h"
#include "speka.h"
SPEKA_BEGIN
struct Token;
struct Token {
	enum class Type {
		String, Number, Identifier,Keyword,Symbol, Terminator,Nil
	} type;
	std::string tok;
	int line;
	int col;
	Token(Type t, const std::string to, int l, int c);
	Token():tok(""),line(0),col(0),type(Type::Nil) {}
};
class Visitor;
class AST;
class AST {
protected:
	std::vector<AST*> children;
	Token content;
	AST* parent;
	virtual void linkRec();
public:
	AST();
	void setContent(const Token&t) {
		content = t;
	}
	virtual std::string str(int depth = 0) const;
	virtual const std::string type()const{return std::string();};
	inline AST* first() {
		return children.at(0);
	}
	inline AST* second() {
		return children.at(1);
	}
	inline AST* third() {
		return children.at(2);
	}
	void add(AST*t) {
		children.push_back(t);
	}
	inline int size() {
		return children.size();
	}
	const Token& getToken() {
		return content;
	}
	std::vector<AST*>::iterator begin() {
		return children.begin();
	}
	std::vector<AST*>::iterator end() {
		return children.end();
	}
	virtual ~AST(){}
	virtual void accept(Visitor* vis);
	virtual void link();
	AST* getParent()const{return parent;}
};
#define S(x) std::string(x)
class Chunk: public AST{
public:
	const std::string type() const {
			return S("Chunk");
		}
	void accept(Visitor*);
};
class BinaryExpression: public AST{
public:
	BinaryExpression(){}
	BinaryExpression(const Token&t){this->content = t;}
	const std::string type() const {
		return S("BinaryExpr");
	}
	void accept(Visitor*);
};
class UnaryExpression: public AST {
public:
	UnaryExpression() {
	}
	UnaryExpression(const Token&t) {
		this->content = t;
	}
	const std::string type() const {
		return S("UnaryExpr");
	}
	void accept(Visitor*);
};
class String : public AST{
public:
	String(const Token&t) {
		this->content = t;
	}
	String() {
	}
	const std::string type() const {
		return S("String");
	}
	void accept(Visitor*);
};
class Number: public AST {
public:
	Number(const Token&t) {
		this->content = t;
	}
	Number() {
	}
	const std::string type() const {
		return S("Number");
	}
	void accept(Visitor*);
};
class BoolConstant:public AST {
public:
	BoolConstant(const Token&t) {
		content = t;
	}
	BoolConstant() {
	}
	const std::string type() const {
		return S("BoolConst");
	}
	void accept(Visitor*);
};
class Identifier: public AST {
public:
	Identifier(const Token&t) {
		content = t;
	}
	Identifier() {
	}
	const std::string type() const {
		return S("Identifier");
	}
	void accept(Visitor*);
};
class Index: public AST {
public:
	const std::string type() const {
		return S("Index");
	}
	void accept(Visitor*);
};
class Call: public AST {
public:
	const std::string type() const {
		return S("Call");
	}
	void accept(Visitor*);
};
class ExprList : public AST{
public:
	const std::string type() const {
		return S("ExprList");
	}
	void accept(Visitor*);
};
class ExprListList : public AST{
public:
	const std::string type() const {
		return S("ExprListList");
	}
	void accept(Visitor*);
};
class Arg: public AST {
public:
	const std::string type() const {
		return S("Arg");
	}
	void accept(Visitor*);
};
class Block: public AST {
public:
	const std::string type() const {
		return S("Block");
	}
	void accept(Visitor*);
};
class Cond: public AST {
public:
	const std::string type() const {
		return S("Cond");
	}
	void accept(Visitor*);
};
class WhileLoop: public AST {
public:
	const std::string type() const {
		return S("WhileLoop");
	}
	void accept(Visitor*);
};
class Return: public AST {
public:
	const std::string type() const {
		return S("Return");
	}
	void accept(Visitor*);
};
class Func:public AST{
public:
	Func(const Token&t){content = t;}
	Func(){}
	const std::string type() const {
		return S("Func");
	}
	void accept(Visitor*);
};
class FuncArg:public AST{
public:
	FuncArg(){}
	const std::string type() const {
		return S("FuncArg");
	}
	void accept(Visitor*);
};
class Let:public AST{
public:
	Let(){}
	const std::string type() const {
			return S("Let");
		}
	void accept(Visitor*);
};
class Const:public AST{
public:
	Const(){}
	const std::string type() const {
			return S("Const");
		}
	void accept(Visitor*);
};
class Native:public AST{
public:
	Native(const Token&token){content = token;}
	const std::string type()const{
		return S("Native");
	}
	void accept(Visitor*);
};
class Class:public AST{
public:
	Class(){}
	Class(const Token&token){content = token;}
	const std::string type()const{
		return S("Class");
	}
	void accept(Visitor*);
};
class Method:public AST{
public:
	Method(const Token&token){content = token;}
	const std::string type()const{
		return S("Method");
	}
	void accept(Visitor*);
};
class Self: public AST{
public:
	const std::string type()const{
		return S("Self");
	}
	void accept(Visitor*);
};
class Import: public AST {
public:
	const std::string type() const {
		return S("Import");
	}
	void accept(Visitor*);
};
SPEKA_END
#endif /* AST_H_ */

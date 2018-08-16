/*
 * parse.h
 *
 *  Created on: 2018Äê6ÔÂ20ÈÕ
 *      Author: XiaochunTong
 */

#ifndef PARSE_H_
#define PARSE_H_
#include "ast.h"
#include "lunatic.h"
namespace lunatic{
class AST;
struct Token;
class Parser;
class Scanner {
    friend class Parser;
	int pos;
	int line, col;
	std::string source;
	std::vector<Token> tokenStream;
	char at(int idx);
	Token next();
	void consume();
	char cur();
	char peek();
	char peek2();
    char peek3();
	void skipspace();
	void skipcomment();
	int isComment();
	bool isKeyWord(const std::string&s);
	Token identifier();
	Token number();
	Token symbol();
	Token string();
    const char *filename;
public:
	Scanner(const char *filename,const std::string& s);
	void scan();
	std::vector<Token>& getTokenStream();
};
    struct SourcePos;
class Parser {
	std::vector<Token> tokenStream;
	std::unordered_map<std::string, int> opPrec;
	std::unordered_map<std::string, int> opAssoc; //1 for left 0 for right
	int pos;
	const char *filename;
	template<typename T,typename... Args>
	T* makeNode(Args...args) {
		auto  t=  new T(args...);
		t->pos = getPos();
		return t;
	}
    SourcePos getPos()const;
public:
	Parser(Scanner&);
	const Token& at(int idx) const;
	inline const Token& cur() const {
		return at(pos);
	}
	inline const Token& peek() const {
		return at(pos + 1);
	}
	void consume();
	inline bool hasNext() const {
		return pos  + 1< (int) tokenStream.size();
	}
	//all of the followings will call consume() themselves
	AST* parse();
	AST* parseBlock();
	AST* parseExpr(int lev = 0);
	AST* parsePostfixExpr();
	AST* parseAtom();
	AST* parseCall();
	AST* parseArg();
	AST* parseUnary();
	AST* parseCond();
	AST* parseWhile();
	AST* parseReturn();
	AST* parseNative();
	AST* parseStmt();
	AST* parseFunc();
	AST* parseFuncArg();
	AST* parseLocal();
	AST* parseConst();
	AST* parseExprList();
	AST* parseExprListList();
    AST* parseFor();
	void expect(const std::string&token);
	bool has(const std::string&token);
	void skip();


};
class ParserException{
	std::string msg;
public:
	ParserException(const std::string& message,int line,int col){
		std::ostringstream out;
		out <<"ParserException: "<< message << " at line "<<line << ":"<<col<<std::endl;
		msg = out.str();
	}
	const char* what(){
		return msg.c_str();
	}
};
}

#endif /* PARSE_H_ */

/*
 * parse.cc
 *
 *  Created on: 2018Äê6ÔÂ20ÈÕ
 *      Author: XiaochunTong
 */

#include "parse.h"
SPEKA_BEGIN

Token::Token(Type t, const std::string to, int l, int c) {
	type = t;
	line = l;
	col = c;
	tok = to;
	if (to.empty()) {
		throw std::runtime_error("token is empty");
	}
}

char Scanner::at(int idx) {
	if (idx >= source.size())
		return 0;
	else
		return source.at(idx);
}

void Scanner::consume() {
	//putchar(cur());
	if (cur() == '\n') {
		line++;
		col = 1;
	} else {
		if (cur() == '\t')
			col += 4;
		else
			col++;
	}
	pos++;
	if (pos > source.length() + 10)
		throw std::runtime_error("parser boom!");
}

char Scanner::cur() {
	return at(pos);
}

char Scanner::peek() {
	return at(pos + 1);
}
char Scanner::peek2() {
	return at(pos + 2);
}
bool isSpace(char c) {
	return c == ' ' || c == '\r' || c == '\t' || c == '\n';
}
void Scanner::skipspace() {
	skipcomment();
	while (isSpace(cur())) {
		consume();
		skipcomment();
	}
}

void Scanner::skipcomment() {
	int i = isComment();
	if (!i)
		return;
	if (i == 1 || i == 2) {
		while (cur() != '\n')
			consume();
	} else if (i == 3) {
		while (!(cur() == '*' && peek() == '/'))
			consume();
		consume();
		consume();
	}
}

int Scanner::isComment() {
	if (cur() == '#')
		return 1;
	if (cur() == '/' && peek() == '/') {
		return 2;
	}
	if (cur() == '/' && peek() == '*') {
		return 3;
	}
	return 0;
}

Scanner::Scanner(const std::string& s) {
	pos = 0;
	source = s;
	line = 1;
	col = 1;

}
bool isDigit(char c) {
	return c >= '0' && c <= '9';
}
bool isIden(char c) {
	return c == '_' || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}
static std::set<std::string> keywords = { "and", "break", "do", "else",
		"elseif", "end", "false", "for", "fn", "goto", "if", "in",
		"let","const", "nil", "not", "or", "repeat", "return", "then", "true",
		"until", "while" ,"self","new","finalize","import"};
static std::vector<std::set<std::string>> operators = { { "::=", ">>=", "<<=" },
		{ ":=", "!=", "<=", ">=", "==", "+=", "-=", "*=", "/=", "%=", "^=",
				"&&", "||", "<<", ">>", "..", "->", "~=", "..", "::" }, { "+",
				"-", "*", "/", "%", "&", "|", "^", "(", ")", "[", "]", "{", "}",
				",", "=", "\\", "<", ">", ".", ":" } };
static std::set<char> opChar = { '+', '-', '*', '/', '%', '^', '>', '<', '!',
		'=', '(', ')', '[', ']', '{', '}', '.', ':', ',', '\\', '&', '|', '~' };
Token Scanner::next() {

	if (cur() == ';') {
		consume();
		return Token(Token::Type::Terminator, ";", line, col);
	} else if (isdigit(cur())) { // numbers
		return number();
	} else if (isIden(cur())) {
		return identifier();
	} else if (opChar.find(cur()) != opChar.end()) {
		return symbol();
	} else if (cur() == '\"' || cur() == '\'') {
		return string();
	}
	throw std::runtime_error(std::string("unable to parse ") + cur());
}
void Scanner::scan() {
	try {
		skipspace();
		while (pos < source.length()) {
			tokenStream.push_back(next());
			skipspace();
		}
	} catch (std::runtime_error & e) {
		std::cerr << e.what() << std::endl;
	}
}

bool Scanner::isKeyWord(const std::string& s) {
	return keywords.find(s) != keywords.end();
}

Token Scanner::identifier() {
	std::string iden;
	while (isIden(cur()) || isdigit(cur())) {
		iden += cur();
		consume();
	}
	if(isKeyWord(iden))
		return Token(Token::Type::Keyword,iden,line,col);
	else
		return Token(Token::Type::Identifier, iden, line, col);
}

Token Scanner::number() {
	std::string number;
	if (cur() == '0' && peek() == 'x') {
		number += cur();
		number += peek();
		consume();
		consume();
		while (isdigit(cur()) || (cur() >= 'A' && cur() <= 'F')
				|| (cur() >= 'a' && cur() <= 'f')) {
			number += cur();
			consume();
		}
	} else if (cur() == '0' && peek() != 'x' && peek() != '.') {
		while (cur() >= '0' && cur() <= '7') {
			number += cur();
			consume();
		}
	} else {
		while (isdigit(cur())) {
			number += cur();
			consume();
		}
		if (cur() == '.') {
			number += cur();
			consume();
			while (isdigit(cur())) {
				number += cur();
				consume();
			}
		}
		if (cur() == 'e') {
			number += cur();
			if (peek() == '-') {
				consume();
				number += cur();
			}
			consume();
			while (isdigit(cur())) {
				number += cur();
				consume();
			}
		}
	}
	return Token(Token::Type::Number, number, line, col);
}

Token Scanner::symbol() {
	char p2 = peek2();
	char p = peek();
	std::string s1, s2, s3;
	s1 = cur();
	s2 = s1 + p;
	s3 = s2 + p2;
	//std::cout << s1 << " " << s2 << " " << s3 << " " << std::endl;
	//system("pause");
	if (operators[0].find(s3) != operators[0].end()) {
		Token t(Token::Type::Symbol, s3, line, col);
		consume();
		consume();
		consume();
		return t;
	} else if (operators[1].find(s2) != operators[1].end()) {
		Token t(Token::Type::Symbol, s2, line, col);
		consume();
		consume();
		return t;
	} else if (operators[2].find(s1) != operators[2].end()) {
		Token t(Token::Type::Symbol, s1, line, col);
		consume();
		return t;
	}
}

Token Scanner::string() {
	std::string s;
	//s += cur();
	char c = cur();
	consume();
	while (cur() != c) {
		if (cur() == '\\') {
			consume();
			if (cur() == '\\') {
				s += '\\';
			} else if (cur() == 'n') {
				s += '\n';
			} else if (cur() == '"') {
				s += '\"';
			} else if (cur() == '\'') {
				s += '\'';
			}
		} else
			s += cur();
		consume();
	}
	consume();
	//s += "\"";
	return Token(Token::Type::String, s, line, col);
}

std::vector<Token>& Scanner::getTokenStream() {
	return tokenStream;
}

Parser::Parser(Scanner & lex) {
	pos = -1;
	tokenStream = lex.getTokenStream();
	opPrec[":="] = 0;
	opPrec["::="] = 0;
	opPrec["="] = 0;
	opPrec["|"] = 1;
	opPrec["or"] = 1;
	opPrec["and"] = 2;
	opPrec["&"] = 2;
	opPrec[">="] = opPrec["<="] = opPrec[">"] = opPrec["<"] = opPrec["=="] = opPrec["~="] =
			opPrec["!="] = 3;
	opPrec["+"] = opPrec["-"] = opPrec[".."] = 4;
	opPrec["*"] = opPrec["/"] = opPrec["%"] = 5;
	opPrec["+="] = 0;
	opPrec["-="] = 0;
	opPrec["*="] = 0;
	opPrec["/="] = 0;
	opPrec["<<="] = 0;
	opPrec[">>="] = 0;
	opPrec["^="] = 0;
	opPrec["&="] = 0;
	opPrec["|="] = 0;
	opAssoc = {
		{	"+=",0},
		{	"-=",0},
		{	"*=",0},
		{	"/=",0},
		{	">>=",0},
		{	"<<=",0},
		{	"%=",0},
		{	"&=",0},
		{	"|=",0},
		{	":=",0},
		{	"::=",0},
		{	"=",0},
		{	".",1},
		{	"->",1},
		{	"+",1},
		{	"-",1},
		{	"*",1},
		{	"/",1},
		{	"!=",1},
		{	"==",1},
		{	"~=",1},
		{	">",1},
		{	">=",1},
		{	"<=",1},
		{	"<",1},
		{	"%",1},
		{	"and",1},
		{	"&",1},
		{	"or",1},
		{	"|",1}
	};
}

const Token& Parser::at(int idx) const {
	static Token nil=Token();
	if(idx>= this->tokenStream.size() || idx <0){
		return nil;
	}else{
		return tokenStream[idx];
	}
}
AST* Parser::parseClass() {
	expect("class");
	if (peek().type != Token::Type::Identifier) {
		throw ParserException(
				std::string("identifier expected in class definition"),
				peek().line, peek().col);
	}
	auto node = new Class(peek());
	consume();
	if(has(":")){
		consume();
		if (peek().type != Token::Type::Identifier) {
			throw ParserException(
					std::string("identifier expected in class definition"),
					peek().line, peek().col);
		}
		node->add(new Identifier(peek()));
		consume();
	}else{
		node->add(new Identifier(Token(Token::Type::Keyword,"Object",0,0)));
	}
	expect("{");
	while(hasNext() && !has("}")){
		node->add(parseMethod());
	}
	expect("}");
	return node;
}


AST* Parser::parse() {
	auto node = new Chunk();
	while(hasNext()){
		node->add(parseStmt());
		if(has(";"))consume();
	}
	return node;
}
AST* Parser::parseStmt() {
	if (has("if"))
		return (parseCond());
	else if (has("return")) {
		auto e =  (parseReturn());
		expect(";");
		return e;
	} else if (has("while")) {
		return (parseWhile());
	} else if (has("fn")) {
		return parseFunc();
	} else if (has("let")) {
		return parseLet();
	} else if (has("const")) {
		return parseConst();
	} else if (has("import")) {
		return parseImport();
	} else if (has("native")) {
		return parseNative();
	} else if (has("class")) {
		return parseClass();
	} else if (has(";")) {
		consume();
		return new Empty();
	} else {
		auto e = parseExpr(0);
		expect(";");
		return e;
	}
}
#define BLOCK_END (has("}"))
AST* Parser::parseBlock() {
	auto node = new Block();
	if(has("{")){
		consume();
		while (hasNext() && !BLOCK_END) {
			node->add(parseStmt());
		}
		expect("}");
	}else{
		node->add(parseStmt());
	}
	return node;
}

AST* Parser::parseExpr(int lev) {
	AST* result = parseUnary();
	while (hasNext()) {
		auto next = peek();
		if (opPrec.find(next.tok) == opPrec.end())
			break;
		if (opPrec[next.tok] >= lev) {
			consume();
			AST* rhs = parseExpr(opAssoc[next.tok] + opPrec[next.tok]);
			AST* op = new BinaryExpression(next);
			op->add(result);
			op->add(rhs);
			result = op;
		} else
			break;
	}
	return result;
}


AST* Parser::parseAtom() {
	auto& next = peek();
	if(next.type == Token::Type::Number){
		consume();
		return new Number(cur());
	}else if(next.type == Token::Type::Identifier){
		consume();
		return new Identifier(cur());
	} else if (next.type == Token::Type::Keyword
			&& (next.tok == "true" || next.tok == "false")) {
		consume();
		return new BoolConstant(cur());
	} else if (next.type == Token::Type::Keyword
			&& (next.tok == "self")) {
		consume();
		return new Self();
	}else if (next.type == Token::Type::String) {
		consume();
		return new String(cur());
	} else if (next.tok == "(") {
		consume();
		auto n =  parseExpr(0);
		expect(")");
		return n;
	} else if (next.tok == "{") {
		return parseExprList();
	} else if (next.tok == "[") {
		return parseExprListList();
	}  else {
		throw ParserException(std::string("illegal token: ").append(next.tok),
				next.line, next.col);
		return nullptr;
	}
}

AST* Parser::parseCall() {
	return nullptr;
}
AST* Parser::parseExprListList() {
	expect("[");
	auto list = new ExprListList();
	while (hasNext() && !has("]")) {
		list->add(parseExpr(0));
		if (has(","))
			consume();
	}
	expect("]");
	return list;
}


AST* Parser::parseExprList() {
	expect("{");
	auto list = new ExprList();
	while(hasNext()&&!has("}")){
		list->add(parseExpr(0));
		if(has(","))
			consume();
	}
	expect("}");
	return list;
}

AST* Parser::parseArg() {
	expect("(");
	auto arg = new Arg();
	while(hasNext()&&!has(")")){
		arg->add(parseExpr(0));
		if(has(","))
			consume();
	}
	expect(")");
	return arg;
}
#define RET_UNARY consume(); \
auto node = new UnaryExpression(cur()); \
node->add(parseUnary()); \
return node;

AST* Parser::parseUnary() {
	auto next = peek();
	if(next.type == Token::Type::Symbol){
		if(next.tok == "-"){
			RET_UNARY
		}
	}else if(next.type == Token::Type::Keyword){
		if(next.tok == "not"){
			RET_UNARY
		}
		if(next.tok == "new"){
			RET_UNARY
		}
	}
	return parsePostfixExpr();
}
AST* Parser::parsePostfixExpr() {
	auto node =  parseAtom();
	while(hasNext()&&(has("[") || has("(")|| has("."))){
		if(has("[")){
			auto index = new Index();
			index->add(node);
			consume();
			index->add(parseExpr(0));
			expect("]");
			node = index;
		}else if(has("(")){
			auto call = new Call();
			call->add(node);
			call->add(parseArg());
			node = call;
		} else {	//.
			consume();
			if (peek().type != Token::Type::Identifier
					&& !(peek().type == Token::Type::Keyword
							&& peek().tok == "new")) {
				throw ParserException(std::string("identifier expected"),
						peek().line, peek().col);
			}
			consume();
			auto index = new Index();
			index->add(node);
			index->add(new String(cur()));
			node = index;
		}
	}
	return node;
}

AST* Parser::parseCond() {
	expect("if");
	auto node = new Cond();
	node->add(parseExpr(0));
	node->add(parseBlock());
	if(has("else")){
		consume();
		node->add(parseBlock());
	}
	return node;
}

AST* Parser::parseReturn() {
	expect("return");
	if(peek().type == Token::Type::Terminator){
		return new Return();
	}else{
		auto node = new Return();
		node->add(parseExpr(0));
		return node;
	}
}

AST* Parser::parseNative() {
	expect("native");
	auto n = new Native(peek());
	consume();
	expect(";");
	return n;
}

void Parser::expect(const std::string& token) {
	if(peek().tok != token){
		std::string msg(token);
		msg.append(" expected ");
		throw ParserException(msg,peek().line,peek().col);
	}else{
		consume();
	}
}

bool Parser::has(const std::string&token) {
	return peek().tok == token;
}

AST* Parser::parseWhile() {
	expect("while");
	auto node = new WhileLoop();
	node->add(parseExpr(0));
	node->add(parseBlock());
	return node;
}

void Parser::consume() {
	pos++;
}

void Parser::skip() {
	while(peek().type == Token::Type::Terminator && peek().tok == "EOL")
		consume();
}
AST* Parser::parseMethod() {
	Method * m;
	if(has("new")){
		m = new Method(peek());
		consume();
	} else {
		expect("fn");
		if (peek().type == Token::Type::Identifier) {
			m = new Method(peek());
			consume();
		}else{
			throw ParserException(std::string("identifier or '(' expected after 'fn'"),peek().line,peek().col);
		}
	}
	m->add(parseFuncArg());
	m->add(parseBlock());
	return m;
}
AST* Parser::parseFunc() {
	expect("fn");
	Func* func = nullptr;
	if(peek().type == Token::Type::Identifier){
		func = new Func(peek());
		consume();
	}else if(has("(")){//lambda
		func = new Func();
	}else{
		std::string msg = "identifier or '(' expected after 'fn'";
		throw ParserException(msg,peek().line,peek().col);
	}
	func->add(parseFuncArg());
	func->add(parseBlock());
	return func;
}
AST* Parser::parseFuncArg() {
	FuncArg * arg = new FuncArg();
	expect("(");
	while (!has(")")) {
		auto atom = parseAtom();
		if(atom->getToken().type != Token::Type::Identifier){
			std::string msg = "identifier expected in function definition";
			throw ParserException(msg,atom->getToken().line,atom->getToken().col);
		}
		arg->add(atom);
		if (has(","))
			consume();
	}
	expect(")");
	return arg;
}
AST* Parser::parseLet() {
	expect("let");
	AST* let = new Let();
	let->add(parseAtom());
	if(let->first()->getToken().type != Token::Type::Identifier){
		auto& tok = let->first()->getToken();
		throw ParserException("identifier expected in let expression",tok.line,tok.col);
	}
	expect("=");
	let->add(parseExpr(1));
	expect(";");
	return let;
}

AST* Parser::parseConst() {
	expect("const");
	AST* c = new Const();
	c->add(parseAtom());
	if (c->first()->getToken().type != Token::Type::Identifier) {
		auto& tok = c->first()->getToken();
		throw ParserException("identifier expected in let expression", tok.line,
				tok.col);
	}
	expect("=");
	c->add(parseExpr(1));
	expect(";");
	return c;
}
AST* Parser::parseImport() {
	expect("import");
	auto n = new Import();
	n->setContent(peek());
	consume();
	expect(";");
	return n;
}

SPEKA_END


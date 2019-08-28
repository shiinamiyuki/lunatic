#include "parse.h"
namespace lunatic {
	Parser::Parser(Scanner& lex) {
		filename = lex.filename;
		pos = -1;
		tokenStream = lex.getTokenStream();
		opPrec["="] = 0;
		opPrec["|"] = 1;
		opPrec["or"] = 1;
		opPrec["and"] = 2;
		opPrec["&"] = 2;
		opPrec[">="] = opPrec["<="] = opPrec[">"] = opPrec["<"] = opPrec["=="] = opPrec["~="] =
			opPrec["!="] = 3;
		opPrec["+"] = opPrec["-"] = opPrec[".."] = 4;
		opPrec["*"] = opPrec["/"] = opPrec["//"] = opPrec["%"] = 5;
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
				{"=",   0},
				{".",   1},
				{"->",  1},
				{"+",   1},
				{"-",   1},
				{"*",   1},
				{"/",   1},
				{"//",  1},
				{"!=",  1},
				{"==",  1},
				{"~=",  1},
				{">",   1},
				{">=",  1},
				{"<=",  1},
				{"<",   1},
				{"%",   1},
				{"and", 1},
				{"&",   1},
				{"or",  1},
				{"|",   1}
		};
	}

	const Token& Parser::at(int idx) const {
		static Token nil = Token();
		if (idx >= this->tokenStream.size() || idx < 0) {
			return nil;
		}
		else {
			return tokenStream[idx];
		}
	}


	AST* Parser::parse() {
		auto node = makeNode<Chunk>();
		while (hasNext()) {
			node->add(parseStmt());
			if (has(";"))consume();
		}
		return node;
	}

	AST* Parser::parseStmt() {
		skip();
		if (has("if"))
			return (parseCond());
		else if (has("return")) {
			auto e = (parseReturn());
			return e;
		}
		else if (has("while")) {
			return (parseWhile());
		}
		else if (has("break")) {
			consume();
			return makeNode<Break>();
		}
		else if (has("for")) {
			return parseFor();
		}
		else if (has("function")) {
			return parseFunc();
		}
		else if (has("local")) {
			return parseLocal();
		}
		else if (has("native")) {
			return parseNative();
		}
		else if (has(";")) {
			consume();
			return makeNode<Empty>();
		}
		else {
			auto e = parseExpr(0);
			return e;
		}
	}

#define BLOCK_END (has("end")||has("else")||has("elseif"))

	AST* Parser::parseBlock() {
		skip();
		auto node = makeNode<Block>();
		while (hasNext() && !BLOCK_END) {
			node->add(parseStmt());
		}
		return node;
	}

	AST* Parser::parseExpr(int lev) {
		skip();
		AST* result = parseUnary();
		while (hasNext()) {
			auto next = peek();
			if (opPrec.find(next.tok) == opPrec.end())
				break;
			if (opPrec[next.tok] >= lev) {
				consume();
				AST* rhs = parseExpr(opAssoc[next.tok] + opPrec[next.tok]);
				AST* op = makeNode<BinaryExpression>(next);
				op->add(result);
				op->add(rhs);
				result = op;
			}
			else
				break;
		}
		return result;
	}


	AST* Parser::parseAtom() {
		skip();
		auto& next = peek();
		if (has("function")) {
			return parseFunc();
		}
		else if (next.type == Token::Type::Number) {
			consume();
			return makeNode<Number>(cur());
		}
		else if (next.type == Token::Type::Identifier) {
			consume();
			return makeNode<Identifier>(cur());
		}
		else if (next.type == Token::Type::Keyword
			&& (next.tok == "true" || next.tok == "false" || next.tok == "nil")) {
			consume();
			return makeNode<BoolConstant>(cur());
		}
		else if (next.type == Token::Type::String) {
			consume();
			return makeNode<StringLiteral>(cur());
		}
		else if (next.tok == "(") {
			consume();
			auto n = parseExpr(0);
			expect(")");
			return n;
		}
		else if (next.tok == "{") {
			return parseExprList();
		}
		else {
			throw ParserException(std::string("illegal token: ").append(next.tok),
				next.line, next.col);
			return nullptr;
		}
	}

	AST* Parser::parseCall() {
		return nullptr;
	}

	AST* Parser::parseFor() {
		skip();
		expect("for");
		auto f = makeNode<For>();
		f->add(parseAtom());
		expect("=");
		f->add(parseAtom());
		expect(",");
		f->add(parseAtom());
		if (has(",")) {
			consume();
			f->add(parseAtom());
		}
		expect("do");
		f->add(parseBlock());
		expect("end");
		return f;
	}


	AST* Parser::parseExprList() {
		skip();
		expect("{");
		auto list = makeNode<ExprList>();
		while (hasNext() && !has("}")) {
			list->add(parseExpr(0));
			if (has(","))
				consume();
		}
		expect("}");
		return list;
	}

	AST* Parser::parseArg() {
		skip();
		expect("(");
		auto arg = makeNode<Arg>();
		while (hasNext() && !has(")")) {
			arg->add(parseExpr(0));
			if (has(","))
				consume();
		}
		expect(")");
		return arg;
	}

#define RET_UNARY consume(); \
    auto node = makeNode<UnaryExpression>(cur()); \
    node->add(parseUnary()); \
    return node;

	AST* Parser::parseUnary() {
		skip();
		auto next = peek();
		if (next.type == Token::Type::Symbol) {
			if (next.tok == "-") {
				RET_UNARY
			}
			if (next.tok == "#") {
				RET_UNARY
			}
		}
		else if (next.type == Token::Type::Keyword) {
			if (next.tok == "not") {
				RET_UNARY
			}
			if (next.tok == "new") {
				RET_UNARY
			}
		}
		return parsePostfixExpr();
	}

	AST* Parser::parsePostfixExpr() {
		skip();
		auto node = parseAtom();
		while (hasNext() && (has("[") || has("(") || has(".") || has(":"))) {
			if (has("[")) {
				auto index = makeNode<Index>();
				index->add(node);
				consume();
				index->add(parseExpr(0));
				expect("]");
				node = index;
			}
			else if (has("(")) {
				auto call = makeNode<Call>();
				call->add(node);
				call->add(parseArg());
				node = call;
			}
			else if (has(".")) {    //.
				consume();
				if (peek().type != Token::Type::Identifier
					&& !(peek().type == Token::Type::Keyword
						&& peek().tok == "new")) {
					throw ParserException(std::string("identifier expected"),
						peek().line, peek().col);
				}
				consume();
				auto index = makeNode<Index>();
				index->add(node);
				index->add(makeNode<StringLiteral>(cur()));
				node = index;
			}
			else if (has(":")) {    //.
				consume();
				if (peek().type != Token::Type::Identifier
					&& !(peek().type == Token::Type::Keyword
						&& peek().tok == "new")) {
					throw ParserException(std::string("identifier expected"),
						peek().line, peek().col);
				}
				consume();
				auto index = makeNode<Colon>();
				index->add(node);
				index->add(makeNode<StringLiteral>(cur()));
				node = index;
			}
		}
		return node;
	}

	AST* Parser::parseCond() {
		skip();
		expect("if");
		auto node = makeNode<Cond>();
		node->add(parseExpr(0));
		expect("then");
		node->add(parseBlock());
		while (hasNext() && has("elseif")) {
			consume();
			node->add(parseExpr(0));
			expect("then");
			node->add(parseBlock());
		}
		if (has("else")) {
			consume();
			node->add(parseBlock());
		}
		expect("end");
		//  std::cout <<node->str()<<std::endl;
		return node;
	}

	AST* Parser::parseReturn() {
		expect("return");
		if (peek().type == Token::Type::Terminator) {
			return makeNode<Return>();
		}
		else {
			auto node = makeNode<Return>();
			node->add(parseExpr(0));
			return node;
		}
	}

	AST* Parser::parseNative() {
		skip();
		expect("native");
		auto n = makeNode<Native>(peek());
		consume();
		expect(";");
		return n;
	}

	void Parser::expect(const std::string& token) {
		skip();
		if (peek().tok != token) {
			std::string msg(token);
			msg.append(" expected ");
			throw ParserException(msg, peek().line, peek().col);
		}
		else {
			consume();
		}
	}

	bool Parser::has(const std::string& token) {
		skip();
		return peek().tok == token;
	}

	AST* Parser::parseWhile() {
		expect("while");
		skip();
		auto node = makeNode<WhileLoop>();
		node->add(parseExpr(0));
		expect("do");
		node->add(parseBlock());
		expect("end");
		return node;
	}

	void Parser::consume() {
		pos++;
	}

	void Parser::skip() {
		while (peek().type == Token::Type::Terminator && peek().tok == "EOL")
			consume();
	}

	AST* Parser::parseFunc() {
		expect("function");
		skip();
		Func* func = nullptr;
		if (peek().type == Token::Type::Identifier) {
			auto name = parseAtom();
			AST* f = name;
			if (has(":") || has(".")) {
				if (has(":")) {
					consume();
					consume();
					f = makeNode<Colon>();
					f->add(name);
					f->add(makeNode<StringLiteral>(cur()));
				}
				else {
					consume();
					consume();
					f = makeNode<Index>();
					f->add(name);
					f->add(makeNode<StringLiteral>(cur()));
				}
			}
			func = new Func();
			func->add(f);
		}
		else if (has("(")) {//lambda
			func = makeNode<Func>();
		}
		else {
			std::string msg = "identifier or '(' expected after 'fn'";
			throw ParserException(msg, peek().line, peek().col);
		}
		func->add(parseFuncArg());
		func->add(parseBlock());
		expect("end");
		return func;
	}

	AST* Parser::parseFuncArg() {
		skip();
		FuncArg* arg = makeNode<FuncArg>();
		expect("(");
		while (!has(")")) {
			auto atom = parseAtom();
			if (atom->getToken().type != Token::Type::Identifier) {
				std::string msg = "identifier expected in function definition";
				throw ParserException(msg, atom->getToken().line, atom->getToken().col);
			}
			arg->add(atom);
			if (has(","))
				consume();
		}
		expect(")");
		return arg;
	}

	AST* Parser::parseLocal() {
		expect("local");
		skip();
		AST* let = makeNode<Local>();
		let->add(parseAtom());
		if (let->first()->getToken().type != Token::Type::Identifier) {
			auto& tok = let->first()->getToken();
			throw ParserException("identifier expected in let expression", tok.line, tok.col);
		}
		expect("=");
		let->add(parseExpr(1));
		return let;
	}

	SourcePos Parser::getPos() const {
		return SourcePos(filename, peek().line, peek().col);
	}

}
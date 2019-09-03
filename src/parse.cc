#include "parse.h"
#include "format.h"
namespace lunatic {
	Parser::Parser(Scanner& lex) {
		filename = lex.filename;
		pos = -1;
		tokenStream = lex.getTokenStream();
		opPrec["="] = 0;
		opPrec[","] = 1;
		opPrec["|"] = 2;
		opPrec["or"] = 3;
		opPrec["and"] = 3;
		opPrec["&"] = 3;
		opPrec[">="] = opPrec["<="] = opPrec[">"] = opPrec["<"] = opPrec["=="] = opPrec["~="] =
			opPrec["!="] = 4;
		opPrec["+"] = opPrec["-"] = opPrec[".."] = 5;
		opPrec["*"] = opPrec["/"] = opPrec["//"] = opPrec["%"] = 6;
		opAssoc = {
				{"=",   0},
				{",",   1},
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
		else if (has("do")) {
			consume();
			auto b = parseBlock();
			expect("end");
			return b;
		}
		else if (has("while")) {
			return (parseWhile());
		}
		else if (has("repeat")) {
			return parseRepeat();
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
		else if (has("__native")) {
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

#define BLOCK_END (has("end")||has("else")||has("elseif")||has("until"))

	AST* Parser::parseBlock() {
		skip();
		auto node = makeNode<Block>();
		while (hasNext() && !BLOCK_END) {
			node->add(parseStmt());
		}
		return node;
	}
	std::vector<AST*> unrollComma(Expression* _e) {
		if (!dynamic_cast<BinaryExpression*>(_e)) {
			return std::vector<AST*>{ _e };
		}
		else {
			auto e = static_cast<BinaryExpression*>(_e);
			std::vector<AST*> result;
			if (!dynamic_cast<BinaryExpression*>(e->first())) {
				result.emplace_back(e->first());
			}
			else {
				auto t = std::move(unrollComma(dynamic_cast<BinaryExpression*>(e->first())));
				result.insert(result.end(), t.begin(), t.end());
			}
			result.emplace_back(e->second());
			return result;
		}
	}

	Expression* Parser::hackParallelAssign(Expression* _node) {
		auto node = static_cast<BinaryExpression*>(_node);
		if (_node->getToken().tok == "=" && node->first()->getToken().tok == ",") {
			auto& tok = node->second()->getToken();
			std::vector<AST*>left, right;
			if (node->second()->getToken().tok != ",") {
				right.emplace_back(node->second());
			}
			else {
				right = unrollComma(static_cast<BinaryExpression*>(node->second()));
			}
			left = unrollComma(static_cast<BinaryExpression*>(node->first()));

			auto assign = makeNode<ParallelAssign>();
			auto L = makeNode<ParallelAssignEntry>();
			auto R = makeNode<ParallelAssignEntry>();
			for (auto i : left) {
				L->add(i);
			}
			for (auto i : right) {
				R->add(i);
			}
			assign->add(L);
			assign->add(R);
			return assign;
		}
		return node;
	}
	Expression* Parser::parseExpr(int lev, int maxLev) {
		if (maxLev == -1) {
			maxLev = 6;
		}
		skip();
		Expression* result = parseUnary();
		while (hasNext()) {
			auto next = peek();
			if (opPrec.find(next.tok) == opPrec.end())
				break;
			if (opPrec[next.tok] >= lev && opPrec[next.tok] <= maxLev) {
				consume();
				Expression* rhs = parseExpr(opAssoc[next.tok] + opPrec[next.tok]);
				Expression* op = makeNode<BinaryExpression>(next);
				op->add(result);
				op->add(rhs);
				op = hackParallelAssign(op);
				result = op;
			}
			else
				break;
		}
		return result;
	}


	Expression* Parser::parseAtom() {
		skip();
		auto& next = peek();
		if (has("function")) {
			return parseLambda();
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
			auto n = parseExpr(1);
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

	Expression* Parser::parseCall() {
		return nullptr;
	}

	AST* Parser::parseFor() {
		skip();
		expect("for");

		if (at(pos + 2).tok == "=") {
			auto f = makeNode<For>();
			f->add(parseAtom());
			expect("=");
			f->add(parseExpr(2));
			expect(",");
			f->add(parseExpr(2));
			if (has(",")) {
				consume();
				f->add(parseExpr(2));
			}
			else {
				f->add(makeNode<Number>(Token(Token::Type::Number, "1", getPos().line, getPos().col)));
			}
			expect("do");
			f->add(parseBlock());
			expect("end");
			return f;
		}
		else {
			auto pos = getPos();
			auto f = makeNode<GenericFor>();
			auto vars = unrollComma(parseExpr(1, 1));
			auto var = makeNode<GenericForVarList>();
			for (auto i : vars) {
				var->add(i);
			}
			f->add(var);
			expect("in");
			auto exprs = unrollComma(parseExpr(1));
			auto expr = makeNode<GenericForExprList>();
			for (auto i : exprs) {
				expr->add(i);
			}
			f->add(expr);
			expect("do");
			f->add(parseBlock());
			expect("end");

			return f;
		}
	}


	Expression* Parser::parseExprList() {
		skip();
		expect("{");
		auto list = makeNode<ExprList>();
		while (hasNext() && !has("}")) {
			skip();
			if (at(pos + 2).tok == "=") {
				auto pair = makeNode<KVPair>();
				auto tok = at(pos + 1);
				if (tok.type != Token::Type::Identifier) {
					throw ParserException("Identifier expected", tok.line, tok.col);
				}
				consume();
				expect("=");
				auto key = makeNode<StringLiteral>(tok);
				auto value = parseExpr(2);
				pair->add(key);
				pair->add(value);
				list->add(pair);
			}
			else if (peek().tok == "[") {
				auto pair = makeNode<KVPair>();
				consume();
				auto key = parseExpr(2);
				expect("]");
				expect("=");
				auto value = parseExpr(2);
				pair->add(key);
				pair->add(value);
				list->add(pair);
			}
			else
				list->add(parseExpr(2));
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
			arg->add(parseExpr(2));
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

	Expression* Parser::parseUnary() {
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

	Expression* Parser::parsePostfixExpr() {
		skip();
		auto node = parseAtom();
		while (hasNext() && (has("[") || has("(") || has(".") || has(":"))) {
			if (has("[")) {
				auto index = makeNode<Index>();
				index->add(node);
				consume();
				index->add(parseExpr(2));
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
				if (peek().type != Token::Type::Identifier) {
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
			node->add(parseExpr(2));
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
			auto e = parseExpr(1);
			auto r = dynamic_cast<BinaryExpression*>(e);
			if (r && r->getToken().tok == ",") {
				auto v = unrollComma(r);
				for (auto i : v) {
					node->add(i);

				}
				return node;
			}
			node->add(e);
			return node;
		}
	}

	AST* Parser::parseNative() {
		skip();
		expect("__native");
		auto n = makeNode<Native>(peek());
		consume();
		expect(";");
		return n;
	}

	void Parser::expect(const std::string& token) {
		skip();
		if (peek().tok != token) {
			std::string msg = format("'{}' expected", token);
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
	AST* Parser::parseRepeat() {
		expect("repeat");
		skip();
		auto node = makeNode<Repeat>();
		node->add(parseBlock());
		expect("until");
		node->add(parseExpr(2));
		return node;
	}
	AST* Parser::parseWhile() {
		expect("while");
		skip();
		auto node = makeNode<WhileLoop>();
		node->add(parseExpr(2));
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
	Expression* Parser::parseLambda() {
		expect("function");
		skip();
		auto func = makeNode<Func>();
		func->add(parseFuncArg());
		func->add(parseBlock());
		expect("end");
		return func;
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
		if (peek().tok == "function") {
			auto func = parseFunc();
			Local* local = makeNode<Local>();
			local->add(func->first());
			func->first() = local;
			return func;
		}
		else {
			auto tok = cur();
			AST* local = makeNode<Local>();
			/*	local->add(parseAtom());
				if (local->first()->getToken().type != Token::Type::Identifier) {
					auto& tok = local->first()->getToken();
					throw ParserException("identifier expected in local expression", tok.line, tok.col);
				}
				if (has("=")) {
					consume();
					local->add(parseExpr(1));
				}*/
			auto assign = parseExpr();
			local->add(assign);
			if (!dynamic_cast<Identifier*>(assign) && !dynamic_cast<ParallelAssign*>(assign)
				&& !dynamic_cast<BinaryExpression*>(assign)) {
				throw ParserException("assignment or identifier expected after local", tok.line, tok.col);
			}
			if (dynamic_cast<BinaryExpression*>(assign) && dynamic_cast<BinaryExpression*>(assign)->getToken().tok != "=") {
				throw ParserException("assignment expected after local", tok.line, tok.col);
			}
			return local;
		}

	}

	SourcePos Parser::getPos() const {
		return SourcePos(filename, peek().line, peek().col);
	}

	void Parser::free() {
		for (auto i : pool) {
			delete i;
		}
		pool.clear();
	}

}
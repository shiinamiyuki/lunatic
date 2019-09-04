#include "lex.h"


namespace lunatic {
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
		}
		else {
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

	char Scanner::peek3() {
		return at(pos + 3);
	}

	bool isSpace(char c) {
		return c == ' ' || c == '\r' || c == '\t';
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
		if (i == 1) {
			while (cur() && cur() != '\n')
				consume();
		}
		else if (i == 2) {
			while (!(cur() == ']' && peek() == ']' && peek2() == '-' && peek3() == '-'))
				consume();
			consume();
			consume();
			consume();
			consume();
		}
	}

	int Scanner::isComment() {
		if (cur() == '-' && peek() == '-') {
			return 1;
		}
		if (cur() == '-' && peek() == '-' && peek2() == '[' && peek3() == '[') {
			return 2;
		}
		return 0;
	}

	Scanner::Scanner(const char* filename, const std::string& s) {
		pos = 0;
		source = s;
		line = 1;
		col = 1;
		this->filename = filename;

	}

	bool isDigit(char c) {
		return c >= '0' && c <= '9';
	}

	bool isIden(char c) {
		return c == '_' || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
	}
	static std::set<std::string> keywords = { "and", "break", "do", "else",
											"elseif", "end", "false", "for", "in", "function", "goto", "if", "nil", "not", "or", "repeat", "return", "then", "true",
											"until", "while", "new"};
	static std::vector<std::set<std::string>> operators = { {"::=", ">>=", "<<="},
														   {"<=",  ">=",  "==",
																			   "<<", ">>", "..", "~=", "..", "::", "//"},
														   {"+",   "-",   "*", "/",  "%",  "&",  "|",  "^",  "(",  ")", "[", "]", "{", "}",
																   ",", "=", "\\", "<", ">", ".", ":", "+", "#"} };
	static std::set<char> opChar = { '#', '+', '-', '*', '/', '%', '^', '>', '<', '!',
									'=', '(', ')', '[', ']', '{', '}', '.', ':', ',', '\\', '&', '|', '~' };

	Token Scanner::next() {
		if (cur() == ';') {
			consume();
			return Token(Token::Type::Terminator, ";", line, col);
		}
		else if (cur() == '\n') {
			consume();
			return Token(Token::Type::Terminator, "EOL", line, col);
		}
		else if (isdigit(cur())) { // numbers
			return number();
		}
		else if (isIden(cur())) {
			return identifier();
		}
		else if (opChar.find(cur()) != opChar.end()) {
			return symbol();
		}
		else if (cur() == '\"' || cur() == '\'') {
			return string();
		}
		std::ostringstream out;
		out << cur() << " " << (int)(cur()) << std::ends;
		throw std::runtime_error(std::string("unable to parse ").append(out.str()));
	}

	void Scanner::scan() {
		try {
			skipspace();
			while (pos < source.length() && cur()) {
				tokenStream.push_back(next());
				skipspace();
			}
		}
		catch (std::runtime_error& e) {
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
		if (isKeyWord(iden))
			return Token(Token::Type::Keyword, iden, line, col);
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
		}
		else if (cur() == '0' && peek() != 'x' && peek() != '.') {
			while (cur() >= '0' && cur() <= '7') {
				number += cur();
				consume();
			}
		}
		else {
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
		}
		else if (operators[1].find(s2) != operators[1].end()) {
			Token t(Token::Type::Symbol, s2, line, col);
			consume();
			consume();
			return t;
		}
		else if (operators[2].find(s1) != operators[2].end()) {
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
				}
				else if (cur() == 'n') {
					s += '\n';
				}
				else if (cur() == '"') {
					s += '\"';
				}
				else if (cur() == '\'') {
					s += '\'';
				}
			}
			else
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
}
#pragma once
#include <lunatic/common.h>

namespace lunatic {

	struct Token{
		enum class Type {
			String, Number, Identifier, Keyword, Symbol, Terminator, Nil
		} type;
		std::string tok;
		int line;
		int col;
		const char* filename;
		Token(Type t, const std::string to, int l, int c);

		Token() : tok(""), line(0), col(0), type(Type::Nil) {}
	};

	struct SourcePos {
		int line;
		int col;
		const char* filename;
		SourcePos() {
			line = col = -1;
			filename = "";
		}
		SourcePos(const char* f, int x, int y) {
			line = x;
			col = y;
			filename = f;
		}
	};

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
		bool isKeyWord(const std::string& s);
		Token identifier();
		Token number();
		Token symbol();
		Token string();
		const char* filename;
	public:
		Scanner(const char* filename, const std::string& s);
		void scan();
		std::vector<Token>& getTokenStream();
	};
} 
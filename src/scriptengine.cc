#include "scriptengine.h"
#include "format.h"
#include "lib.h"

void readFile(const char* filename, std::string& s) {
	FILE* f = fopen(filename, "r");
	if (!f) {
		s = "";
		std::cerr << "cannot open file " << filename << std::endl;
		return;
	}
	while (!feof(f)) {
		char c = fgetc(f);
		if (!c || c == EOF)break;
		s += c;
	}
}

namespace lunatic {
	void ScriptEngine::execString(const std::string& s, const char* filename) {
		compileString(s, filename);

	}

	void ScriptEngine::execFile(const std::string& s) {
		std::string source;
		readFile(s.c_str(), source);
		execString(source, s.c_str());
	}

	void ScriptEngine::compileString(const std::string& s, const char* filename) {
		auto len = gen.program.size();
		try {
			len = gen.program.size();
			Scanner scan(filename, s);
			scan.scan();
			Parser p(scan);
			auto ast = p.parse();
			ast->link();
			//std::cout <<ast->str()<<std::endl;
			ast->accept(&gen);
			//gen.print();
			vm.loadProgram(gen.getProgram());
			vm.loadStringPool(gen.getStringPool());
			vm.eval(&state);
		}
		catch (ParserException& e) {
			std::cerr << e.what() << std::endl;
			recover(len);
		}
		catch (CompilerException& e) {
			std::cerr << e.what() << std::endl;
			recover(len);
		}
		catch (std::runtime_error& e) {
			auto pc = vm.getCurrentState()->pc;
			auto pos = gen.getSourcePos(pc);
			fprintln(stderr, "\033[31merror: {} at {}:{}:{} with stack trace:{}\033[0m\n",
				e.what(),
				pos.filename, pos.line, pos.col,
				dumpStackTrace());

		}
	}

	void ScriptEngine::addNative(const std::string& s, NativeHandle f) {
		gen.addNative(s);
		vm.addNative(f);
	}

	void ScriptEngine::addLib(const std::string& s) {
		gen.addLib(s);
	}

	void ScriptEngine::addLibMethod(const std::string& lib,
		const std::string& m,
		NativeHandle f) {
		try {
			gen.addLibMethod(lib, m);
			vm.addNative(std::move(f));
		}
		catch (CompilerException& e) {
			std::cerr << e.what() << std::endl;
		}
	}

	void ScriptEngine::recover(int i) {
		while (gen.program.size() > i)
			gen.program.pop_back();
	}

#define addSym(sym) addSymbol(#sym,sym)
#define addFunc(func) addNative(#func, bind(#func,func))
	void ScriptEngine::loadLib() {
		addLib("list");
		addLib("string");
		addLib("math");
		addLib("table");
		addLib("file");
		addLib("glfw");
		addLib("gl");
		addLibMethod("file", "open", FileLib::open);
		addLibMethod("file", "read", FileLib::read);
		addLibMethod("file", "write", FileLib::write);
		addLibMethod("file", "close", FileLib::close);
		addLibMethod("table", "clone", TableLib::clone);
		bindLibMethod("math", "sqrt", (double(*)(double))::sqrt);
		bindLibMethod("math", "atan", (double(*)(double))::atan);
		bindLibMethod("math", "sin", (double(*)(double))std::sin);
		bindLibMethod("math", "cos", (double(*)(double))::cos);
		bindLibMethod("math", "tan", (double(*)(double))::tan);
		bindLibMethod("math", "asin", (double(*)(double))::asin);
		bindLibMethod("math", "acos", (double(*)(double))::acos);
		bindLibMethod("math", "log", (double(*)(double))::log);
		bindLibMethod("math", "log10", (double(*)(double))::log10);
		bindLibMethod("math", "pow", (double(*)(double,double))::pow);
		addNative("print", print);
		addNative("tonumber", tonumber);
		addNative("tostring", tostring);
		addNative("getmetatable", getmetatable);
		addNative("setmetatable", setmetatable);
		addNative("getline", _getline);
		addLibMethod("string", "char", StringLib::Char);
		addLibMethod("string", "byte", StringLib::byte);
		addLibMethod("string", "length", StringLib::length);
		addLibMethod("string", "sub", StringLib::sub);
		addFunc(rand);
		// addFunc(GetTickCount);

		addFunc(exit);
	}


	ScriptEngine::ScriptEngine() {
		loadLib();
	}


	void ScriptEngine::addSymbol(const std::string& s, int i) {
		gen.defineSymbol(s, i);
	}
	template<typename T>
	void ScriptEngine::bindLibMethod(const std::string& lib, const std::string& m, T f) {
		auto func = bind(m, f);
		addLibMethod(lib, m, func);
	}


	std::string ScriptEngine::dumpStackTrace() {
		std::string dump;
		auto state = vm.getCurrentState();
		auto stack = state->callStack;
		for (auto iter = stack.rbegin(); iter != stack.rend(); iter++) {
			auto pos = gen.getSourcePos(iter->pc - 1);
			dump.append(format("\n\tat {}:{}:{}", pos.filename, pos.line, pos.col));
		}
		dump.append("\n");
		return dump;
	}
}
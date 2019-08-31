#include "scriptengine.h"
#include "format.h"
#include "lib.h"
#include <cmath>
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
	Error ScriptEngine::execString(const std::string& s, const char* filename) {
		return compileAndRunString(s, filename);
	}

	Error ScriptEngine::execFile(const std::string& s) {
		std::string source;
		readFile(s.c_str(), source);
		return execString(source, s.c_str());
	}

	Error ScriptEngine::compileAndRunString(const std::string& s, const char* filename) {
		auto len = gen.program.size();
		try {
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
			vm.exec();
		}
		catch (ParserException& e) {
			recover(len);
			return Error(ErrorCode::ParserError, e.what());
		}
		catch (CompilerException& e) {
			recover(len);
			return Error(ErrorCode::CompilerError, e.what());
		}
		catch (std::runtime_error& e) {
			auto pc = vm.getCurrentState()->pc;
			auto pos = gen.getSourcePos(pc);
			auto msg = format("\033[31merror: {} at {}:{}:{} with stack trace:{}\033[0m",
				e.what(),
				pos.filename, pos.line, pos.col,
				dumpStackTrace());
			vm.getCurrentState()->reset();
			vm.getCurrentState()->pc = vm.program.size();
			return Error(ErrorCode::RuntimeError, msg);

		}
		return Error(ErrorCode::None);
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
		addLibMethod("file", "open", FileLib::open);
		addLibMethod("file", "read", FileLib::read);
		addLibMethod("file", "write", FileLib::write);
		addLibMethod("file", "close", FileLib::close);
		addLibMethod("table", "clone", TableLib::clone);
		bindLibMethod("math", "sqrt", (double(*)(double))std::sqrt);
		bindLibMethod("math", "atan", (double(*)(double))std::atan);
		bindLibMethod("math", "sin", (double(*)(double))std::sin);
		bindLibMethod("math", "cos", (double(*)(double))std::cos);
		bindLibMethod("math", "tan", (double(*)(double))std::tan);
		bindLibMethod("math", "asin", (double(*)(double))std::asin);
		bindLibMethod("math", "acos", (double(*)(double))std::cos);
		bindLibMethod("math", "log", (double(*)(double))std::log);
		bindLibMethod("math", "log10", (double(*)(double))std::log10);
		bindLibMethod("math", "pow", (double(*)(double, double))std::pow);
		addNative("print", print);
		addNative("tonumber", tonumber);
		addNative("tostring", tostring);
		addNative("getmetatable", getmetatable);
		addNative("setmetatable", setmetatable);
		addNative("getline", _getline);
		addNative("collectgarbage", collectGarbage);
		addNative("pcall", pCall);
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
			auto funcName = gen.funcInfoMap[iter->closure->getAddress()];
			dump.append(format("\n\t in  <{}> at {}:{}:{}", funcName, pos.filename, pos.line, pos.col));
		}
		dump.append("\n");
		return dump;
	}

	void ScriptEngine::addModule(const Module& module) {
		addLib(module.name());
		for (const auto& i : module.handles) {
			addLibMethod(module.name(), i.first, i.second);
		}
	}
}

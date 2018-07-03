/*
 * main.cc
 *
 *  Created on: 2018��6��21��
 *      Author: XiaochunTong
 */

#include "parse.h"
#include "ast.h"
#include "codegen.h"
#include "vm.h"
#include "speka.h"
#include "gc.h"
#include "value.h"
#include "table.h"
#include "Windows.h"
#include "scriptengine.h"
using namespace speka;
template<typename T>
void timing(T f) {
	int i = GetTickCount();
	f();
	i = GetTickCount() - i;
	std::cout << "time elapsed " << i << "ms" << std::endl;
}

class D {
public:
	D() {
		std::cout << "created" << std::endl;
	}
	~D() {
		std::cout << "destroyed" << std::endl;
	}
};

int test() {
	ScriptEngine engine;
	engine.execFile("test/test.spk");

}
void Interactive() {
	ScriptEngine engine;
	std::cout << "Loading std lib" << std::endl;
	std::string source = "import \"lang.spk\";\n";
	engine.execString(source);
	source.clear();
	std::cout << "Speka REPL" << std::endl;
	;
	while (!feof(stdin)) {
		std::cout << "<<";
		std::getline(std::cin, source);
		source.append(";");
		engine.execString(source);
		source.clear();
	}
}
int main(int argc, char**argv) {
	ScriptEngine engine;
	if (argc == 1) {
		Interactive();
	} else {
		for (int i = 1; i < argc; i++) {
			timing([&]() {
				engine.execFile(argv[i]);});
		}
	}
}


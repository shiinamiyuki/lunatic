/*
 * main.cc
 *
 *  Created on: 2018Äê6ÔÂ21ÈÕ
 *      Author: XiaochunTong
 */

#include "parse.h"
#include "ast.h"
#include "codegen.h"
#include "vm.h"
#include "lunatic.h"
#include "gc.h"
#include "value.h"
#include "table.h"
#include "Windows.h"
#include "scriptengine.h"

using namespace lunatic;
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
	return 0;
}
void Interactive() {
	ScriptEngine engine;
    std::string source;
    std::cout << "lunatic v0.1 REPL" << std::endl;
	while (!feof(stdin)) {
		std::cout << "< ";
		std::getline(std::cin, source);
		source.append(";");
		engine.execString(source,"stdin");
		source.clear();
	}
}
template<typename T>
void printType(){

}
template<>
void printType<int>(){
	std::cout<<"int"<<std::endl;
}
template< class Ret, class... Args>
void foo(Ret(*)(Args...)){
	printType<Ret>();
}
int main(int argc, char**argv) {
	glfwInit();
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


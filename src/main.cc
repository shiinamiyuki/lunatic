
#include "scriptengine.h"

using namespace lunatic;
template<typename T>
void timing(T f) {

	f();
	
}




void Interactive() {
	ScriptEngine engine;
	std::string source;
	std::cout << "lunatic v0.1 REPL" << std::endl;
	while (!feof(stdin)) {
		std::cout << "< ";
		std::getline(std::cin, source);
		source.append(";");
		engine.execString(source, "stdin");
		source.clear();
	}
}

int main(int argc, char** argv) {
	ScriptEngine engine;

	if (argc == 1) {
		Interactive();
	}
	else {
		for (int i = 1; i < argc; i++) {
			timing([&]() {
				engine.execFile(argv[i]); });
		}
	}
}
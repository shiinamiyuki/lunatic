
/*
 * scriptengine.cc
 *
 *  Created on: 2018Äê6ÔÂ28ÈÕ
 *      Author: xiaoc
 */

#include "scriptengine.h"
#include "lib.h"
void readFile(const char* filename,std::string&s){
	std::ifstream in(filename);
	while(!in.eof()){
		std::string temp;
		std::getline(in,temp);
		temp.append("\n");
		s.append(temp);
	}
}
SPEKA_BEGIN
void ScriptEngine::execString(const std::string& s) {
	compileString(s);
	vm.eval(&state);
}

void ScriptEngine::execFile(const std::string& s) {
	std::string source;
	readFile(s.c_str(),source);
	execString(source);
}

void ScriptEngine::compileString(const std::string& s) {
	int len = gen.program.size();
	try{
		int len = gen.program.size();
		Scanner scan(s);
		scan.scan();
		Parser p(scan);
		auto ast = p.parse();
		ast->link();
		//std::cout <<ast->str()<<std::endl;
		ast->accept(&gen);
		//gen.print();
		vm.loadProgram(gen.getProgram());
		vm.loadStringPool(gen.getStringPool());
	}catch(ParserException & e){
		std::cerr << e.what() <<std::endl;
		recover(len);
	}catch(CompilerException&e){
		std::cerr << e.what() << std::endl;
		recover(len);
	}
}

void ScriptEngine::addNative(const std::string& s, NativeHandle f) {
	gen.addNative(s);
	vm.addNative(f);
}
void ScriptEngine::recover(int i) {
	while(gen.program.size() >i)
		gen.program.pop_back();
}
void ScriptEngine::loadLib() {
	addNative("list_length",ListLength);
	addNative("print",print);
}

ScriptEngine::ScriptEngine() {
	loadLib();
}

SPEKA_END


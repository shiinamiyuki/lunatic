
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
	//	gen.print();
		vm.loadProgram(gen.getProgram());
		vm.loadStringPool(gen.getStringPool());
		vm.eval(&state);
	}catch(ParserException & e){
		std::cerr << e.what() <<std::endl;
		recover(len);
	}catch(CompilerException&e){
		std::cerr << e.what() << std::endl;
		recover(len);
	}catch(std::runtime_error &e){
		std::cerr<<e.what()<<std::endl;
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
	addNative("list_append", ListAppend);
	addNative("list_length", ListLength);
	addNative("str2list", StringtoList);
	addNative("list2str", ListtoString);
	addNative("math_sin",MathLib::sin);
	addNative("math_cos",MathLib::cos);
	addNative("math_tan", MathLib::tan);
	addNative("math_sqrt", MathLib::sqrt);
	addNative("test_run", run);
	/*
	addNative("gl_init",GLLib::init);
	addNative("gl_displayfunc",GLLib::setRenderFunc);
	addNative("gl_setwindowpos",GLLib::setWindowPos);
	addNative("gl_setwindowsize",GLLib::setWindowSize);
	addNative("gl_begin",GLLib::glBegin);
	addNative("gl_end",GLLib::glEnd);
	addNative("gl_color",GLLib::glColor);
	addNative("gl_vertex",GLLib::glVertex);*/
	addNative("print", print);
}

ScriptEngine::ScriptEngine() {
	loadLib();
}

SPEKA_END


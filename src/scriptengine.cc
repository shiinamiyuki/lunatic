
/*
 * scriptengine.cc
 *
 *  Created on: 2018Äê6ÔÂ28ÈÕ
 *      Author: xiaoc
 */

#include "scriptengine.h"
#include "lib.h"
void readFile(const char* filename,std::string&s){
    FILE * f= fopen(filename,"r");
    if(!f){
        s="";
        std::cerr<<"cannot open file "<<filename <<std::endl;
        return;
    }
    while(!feof(f)){
        char c = fgetc(f);
        if(!c||c==EOF)break;
        s += c;
        if(c=='\n'){
            s.append(";");
        }
	}
}
namespace lunatic{
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

void ScriptEngine::addLib(const std::string &s)
{
    gen.addLib(s);
}

void ScriptEngine::addLibMethod(const std::string & lib,
                                const std::string & m,
                                NativeHandle f)
{
    try{
    gen.addLibMethod(lib,m);
    vm.addNative(f);
    }catch(CompilerException&e){
        std::cerr<<e.what()<<std::endl;
    }
}
void ScriptEngine::recover(int i) {
	while(gen.program.size() >i)
		gen.program.pop_back();
}
#define SBIND(func) bind(#func,func)
void ScriptEngine::loadLib() {
    addLib("list");
    addLib("string");
    addLib("math");
    addLib("table");
    addLibMethod("list","append", ListAppend);
    addLibMethod("list","length", ListLength);
    addLibMethod("table","clone",TableLib::clone);
	addNative("str2list", StringtoList);
	addNative("list2str", ListtoString);
    bindLibMethod("math","sqrt",::sqrt);
    bindLibMethod("math","atan",::atan);
    bindLibMethod("math","sin",::sin);
    bindLibMethod("math", "cos",::cos);
    bindLibMethod("math", "tan",::tan);
    bindLibMethod("math", "asin",::asin);
    bindLibMethod("math", "acos",::acos);
    bindLibMethod("math", "log",::log);
    bindLibMethod("math", "log10",::log10);
	addNative("print", print);
    addNative("tonumber",tonumber);
    addNative("tostring",tostring);
}

ScriptEngine::ScriptEngine() {
	loadLib();
}
template<typename Ret>
inline NativeHandle ScriptEngine::bind(const std::string& name, Ret(*function))
{
	auto func = [=](VM*vm) {
		Ret t = function();
		Value v(t);
		vm->storeReturn(0,v);
	};
    return func;
}

template<typename Ret, typename Arg1>
inline NativeHandle ScriptEngine::bind(const std::string& name, Ret(*function)(Arg1))
{
	auto func = [=](VM*vm){
		auto arg0 = vm->getLocal(0);
		Arg1 val = checkValue<Arg1>(&arg0);
		Ret t = function(val);
		Value v(t);
		vm->storeReturn(0,v);
	};
    return func;
}
template<typename Ret,typename Arg1,typename Arg2>
inline	NativeHandle ScriptEngine::bind(const std::string&name,Ret(*function)(Arg1,Arg2)){
	auto func = [=](VM*vm){
			auto arg0 = vm->getLocal(0);
			auto arg1 = vm->getLocal(1);
			Arg1 val = checkValue<Arg1>(&arg0);
			Arg2 val2 = checkValue<Arg2>(&arg1);
			Ret t = function(val,val2);
			Value v(t);
			vm->storeReturn(0,v);
		};
        return func;
}
template<typename Ret, typename Arg1, typename Arg2, typename Arg3>
NativeHandle ScriptEngine::bind(const std::string&name, Ret (*function)(Arg1, Arg2, Arg3)) {
	auto func = [=](VM*vm) {
		auto arg0 = vm->getLocal(0);
		auto arg1 = vm->getLocal(1);
		auto arg2 = vm->getLocal(2);
		Arg1 val = checkValue<Arg1>(&arg0);
		Arg2 val2 = checkValue<Arg2>(&arg1);
		Arg3 val3 = checkValue<Arg3>(&arg2);
		Ret t = function(val,val2,val3);
		Value v(t);
		vm->storeReturn(0,v);
	};
    return func;
}
template<typename T>
inline T checkValue(const Value*)
{
	//do nothing
	return T();
}

template<>
inline int checkValue<int>(const Value*v)
{
	v->checkInt();
	return v->getInt();
}
template<>
inline double checkValue<double>(const Value*v)
{
	v->checkFloat();
	return v->getFloat();
}

template<>
inline float checkValue<float>(const Value*v)
{
	v->checkFloat();
    return v->getFloat();
}

template<typename T>
void ScriptEngine::bindLibMethod(const std::string & lib,  const std::string &m, T f)
{
    auto func = bind(m,f);
    addLibMethod(lib,m,func);
}

}


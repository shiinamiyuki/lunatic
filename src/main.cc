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
#include "speka.h"
#include "gc.h"
#include "value.h"
#include "table.h"
#include "Windows.h"
#include "scriptengine.h"
using namespace speka;
template< typename T>
void timing(T f){
	int i = GetTickCount();
	f();
	i = GetTickCount() - i;
	std::cout <<"time elapsed "<<i<<"ms"<<std::endl;
}

class D{
public:
	D(){
		std::cout <<"created"<<std::endl;
	}
	~D(){
		std::cout <<"destroyed"<<std::endl;
	}
};

int test(){
	ScriptEngine engine;
	engine.execFile("test/test.spk");

}
int main(int argc,char**argv){
	ScriptEngine engine;
	for(int i = 1;i<argc;i++){
		engine.execFile(argv[i]);
	}
}


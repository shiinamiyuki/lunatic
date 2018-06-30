/*
 * scriptengine.h
 *
 *  Created on: 2018Äê6ÔÂ28ÈÕ
 *      Author: xiaoc
 */

#ifndef SCRIPTENGINE_H_
#define SCRIPTENGINE_H_

#include "speka.h"
#include "codegen.h"
#include "vm.h"
SPEKA_BEGIN
class ScriptEngine
{
	CodeGen gen;
	VM vm;
	State state;
	void recover(int i);
	void loadLib();
public:
	ScriptEngine();
	void execString(const std::string&);
	void execFile(const std::string&);
	void compileString(const std::string&);
	void addNative(const std::string&,NativeHandle);
};

SPEKA_END



#endif /* SCRIPTENGINE_H_ */

/*
 * scriptengine.h
 *
 *  Created on: 2018Äê6ÔÂ28ÈÕ
 *      Author: xiaoc
 */

#ifndef SCRIPTENGINE_H_
#define SCRIPTENGINE_H_

#include "lunatic.h"
#include "codegen.h"
#include "vm.h"
namespace lunatic{
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
    void addLib(const std::string&);
    void addLibMethod(const std::string&,const std::string&,NativeHandle);
    template<typename T>
    void bindLibMethod(const std::string&,const std::string & ,T);
    template<typename Ret>
    NativeHandle bind(const std::string&,Ret(*));
    template<typename Ret,typename Arg1>
    NativeHandle bind(const std::string&,Ret(*)(Arg1));
    template<typename Ret,typename Arg1,typename Arg2>
    NativeHandle bind(const std::string&,Ret(*)(Arg1,Arg2));
    template<typename Ret,typename Arg1,typename Arg2,typename Arg3>
    NativeHandle bind(const std::string&,Ret(*)(Arg1,Arg2,Arg3));
};
template<typename T>
T checkValue(const Value*);
template<>
int checkValue<int>(const Value*);
template<>
float checkValue<float>(const Value*);
template<>
double checkValue<double>(const Value*);

}




#endif /* SCRIPTENGINE_H_ */

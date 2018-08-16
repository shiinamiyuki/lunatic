
/*
 * scriptengine.cc
 *
 *  Created on: 2018Äê6ÔÂ28ÈÕ
 *      Author: xiaoc
 */

#include "scriptengine.h"
#include "lib.h"
#include "format.h"

void readFile(const char *filename, std::string &s) {
    FILE *f = fopen(filename, "r");
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
    void ScriptEngine::execString(const std::string &s,const char  * filename) {
        compileString(s,filename);

    }

    void ScriptEngine::execFile(const std::string &s) {
        std::string source;
        readFile(s.c_str(), source);
        execString(source,s.c_str());
    }

    void ScriptEngine::compileString(const std::string & s,const char  * filename ) {
        auto len = gen.program.size();
        try {
            len = gen.program.size();
            Scanner scan(filename,s);
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
        } catch (ParserException &e) {
            std::cerr << e.what() << std::endl;
            recover(len);
        } catch (CompilerException &e) {
            std::cerr << e.what() << std::endl;
            recover(len);
        } catch (std::runtime_error &e) {
            auto pc = vm.getCurrentState()->pc;
            auto pos = gen.getSourcePos(pc);
            fprintln(stderr,"error: {} at {}:{}:{} with stack trace:{}",
                    e.what(),
                    pos.filename,pos.line,pos.col,
                    dumpStackTrace());
            recover(len);
        }
    }

    void ScriptEngine::addNative(const std::string &s, NativeHandle f) {
        gen.addNative(s);
        vm.addNative(f);
    }

    void ScriptEngine::addLib(const std::string &s) {
        gen.addLib(s);
    }

    void ScriptEngine::addLibMethod(const std::string &lib,
                                    const std::string &m,
                                    NativeHandle f) {
        try {
            gen.addLibMethod(lib, m);
            vm.addNative(std::move(f));
        } catch (CompilerException &e) {
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
        addLib("glfw");
        addLib("gl");
        addLibMethod("file", "open", FileLib::open);
        addLibMethod("file", "read", FileLib::read);
        addLibMethod("file", "write", FileLib::write);
        addLibMethod("file", "close", FileLib::close);
        addLibMethod("list", "append", ListAppend);
        addLibMethod("list", "length", ListLength);
        addLibMethod("table", "clone", TableLib::clone);
        addNative("str2list", StringtoList);
        addNative("list2str", ListtoString);
        bindLibMethod("math", "sqrt", ::sqrt);
        bindLibMethod("math", "atan", ::atan);
        bindLibMethod("math", "sin", ::sin);
        bindLibMethod("math", "cos", ::cos);
        bindLibMethod("math", "tan", ::tan);
        bindLibMethod("math", "asin", ::asin);
        bindLibMethod("math", "acos", ::acos);
        bindLibMethod("math", "log", ::log);
        bindLibMethod("math", "log10", ::log10);
        bindLibMethod("math", "pow", pow);
        addNative("print", print);
        addNative("tonumber", tonumber);
        addNative("tostring", tostring);
        addNative("getmetatable", getmetatable);
        addNative("setmetatable", setmetatable);
        addNative("getline", _getline);
        addLibMethod("string", "char", StringLib::Char);
        addLibMethod("string", "byte", StringLib::byte);
        addLibMethod("string", "length", StringLib::length);
        addLibMethod("glfw", "CreateWindow", GLFWLib::createWindow);
        bindLibMethod("glfw","MakeContextCurrent",glfwMakeContextCurrent);
        bindLibMethod("glfw","WindowShouldClose",glfwWindowShouldClose);
        bindLibMethod("glfw","Terminate",glfwTerminate);
        bindLibMethod("glfw","Init",glfwInit);
        bindLibMethod("gl","Clear",glClear);
        bindLibMethod("gl","Viewport",glViewport);
        bindLibMethod("gl","Begin",glBegin);
        bindLibMethod("gl","End",glEnd);
        bindLibMethod("gl","Scaled",glScaled);
        bindLibMethod("gl","Vertex3d",glVertex3d);
        bindLibMethod("gl","Color3d",glColor3d);
        bindLibMethod("glfw","SwapBuffers",glfwSwapBuffers);
        bindLibMethod("glfw","PollEvents",glfwPollEvents);
        addSym(GL_COLOR_BUFFER_BIT);
        addSym(GL_POINTS);
        addFunc(Sleep);
    }

    ScriptEngine::ScriptEngine() {
        loadLib();
    }
#if 0
    template<typename Ret>
    inline NativeHandle ScriptEngine::bind(const std::string &name, Ret(*function)) {
        auto func = [=](VM *vm) {
            Ret t = function();
            Value v(t);
            vm->storeReturn(0, v);
        };
        return func;
    }

    template<typename Ret, typename Arg1>
    inline NativeHandle ScriptEngine::bind(const std::string &name, Ret(*function)(Arg1)) {
        auto func = [=](VM *vm) {
            auto arg0 = vm->getLocal(0);
            Arg1 val = checkValue<Arg1>(&arg0);
            Ret t = function(val);
            Value v(t);
            vm->storeReturn(0, v);
        };
        return func;
    }

    template<typename Ret, typename Arg1, typename Arg2>
    inline NativeHandle ScriptEngine::bind(const std::string &name, Ret(*function)(Arg1, Arg2)) {
        auto func = [=](VM *vm) {
            auto arg0 = vm->getLocal(0);
            auto arg1 = vm->getLocal(1);
            Arg1 val = checkValue<Arg1>(&arg0);
            Arg2 val2 = checkValue<Arg2>(&arg1);
            Ret t = function(val, val2);
            Value v(t);
            vm->storeReturn(0, v);
        };
        return func;
    }

    template<typename Ret, typename Arg1, typename Arg2, typename Arg3>
    NativeHandle ScriptEngine::bind(const std::string &name, Ret (*function)(Arg1, Arg2, Arg3)) {
        auto func = [=](VM *vm) {
            auto arg0 = vm->getLocal(0);
            auto arg1 = vm->getLocal(1);
            auto arg2 = vm->getLocal(2);
            Arg1 val = checkValue<Arg1>(&arg0);
            Arg2 val2 = checkValue<Arg2>(&arg1);
            Arg3 val3 = checkValue<Arg3>(&arg2);
            Ret t = function(val, val2, val3);
            Value v(t);
            vm->storeReturn(0, v);
        };
        return func;
    }

#endif
    template<typename T>
    void ScriptEngine::bindLibMethod(const std::string &lib, const std::string &m, T f) {
        auto func = bind(m, f);
        addLibMethod(lib, m, func);
    }

    void ScriptEngine::addSymbol(const std::string &s, int i) {
        gen.defineSymbol(s, i);
    }

    std::string ScriptEngine::dumpStackTrace() {
        std::string dump;
        auto state = vm.getCurrentState();
        auto stack = state->callStack;
        for(auto iter = stack.rbegin();iter != stack.rend();iter++){
            auto pos = gen.getSourcePos(iter->pc - 1);
            dump.append(format("\n\tat {}:{}:{}",pos.filename,pos.line,pos.col));
        }
        return dump;
    }
}


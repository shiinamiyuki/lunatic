#pragma once
#include "common.h"
#include "codegen.h"
#include "vm.h"
#include <type_traits>
#include "error.h"


namespace lunatic {
	
	template<typename Ret>
	inline void handleReturnValue(VM* vm, std::function<Ret(void)> func, int i) {
		Value v;
		v.store(func());
		vm->storeReturn(i, v);
	}

	template<>
	inline void handleReturnValue<void>(VM* vm, std::function<void(void)> func, int i) {
		func();
	}

#if 1

	template<typename Ret, typename Arg0, typename... Args>
	struct CallDefer {
		Arg0 arg0;
		std::function<Ret(Arg0, Args...)> func;

		inline Ret operator()(Args... args) {
			return func(arg0, args...);
		}

		CallDefer(Arg0 arg0, std::function<Ret(Arg0, Args...)> func) {
			this->arg0 = arg0;
			this->func = func;
		}
	};

	template<typename Ret, typename Arg0, typename... Args>
	CallDefer<Ret, Arg0, Args...> MakeCallDefer(Arg0 arg0, std::function<Ret(Arg0, Args...)> func) {
		return CallDefer<Ret, Arg0, Args...>(arg0, func);
	}

#endif

	template<typename Ret>
	inline std::function<Ret(void)> bindHelper(VM* vm, int i, std::function<Ret(void)> func) {
		return func;
	}

	template<typename Ret, typename Arg0>
	inline std::function<Ret(void)> bindHelper(VM* vm, int i, std::function<Ret(Arg0)> func) {
		auto arg0 = vm->getLocal(i);
		Arg0 val = arg0.load<Arg0>();
		return std::bind(func, val);
	}

	template<typename Ret, typename Arg0, typename... Args>
	inline std::function<Ret(void)> bindHelper(VM* vm, int i, std::function<Ret(Arg0, Args...)> func) {
		auto arg0 = vm->getLocal(i);
		Arg0 val = arg0.load<Arg0>();
		std::function<Ret(Args...)> f = MakeCallDefer(val, func); // std::bind is rubbish
		return bindHelper(vm, i + 1, f);
	}

	template<typename Ret, typename... Args>
	inline std::function<Ret(Args...)> toStdFunction(Ret(*f)(Args... args)) {
		return std::function<Ret(Args...)>(f);
	}

	template<typename Ret, typename... Args>
	NativeHandle bind(const std::string&, Ret(*f)(Args... args)) {
		auto func = toStdFunction(f);
		auto handle = [=](const CallContext& ctx) {
			auto helper = bindHelper(ctx.vm, 0, func);
			handleReturnValue(ctx.vm, helper, 0);
		};
		return handle;
	}


	class ScriptEngine;
	class Module {
		friend class ScriptEngine;
		std::string _name;
		std::unordered_map<std::string, NativeHandle> handles;
	public:
		Module(const std::string& name) :_name(name) {}
		Module& add(const std::string& methodName, NativeHandle method) {
			handles[methodName] = method;
			return *this;
		}
		const std::string& name()const { return _name; }
	};

	class ScriptEngine {
		CodeGen gen;
		VM vm;
		State state;

		void recover(int i);

		void loadLib();

		std::string dumpStackTrace();

		Error compileAndRunString(const std::string&, const char* filename = "");

	public:
		ScriptEngine();

		Error execString(const std::string&, const char* filename = "");

		Error execFile(const std::string&);

		void addSymbol(const std::string&, int i);

		void addNative(const std::string&, NativeHandle);

		void addLib(const std::string&);

		void addLibMethod(const std::string&, const std::string&, NativeHandle);

		void addModule(const Module& module);

		template<typename T>
		void bindLibMethod(const std::string&, const std::string&, T);		

	};

}

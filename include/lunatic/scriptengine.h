#pragma once
#include "common.h"
#include "codegen.h"
#include "vm.h"
#include <type_traits>


namespace lunatic {
	template<typename T>
	inline T checkValue(const Value* v) {
		if (std::is_pointer<T>::value) {
			v->checkUserData();
			return static_cast<T>(v->getUserData());
		}
		return T();
	}

	template<>
	inline int checkValue<int>(const Value* v) {
		v->checkInt();
		return v->getInt();
	}

	template<>
	inline unsigned int checkValue<unsigned int>(const Value* v) {
		v->checkInt();
		return v->getInt();
	}

	template<>
	inline long unsigned int checkValue<long unsigned int>(const Value* v) {
		v->checkInt();
		return v->getInt();
	}

	template<>
	inline double checkValue<double>(const Value* v) {
		v->checkFloat();
		return v->getFloat();
	}

	template<>
	inline float checkValue<float>(const Value* v) {
		v->checkFloat();
		return (float)v->getFloat();
	}


	template<>
	inline const char* checkValue<const char*>(const Value* v) {
		v->checkString();
		return v->getString()->str().c_str();
	}

	
	inline void toValue(int i, Value & v){
		v.setInt(i);
	}

	inline void toValue(size_t i, Value & v){
		v.setInt(i);
	}
	inline void toValue(double f, Value & v){
		v.setFloat(f);
	}
	template<typename Ret>
	inline void handleReturnValue(VM* vm, std::function<Ret(void)> func, int i) {
		Value v;
		toValue(func(), v);
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
		Arg0 val = checkValue<Arg0>(&arg0);
		return std::bind(func, val);
	}

	template<typename Ret, typename Arg0, typename... Args>
	inline std::function<Ret(void)> bindHelper(VM* vm, int i, std::function<Ret(Arg0, Args...)> func) {
		auto arg0 = vm->getLocal(i);
		Arg0 val = checkValue<Arg0>(&arg0);
		std::function<Ret(Args...)> f = MakeCallDefer(val, func); // std::bind is rubbish
		return bindHelper(vm, i + 1, f);
	}

	template<typename Ret, typename... Args>
	inline std::function<Ret(Args...)> toStdFunction(Ret(*f)(Args... args)) {
		return std::function<Ret(Args...)>(f);
	}

	class ScriptEngine {
		CodeGen gen;
		VM vm;

		void recover(int i);

		void loadLib();

		std::string dumpStackTrace();

	public:
		ScriptEngine();

		void execString(const std::string&, const char* filename = "");

		void execFile(const std::string&);

		void compileString(const std::string&, const char* filename = "");

		void addSymbol(const std::string&, int i);

		void addNative(const std::string&, NativeHandle);

		void addLib(const std::string&);

		void addLibMethod(const std::string&, const std::string&, NativeHandle);

		template<typename T>
		void bindLibMethod(const std::string&, const std::string&, T);

		template<typename Ret, typename... Args>
		NativeHandle bind(const std::string&, Ret(*f)(Args... args)) {
			auto func = toStdFunction(f);
			auto handle = [=](VM* vm) {
				auto helper = bindHelper(vm, 0, func);
				handleReturnValue(vm, helper, 0);
			};
			return handle;
		}

	};

	template<typename T>
	T checkValue(const Value*);


}

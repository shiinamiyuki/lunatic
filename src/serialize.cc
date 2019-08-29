#include "value.h"
#include "vm.h"
namespace lunatic {
	SerializeContext::SerializeContext(VM * vm):vm(vm) {
	
	}
	String* SerializeContext::newString(const std::string& s) {
		return vm->alloc<String>(s);
	}
	Table* SerializeContext::newTable() {
		return vm->alloc<Table>();
	}
	void fromLuaValue(const Value& v, std::string& s, SerializeContext* ctx){
		v.checkString();
		s = v.getString()->str();
	}
	void fromLuaValue(const Value& v, const char*& s, SerializeContext* ctx){
		v.checkString();
		s = v.getString()->str().c_str();
	}
	void toLuaValue(Value& v, const std::string&s ,SerializeContext* ctx){
		v.setString(ctx->newString(s));
	}
	void toLuaValue(Value& v, const char*s , SerializeContext* ctx){
		v.setString(ctx->newString(s));
	}
}
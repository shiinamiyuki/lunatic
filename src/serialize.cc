#include "value.h"
#include "vm.h"
namespace lunatic {
	SerializeContext::SerializeContext(std::shared_ptr<VM> vm):vm(vm) {
	
	}
	String* SerializeContext::newString(const std::string& s) {
		return vm->alloc<String>(s);
	}
	Table* SerializeContext::newTable() {
		return vm->alloc<Table>();
	}
}
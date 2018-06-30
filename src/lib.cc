/*
 * lib.cc
 *
 *  Created on: 2018Äê6ÔÂ30ÈÕ
 *      Author: xiaoc
 */

#include "lib.h"
#include "value.h"

SPEKA_BEGIN
void ListLength(VM* vm) {
	auto arg = vm->getLocal(0);
	if (arg.isList()) {
		int len = arg.getList().size();
		Value r;
		r.setInt(len);
		vm->storeReturn(0, r);
	}else{
		throw std::runtime_error("is not list object");
	}
}
void print(speka::VM*vm){
	auto v = vm->getLocal(0);
	std::cout << v.str() << std::endl;
}
SPEKA_END



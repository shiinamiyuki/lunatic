/*
 * gc.c
 *
 *  Created on: 2018Äê6ÔÂ25ÈÕ
 *      Author: XiaochunTong
 */


#include "speka.h"
#include "gc.h"
SPEKA_BEGIN
void GCPtr::dec() {
	if (!refCount)
		return;
	//std::cout<<"5"<<std::endl;
	*refCount = *refCount - 1;

	if (*refCount < 0) {// must be < , not <=
		if(data)
			delete data;//here!
	//	std::cout<<"5.5"<<std::endl;
		data = nullptr;
		delete refCount;
		refCount = nullptr;
	}

}



GCPtr::~GCPtr() {
	dec();
}




SPEKA_END


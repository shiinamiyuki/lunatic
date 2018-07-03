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
	*refCount = *refCount - 1;

	if (*refCount < 0) {// must be < , not <=
		if(data)
			delete data;
		data = nullptr;
		delete refCount;
		refCount = nullptr;
	}

}



GCPtr::~GCPtr() {
	dec();
}




SPEKA_END


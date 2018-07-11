/*
 * gc.c
 *
 *  Created on: 2018��6��25��
 *      Author: XiaochunTong
 */


#include "lunatic.h"
#include "gc.h"
namespace lunatic{
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




}


/*
 * gc.h
 *
 *  Created on: 2018Äê6ÔÂ25ÈÕ
 *      Author: XiaochunTong
 */

#ifndef GC_H_
#define GC_H_
#include "lunatic.h"
namespace lunatic{
class _Ptr {
public:
	_Ptr(){

	}
	virtual ~_Ptr() {
	}
	virtual inline void * get() const {
		throw std::runtime_error("should not reach here");
	}
	virtual void collect(){}
};
template<class _T>
class GCPtr_Ptr: public _Ptr {
	_T* data;
public:
	GCPtr_Ptr(_T*d) {
	//	std::cout<<"created"<<std::endl;
		data = d;
		assert(get());
	}
	~GCPtr_Ptr() override {
	//	std::cout<<"destroyed"<<std::endl;
		collect();
	}
	inline void * get() const override {
		return data;
	}
	void collect(){
		if(data)
					delete data;
	}
};
class GCPtr {
	_Ptr*data;
	int *refCount;
	inline void resetCount() {
		refCount = new int;
		*refCount = 0;
	}
	void dec();
	inline void inc() {
		if (refCount)
			*refCount = *refCount + 1;
	}
	bool marked;
public:
	inline void resetMark(){
		marked = false;
	}
	inline void mark(){
		marked = true;
	}
	inline void collectIfNeeded() {
		if (!marked) {
			if (!data)
				return;
			data->collect();
			data = nullptr;
		}
	}
	GCPtr() {
		refCount = nullptr;
		data = nullptr;
		marked = false;
	}
	GCPtr(const GCPtr&rhs) {
		data = rhs.data;
		refCount = rhs.refCount;
		inc();
		marked = false;
	}
	template<typename T>
	inline T* get() const {
		assert(data);
		auto p = static_cast<T*>(data->get());
		assert(p);
		return p;
	}
	template<typename T>
	inline void reset(T * p) {
		dec();
		data = new GCPtr_Ptr<T>(p);
		assert(data);
		assert(data->get());
		resetCount();
	}
	inline void reset(void *p){
		throw std::runtime_error("cannot bind a nullptr");
	}

	inline void reset() {
		dec();
		refCount = nullptr;
		data = nullptr;
	}
	inline GCPtr& operator =(const GCPtr& rhs) {
		//std::cout<<"copy"<<std::endl;
		if (!rhs.isNull()) {
			dec();
			data = rhs.data;
			refCount = rhs.refCount;
			inc();
		} else {
			reset();
		}
		return *this;
	}
	inline bool isNull() const {
		return nullptr == data;// || nullptr == data->get();
	}
	~GCPtr();
};
}

#endif /* GC_H_ */

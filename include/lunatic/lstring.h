#pragma once
#include "common.h"
#include "gc.h"
namespace lunatic {
	class String : public GCObject{
		std::string data;
	public:
		String(const std::string& data) :data(data) {}
		const std::string& str()const { return data; }
		void markReferences(GC*)const override {}
		virtual size_t nBytes()const{
			return sizeof(*this);
		}
	};
}
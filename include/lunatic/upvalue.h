#pragma once
#include <lunatic/gc.h>
#include <lunatic/common.h>
namespace lunatic {
	class UpValue : public GCObject {
		std::unordered_map<int, Value> values;
		UpValue* parent = nullptr;
		bool contains(size_t i)const {
			auto it = values.find(i);
			if (it != values.end())
				return true;
			if (parent) {
				return parent->contains(i);
			}
			return false;
		}
	public:
		UpValue() = default;
		UpValue(UpValue* parent) :parent(parent) {}
		const Value& get(size_t i)const {
			auto it = values.find(i);
			if (it != values.end())
				return it->second;
			if (parent) {
				return parent->get(i);
			}
			// serious error
			std::abort();
		}
		void set(size_t i, const Value& v) {
			//values[i] = v;
			auto it = values.find(i);
			if (it != values.end()) {
				it->second = v;
			}
			else {
				if (parent && parent->contains(i)) {
					parent->set(i, v);
				}
				else {
					values[i] = v;
				}
			}
		}
		void markReferences(GC* gc)const override {
			for (auto i : values) {
				gc->mark(i.second);
			}
			if(parent)
			gc->mark(parent);
		}
		virtual size_t nBytes()const {
			return sizeof(*this) + sizeof(GCObject*) * values.size();
		}
	};
}
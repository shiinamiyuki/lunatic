#pragma once
#include "common.h"
#include "value.h"
#include "gc.h"
namespace lunatic {
	class Value;
	class Table;

	class Table : public GCObject {
		std::unordered_map<std::string, Value> sMap;
		std::unordered_map<int, Value> iMap;
		std::vector<Value> list;
		Table* metatable=nullptr;
	public:
		friend class Value;

		Value get(int);

		Value get(const std::string&);

		void set(int, const Value&);

		void set(const std::string&, const Value&);

		Table() : sMap(), iMap() { list.push_back(Value()); }

		inline int len() const { return list.size() - 1; }

		inline std::vector<Value>& getList() { return list; }
		inline bool isNil()const {
			return len() == 0 && sMap.size() == 0;
		}
		void markReferences(GC*)const override;
		virtual size_t nBytes()const{
			return sizeof(*this);
		}
	};

}

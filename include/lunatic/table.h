#pragma once
#include <lunatic/common.h>
#include <lunatic/value.h>
#include <lunatic/gc.h>
namespace lunatic {
	class Value;
	class Table;
	class VM;
	class Table : public GCObject {
		std::unordered_map<Value, Value, Value::Hash> map;
		std::vector<Value> list;
	public:
		friend class Value;

		Value get(const Value&);

		void set(const Value&, const Value&);

		Table(){ }

		inline int len() const { return list.size();}
		inline std::vector<Value>& getList() { return list; }
		inline bool isNil()const {
			return len() == 0 && map.size() == 0;
		}
		void markReferences(GC*)const override;
		virtual size_t nBytes()const{
			return sizeof(*this);
		}
		struct end_tag_t {};
		std::pair<Value, Value> next(const Value & key, VM *vm)const;
	};
}

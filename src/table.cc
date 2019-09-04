#include "table.h"
#include "vm.h"
namespace lunatic {
	void Table::markReferences(GC* gc)const {
		for (const auto& pair : iMap) {
			gc->mark(pair.second);
		}
		for (const auto& pair : sMap) {
			gc->mark(pair.second);
		}
		for (const auto& i : list) {
			if (i.isTable()) {
				gc->mark(i.getTable());
			}
		}
	}
	Value Table::get(int i) {
		if (i >= 0 && i < list.size()) {
			return list[i];
		}
		auto iter = iMap.find(i);
		if (iter != iMap.end())
			return iter->second;
		else{
			return Value();
		}

	}
	Value Table::get(const std::string& s) {
		auto iter = sMap.find(s);
		if (iter != sMap.end())
			return iter->second;
		else {
			return Value();
		}
		
	}
	void Table::set(int i, const Value& v) {
		if (i >= 0 && i < list.size()) {
			if (v.isNil() && i == list.size() - 1) {
				list.pop_back();
			}
			else
				list[i] = v;
		}
		else if (i == list.size()) {
			list.push_back(v);
		}
		else {
			auto iter = iMap.find(i);
			if (iter == iMap.end()) {
				iMap.insert(std::make_pair(i, v));
			}
			else {
				iMap[i] = v;
			}
		}
	}

	void Table::set(const std::string& s, const Value& v) {
		auto iter = sMap.find(s);
		if (iter == sMap.end()) {
			if (!v.isNil())
				sMap.insert(std::make_pair(s, v));
		}
		else {
			if (v.isNil()) {
				sMap.erase(iter);
			}
			else
				sMap[s] = v;
		}
	}

	std::pair<Value, Value> Table::iterator::get(VM* vm)const {
		if (state == init || state == end) {
			return std::make_pair(Value(), Value());
		}
		if (state == list) {
			Value key;
			key.store(0);
			return std::make_pair(key, table->list[listIter]);
		}
		if (state == iMap) {
			Value key;
			key.store(iMapIter->first);
			return std::make_pair(key, iMapIter->second);
		}
		Value key;
		SerializeContext ctx(vm);
		key.store(sMapIter->first, &ctx);
		return std::make_pair(key, sMapIter->second);
	}
}
#include "table.h"

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
		if (metatable) {
			gc->mark(metatable);
		}
	}
	Value Table::get(int i) {
		if (i >= 0 && i < list.size()) {
			return list[i];
		}
		auto iter = iMap.find(i);
		if (iter != iMap.end())
			return iter->second;
		else if (!metatable) {
			return Value();
		}
		else {
			return metatable->get(i);
		}
	}
	Value Table::get(const std::string& s) {
		auto iter = sMap.find(s);
		if (iter != sMap.end())
			return iter->second;
		else if (!metatable) {
			return Value();
		}
		else {
			return metatable->get(s);
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


}
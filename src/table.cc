#include "table.h"
#include "vm.h"
namespace lunatic {
	void Table::markReferences(GC* gc)const {
		for (const auto& pair : map) {
			gc->mark(pair.first);
			gc->mark(pair.second);
		}
		for (const auto& i : list) {
			if (i.isTable()) {
				gc->mark(i.getTable());
			}
		}
	}
	Value Table::get(const Value & v) {
		if(v.isInt()){
			auto i = v.getInt();
			if (i >= 1 && i <=list.size()) {
				return list[i-1];
			}
		}
		auto iter = map.find(v);
		if (iter != map.end())
			return iter->second;
		else{
			return Value();
		}

	}

	void Table::set(const Value & k, const Value& v) {
		if(k.isInt()){
			auto i = k.getInt();
			if (i >= 1 && i <= list.size()) {
				if (v.isNil() && i == list.size()) {
					list.pop_back();
				}
				else
					list[i-1] = v;
			}
			else if (i == list.size()+1) {
				list.push_back(v);
			}else{
				map[k] = v;
			}
		}
		else {
			map[k] = v;
		}
	}
	std::pair<Value, Value> Table::next(const Value &k, VM * vm)const{
		if(k.isNil()){
			if(list.empty()){
				if(map.empty()){
					return std::make_pair(k,k);
				}else{
					auto iter = map.begin();
					return std::make_pair(iter->first, iter->second);
				}
			}else{
				Value v;
				v.setInt(1);
				return std::make_pair(v, list[0]);
			}
		}
		if(k.isInt()){
			auto i = k.getInt();
			if (i >= 1 && i < list.size()) {
				Value v;
				v.setInt(i+1);
				return std::make_pair(v, list[i]);
			}else{
				if(map.empty()){
					return std::make_pair(k,k);
				}else{
					auto iter = map.begin();
					return std::make_pair(iter->first, iter->second);
				}
			}
		}
		auto iter = map.find(k);
		if (iter != map.end()){
			iter++;
			if (iter != map.end())
				return std::make_pair(iter->first, iter->second);
			else
				return std::make_pair(Value(),Value());
		}else{
			return std::make_pair(Value(),Value());
		}
	}
}
/*
 * table.cc
 *
 *  Created on: 2018Äê6ÔÂ26ÈÕ
 *      Author: XiaochunTong
 */

#include "table.h"

SPEKA_BEGIN
Value Table::get(int i) {
	auto iter = iMap.find(i);
	if (iter != iMap.end())
		return iter->second;
	else if (!proto.isNull()) {
		return proto.get<Table>()->get(i);
	} else {
		throw std::runtime_error("cannot find index");
	}
}
Value Table::get(const std::string &s) {
	auto iter = sMap.find(s);
	if (iter != sMap.end())
		return iter->second;
	else if (!proto.isNull()) {
		return proto.get<Table>()->get(s);
	} else {
		throw std::runtime_error("cannot find index");
	}
}
void Table::set(int int1, const Value&v) {
	auto iter = iMap.find(int1);
	if (iter == iMap.end()) {
	//	std::cout<<"here"<<std::endl;
		iMap.insert(std::make_pair(int1, v));
	//	std::cout<<"there"<<std::endl;
	} else {
	//	std::cout<<"what's next"<<std::endl;
		iMap[int1] = v;
	//	std::cout<<"heh?"<<std::endl;
	}
}

void Table::set(const std::string& s, const Value&v) {
	auto iter = sMap.find(s);
	if (iter == sMap.end()) {
		sMap.insert(std::make_pair(s, v));
	} else {
		sMap[s] = v;
	}
}


SPEKA_END



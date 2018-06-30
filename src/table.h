	/*
 * table.h
 *
 *  Created on: 2018Äê6ÔÂ26ÈÕ
 *      Author: XiaochunTong
 */

#ifndef TABLE_H_
#define TABLE_H_
#include "speka.h"
#include "value.h"
#include "gc.h"
SPEKA_BEGIN
class Value;
class Table;
class Table{
	std::unordered_map<std::string,Value> sMap;
	std::unordered_map<int,Value>iMap;
	GCPtr proto;
public:
	friend class Value;
	Value get(int);
	Value get(const std::string&);
	void set(int,const Value&);
	void set(const std::string&,const Value&);
	Table():sMap(),iMap(){}
};

SPEKA_END


#endif /* TABLE_H_ */

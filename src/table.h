	/*
 * table.h
 *
 *  Created on: 2018Äê6ÔÂ26ÈÕ
 *      Author: XiaochunTong
 */

#ifndef TABLE_H_
#define TABLE_H_
#include "lunatic.h"
#include "value.h"
#include "gc.h"
namespace lunatic{
class Value;
class Table;
class Table{
	std::unordered_map<std::string,Value> sMap;
	std::unordered_map<int,Value>iMap;
    std::vector<Value> list;
	GCPtr proto;
public:
	friend class Value;
	Value get(int);
	Value get(const std::string&);
	void set(int,const Value&);
	void set(const std::string&,const Value&);
    Table():sMap(),iMap(){list.push_back(Value());}
    inline int len(){return list.size() - 1;}
    inline std::vector<Value>& getList(){return list;}
};

}


#endif /* TABLE_H_ */

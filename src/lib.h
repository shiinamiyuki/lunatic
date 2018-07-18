/*
 * lib.h
 *
 *  Created on: 2018Äê6ÔÂ30ÈÕ
 *      Author: xiaoc
 */

#ifndef LIB_H_
#define LIB_H_

#include "lunatic.h"
#include "scriptengine.h"

#include <windows.h>
namespace lunatic{
class StringLib
{
public:

};

class FileLib{
public:

};
class TableLib{
public:
    static void clone(VM*vm);
};


void ListLength(VM* vm);
void ListAppend(VM* vm);
void StringtoList(VM*vm);
void ListtoString(VM*vm);
void print(VM*vm);
void run(VM*vm);
void tonumber(VM*);
void tostring(VM*);
}



#endif /* LIB_H_ */

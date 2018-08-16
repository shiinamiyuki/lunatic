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
#include <GLFW\glfw3.h>
#include <GL\gl.h>
#include <windows.h>
namespace lunatic{
class StringLib
{
public:
    static void length(VM*vm);
    static void sub(VM*vm);
    static void Char(VM*vm);
    static void byte(VM*vm);
};

class FileLib{
public:
    static void open(VM*vm);
    static void read(VM*vm);
    static void write(VM*vm);
    static void close(VM*vm);
};
class TableLib{
public:
    static void clone(VM*vm);
};
class GLFWLib{
public:
    static void createWindow(VM *vm);
    static void glfwMakeContextCurrent(VM*vm);
};

void ListLength(VM* vm);
void ListAppend(VM* vm);
void StringtoList(VM*vm);
void ListtoString(VM*vm);
void print(VM*vm);
void run(VM*vm);
void tonumber(VM*);
void tostring(VM*);
void setmetatable(VM*vm);
void getmetatable(VM*vm);
void _getline(VM *vm);
}



#endif /* LIB_H_ */

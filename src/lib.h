/*
 * lib.h
 *
 *  Created on: 2018Äê6ÔÂ30ÈÕ
 *      Author: xiaoc
 */

#ifndef LIB_H_
#define LIB_H_

#include "speka.h"
#include "scriptengine.h"
/*
#include <windows.h>
#include<GL/gl.h>
#include<GL/glu.h>
#include <GL/freeglut.h>*/
SPEKA_BEGIN
class StringLib
{
public:

};
class MathLib{
public:
	static void sin(VM*vm);
	static void cos(VM*vm);
	static void tan(VM*vm);
	static void sqrt(VM*vm);
};
class FileLib{
public:

};
class TableLib{
public:

};
/*
class GLLib{
public:
	static int argc;
	static char**argv;
	static Value renderFunc;
	static VM *vm;
	static void initArg(int c,char**v){argc = c;argv = v;}
	static void init(VM *vm);
	static void setWindowPos(VM *vm);
	static void setWindowSize(VM*vm);
	static void setRenderFunc(VM*vm);
	static void render();
	static void glBegin(VM*vm);
	static void glEnd(VM*vm);
	static void glColor(VM*vm);
	static void glVertex(VM*vm);
	static void glMainLoop(VM*vm);
};*/
void ListLength(VM* vm);
void ListAppend(VM* vm);
void StringtoList(VM*vm);
void ListtoString(VM*vm);
void print(speka::VM*vm);
void run(VM*vm);
SPEKA_END



#endif /* LIB_H_ */

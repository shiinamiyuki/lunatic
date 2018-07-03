/*
 * lib.cc
 *
 *  Created on: 2018Äê6ÔÂ30ÈÕ
 *      Author: xiaoc
 */

#include "lib.h"
#include "value.h"

SPEKA_BEGIN
void ListLength(VM* vm) {
	auto arg = vm->getLocal(0);
	if (arg.isList()) {
		int len = arg.getList().size();
		Value r;
		r.setInt(len);
		vm->storeReturn(0, r);
	} else {
		throw std::runtime_error("is not list object");
	}
}
void print(speka::VM*vm) {
	auto v = vm->getLocal(0);
	std::cout << v.str() << std::endl;
}
void ListAppend(VM* vm) {
	auto arg = vm->getLocal(0);
	if (arg.isList()) {
		auto v = vm->getLocal(1);
		arg.getList().push_back(v);
		vm->storeReturn(0, arg);
	} else {
		throw std::runtime_error("is not list object");
	}
}
void StringtoList(VM* vm) {
	auto arg = vm->getLocal(0);
	if (arg.isString()) {
		List * vec = new List();
		for (auto i : arg.getString()) {
			Value ch;
			ch.setInt(i);
			vec->push_back(ch);
		}
		Value v;
		v.setList(vec);
		vm->storeReturn(0, v);
	} else {
		throw std::runtime_error("is not string object");
	}
}

void ListtoString(VM* vm) {
	auto arg = vm->getLocal(0);
	if (arg.isList()) {
		GCPtr str;
		str.reset(new std::string());
		for (auto i : arg.getList()) {
			char c = i.getInt();
			std::string s = "";
			s += c;
			str.get<std::string>()->append(s);
		}
		Value v;
		v.setString(str);
		vm->storeReturn(0, v);
	} else {
		throw std::runtime_error("is not list object");
	}
}
void MathLib::sin(VM* vm) {
	auto x = vm->getLocal(0);
	x.checkFloat();
	auto val = x.getFloat();
	val = ::sin(val);
	x.setFloat(val);
	vm->storeReturn(0, x);
}

void MathLib::cos(VM* vm) {
	auto x = vm->getLocal(0);
	x.checkFloat();
	auto val = x.getFloat();
	val = ::cos(val);
	x.setFloat(val);
	vm->storeReturn(0, x);
}
void MathLib::tan(VM* vm) {
	auto x = vm->getLocal(0);
	x.checkFloat();
	auto val = x.getFloat();
	val = ::tan(val);
	x.setFloat(val);
	vm->storeReturn(0, x);

}

void MathLib::sqrt(VM* vm) {
	auto x = vm->getLocal(0);
	x.checkFloat();
	auto val = x.getFloat();
	val = ::sqrt(val);
	x.setFloat(val);
	vm->storeReturn(0, x);
}
void run(VM* vm) {
	auto x = vm->getLocal(0);
	x.checkClosure();
	vm->call(x.getClosureAddr(),0);
}
/*
void GLLib::init(VM* vm) {
	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_DEPTH|GLUT_SINGLE|GLUT_RGBA);
}

void GLLib::setWindowPos(VM* vm) {
	auto x = vm->getLocal(0);
	auto y = vm->getLocal(1);
	x.checkFloat();
	y.checkFloat();
	glutInitWindowPosition(x.getFloat(),y.getFloat());


}

void GLLib::setWindowSize(VM* vm) {
	auto x = vm->getLocal(0);
	auto y = vm->getLocal(1);
	x.checkFloat();
	y.checkFloat();
	glutInitWindowSize(x.getFloat(),y.getFloat());
}
void GLLib::setRenderFunc(VM* vm) {
	auto x = vm->getLocal(0);
	x.checkClosure();
	renderFunc = x;
	GLLib::vm = vm;
}

void GLLib::render() {
	vm->call(renderFunc.getClosureAddr(),0);
}
void GLLib::glBegin(VM* vm) {
	::glBegin(GL_TRIANGLES);
}

void GLLib::glEnd(VM* vm) {
	::glEnd();
	::glFlush();
}

void GLLib::glColor(VM* vm) {
	auto x = vm->getLocal(0);
	auto y = vm->getLocal(1);
	auto z = vm->getLocal(2);
	x.checkFloat();
	y.checkFloat();
	z.checkFloat();
	glColor3d(x.getFloat(), y.getFloat(), z.getFloat());
}

void GLLib::glVertex(VM* vm) {
	auto x = vm->getLocal(0);
	auto y = vm->getLocal(1);
	x.checkFloat();
	y.checkFloat();
	glVertex2d(x.getFloat(), y.getFloat());
}


void GLLib::glMainLoop(VM* vm) {
	glutCreateWindow("Speka GLUT Module");
	glutDisplayFunc(GLLib::render);
	glutMainLoop();
}
*/





SPEKA_END



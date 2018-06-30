/*
 * opcode.h
 *
 *  Created on: 2018Äê6ÔÂ21ÈÕ
 *      Author: XiaochunTong
 */

#ifndef OPCODE_H_
#define OPCODE_H_
#include "speka.h"
SPEKA_BEGIN
enum class Opcode {
	Add, //ABC
	Sub, //ABC
	Mul, //ABC
	Div, //ABC
	iDiv, //ABC
	LT,
	GT,
	LE,
	GE,
	NE,
	EQ,
	LoadInt, // A i32
	LoadFloat, // A f64
	LoadStr, // A Bx
	LoadBool, //A Bx
	LoadGlobal, //A Bx
	LoadRet,// A B
	PushSelf,
	PushNil,
	LoadSelf,
	GetValue,//A B C   C = A[B]
	StoreValue,//A B C   A[B] = C
	StoreRet,//A B
	StoreGlobal,//A Bx
	Move,//A B
	BRC,
	BZ,
	Push,//A
	fCall,//A B C A: func reg B : number of args C: number of rets
	invoke,//A Bx
	Ret,
	MakeClosure,
	NewTable,
	NewList,
	ListAppend,
	Clone,//A B
	SetProto,// A B

};
struct Instruction{
	Opcode opcode;
	char operand[10];
	inline int getA() const {
		return operand[0];
	}
	inline int getB() const {
		return operand[1];
	}
	inline int getC() const {
		return operand[2];
	}
	inline int getInt() const {
		return *(int*) (operand + 1);
	}
	inline double getFloat() const {
		return *(double*) (operand + 1);
	}
	inline int getBx() const {
		return getInt() & 0xffff;
	}
	Instruction(Opcode op, int A, int B, int C) {
		opcode = op;
		operand[0] = A & 0xff;
		operand[1] = B & 0xff;
		operand[2] = C & 0xff;
	}
	Instruction(Opcode op, int A, int Bx) {
		opcode = op;
		operand[0] = A & 0xff;
		*(int*) (operand + 1) = Bx;
	}
	Instruction(Opcode op, int A, double f) {
		opcode = op;
		operand[0] = A & 0xff;
		*(double*) (operand + 1) = f;
	}
	std::string str()const;
};
SPEKA_END
#endif /* OPCODE_H_ */

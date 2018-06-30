/*
 * opcode.cc
 *
 *  Created on: 2018Äê6ÔÂ27ÈÕ
 *      Author: xiaoc
 */


#include "opcode.h"
SPEKA_BEGIN

#define W_AB(o) if(opcode == o){out << (#o) <<" "<< getA() <<" "<<getInt();}
#define W_ABC(o)if(opcode == o){out << (#o) <<" "<< getA() <<" "<<getB()<<" "<<getC();}
#define W_F(o) if(opcode == o){out << (#o) <<" "<< getA() <<" "<<getFloat();}
std::string Instruction::str() const {
	std::ostringstream out;
	W_ABC(Opcode::Add)
	W_ABC(Opcode::Sub)
	W_ABC(Opcode::Mul)
	W_ABC(Opcode::Div)
	W_ABC(Opcode::LT)
	W_ABC(Opcode::LE)
	W_ABC(Opcode::GT)
	W_ABC(Opcode::GE)
	W_ABC(Opcode::EQ)
	W_ABC(Opcode::NE)
	W_AB(Opcode::BRC)
	W_AB(Opcode::BZ)
	W_AB(Opcode::LoadGlobal)
	W_AB(Opcode::LoadRet)
	W_AB(Opcode::LoadInt)
	W_F(Opcode::LoadFloat)
	W_ABC(Opcode::fCall)
	W_AB(Opcode::Ret)
	W_AB(Opcode::StoreGlobal)
	W_AB(Opcode::StoreRet)
	W_AB(Opcode::Push)
	W_AB(Opcode::Move)
	W_AB(Opcode::MakeClosure)
	W_AB(Opcode::invoke)
	W_AB(Opcode::LoadStr)
	W_ABC(Opcode::GetValue)
	W_ABC(Opcode::StoreValue)
	W_AB(Opcode::NewTable)
	W_AB(Opcode::LoadSelf)
	W_AB(Opcode::Clone)
	W_AB(Opcode::PushNil)
	W_AB(Opcode::PushSelf)
	W_AB(Opcode::SetProto)
	return out.str();
}

SPEKA_END


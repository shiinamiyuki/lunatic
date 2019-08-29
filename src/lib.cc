#include "lib.h"
#include "value.h"
#include "table.h"
#include "lstring.h"
namespace lunatic {
	void print(VM* vm) {
		int cnt = vm->getArgCount();
		for (int i = 0; i < cnt; i++) {
			auto v = vm->getLocal(i);
			std::cout << v.str() << " ";
		}
		std::cout << std::endl;
	}
	void tonumber(VM* vm) {
		auto x = vm->getLocal(0);
		Value ret;
		if (x.isString()) {
			std::istringstream s(x.getString()->str());
			double d;
			s >> d;
			ret.setFloat(d);
		}
		vm->storeReturn(0, ret);
	}
	void tostring(VM* vm) {
		auto x = vm->getLocal(0);
		Value ret;
		if (x.isInt()) {
			std::ostringstream s;
			s << x.getInt();

			auto p = vm->alloc<String>(s.str());
			ret.setString(p);
		}
		else if (x.isFloat()) {
			std::ostringstream s;
			s << x.getFloat();
			auto p = vm->alloc<String>(s.str());
			ret.setString(p);
		}
		vm->storeReturn(0, ret);
	}

	void TableLib::clone(VM* vm)
	{
		auto arg = vm->getLocal(0);
		arg.checkTable();
		Value v;
		v.setTable(vm->alloc<Table>(*arg.getTable()));
		vm->storeReturn(0, v);
	}

	void setmetatable(VM* vm)
	{
		auto arg1 = vm->getLocal(0);
		auto arg2 = vm->getLocal(1);
		arg1.checkTable();
		arg2.checkTable();
		arg1.setMetaTable(arg2);
	}
	void getmetatable(VM* vm) {
		auto arg1 = vm->getLocal(0);
		arg1.checkTable();
		Value v;
		v.setTable(arg1.getMetatable());
		vm->storeReturn(0, v);
	}

	void _getline(VM* vm)
	{
		Value v;
		std::string s;
		std::getline(std::cin, s);
		v.setString(s);
		vm->storeReturn(0, v);
	}

	void StringLib::length(VM* vm) {
		auto arg1 = vm->getLocal(0);
		arg1.checkString();
		Value v;
		v.setInt((int)arg1.getString()->str().length());
		vm->storeReturn(0, v);
	}

	void StringLib::sub(VM* vm) {
		int c = vm->getArgCount();
		auto arg1 = vm->getLocal(0);
		arg1.checkString();
		auto arg2 = vm->getLocal(2);
		arg2.checkInt();
		auto s = arg1.getString()->str();
		if (c == 3) {
			auto arg3 = vm->getLocal(3);
			arg3.checkInt();
			s = s.substr(arg2.getInt(), arg3.getInt());
		}
		else {
			s = s.substr(arg2.getInt());
		}
		Value v;
		v.setString(vm->alloc <String>(s));
		vm->storeReturn(0, v);
	}

	void StringLib::Char(VM* vm) {
		auto arg1 = vm->getLocal(0);
		arg1.checkInt();
		Value v;
		std::string s;
		s += arg1.getInt();
		v.setString(s);
		vm->storeReturn(0, v);
	}

	void StringLib::byte(VM* vm) {
		/*	auto arg1 = vm->getLocal(0);
			arg1.checkString();
			Value v;
			v.setInt((int)&arg1->getString()[0]);
			vm->storeReturn(0, v);*/
	}

	void FileLib::open(VM* vm) {
		auto arg1 = vm->getLocal(0);
		arg1.checkString();
		auto arg2 = vm->getLocal(1);
		arg2.checkString();
		FILE* f = fopen(arg1.getString()->str().c_str(), arg2.getString()->str().c_str());
		Value file;
		file.setUserData(f);
		Value v;
		v.setTable(vm->alloc<Table>());
		v.set("fp", file);
		vm->storeReturn(0, v);
	}

	void FileLib::read(VM* vm) {
		auto arg1 = vm->getLocal(0);
		arg1.checkTable();
		auto file = arg1.get("fp");
		file.checkUserData();
		FILE* f = static_cast<FILE*>(file.getUserData());
		std::string s;
		while (!feof(f)) {
			char c = fgetc(f);
			if (!c || c == EOF)break;
			s += c;
		}
		Value v;
		v.setString(s);
		vm->storeReturn(0, v);
	}

	void FileLib::write(VM* vm) {
		auto arg1 = vm->getLocal(0);
		arg1.checkTable();
		auto file = arg1.get("fp");
		file.checkUserData();
		FILE* f = static_cast<FILE*>(file.getUserData());
		auto arg2 = vm->getLocal(1);
		const std::string s = arg2.str();
		fprintf(f, "%s", s.c_str());
	}

	void FileLib::close(VM* vm) {
		auto arg1 = vm->getLocal(0);
		arg1.checkTable();
		auto file = arg1.get("fp");
		file.checkUserData();
		FILE* f = static_cast<FILE*>(file.getUserData());
		fclose(f);
	}
	void collectGarbage(VM* vm) {
		auto _opt = vm->getLocal(0);
		std::string opt = "collect";
		if (!_opt.isNil()) {
			opt = _opt.load<std::string>();
		}
		if (opt == "count") {
			Value ret;
			ret.store(vm->getMemoryUsage(true));
			vm->storeReturn(0, ret);
		}
		else if (opt == "collect") {
			vm->collect();
		}
	}
	void pCall(VM* vm) {
		auto func = vm->getLocal(0);
		func.checkClosure();
		auto info = vm->getCurrentState()->save();
		try {
			vm->call(func.getClosureAddr(), 0);
			vm->forceRecurse();
			Value ret;
			ret.setBool(true);
			vm->storeReturn(0, ret);
		}
		catch (std::runtime_error& e) {
			Value ret;
			vm->storeReturn(0, ret);
			Value msg;
			SerializeContext ctx(vm);
			msg.store(e.what(), &ctx);
			vm->storeReturn(1, msg);
			vm->getCurrentState()->restoreFrom(info);
		}
	}
}

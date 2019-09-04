#include "lib.h"
#include "value.h"
#include "table.h"
#include "lstring.h"
#include "gc.h"
namespace lunatic {
	void print(const CallContext& ctx) {
		auto vm = ctx.vm;
		int cnt = ctx.nArgs;
		for (int i = 0; i < cnt; i++) {
			auto v = vm->getLocal(i);
			std::cout << v.str() << " ";
		}
		std::cout << std::endl;
	}
	void tonumber(const CallContext& ctx) {
		auto vm = ctx.vm;
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
	void tostring(const CallContext& ctx) {
		auto vm = ctx.vm;
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

	void TableLib::clone(const CallContext& ctx)
	{
		auto vm = ctx.vm;
		auto arg = vm->getLocal(0);
		arg.checkTable();
		Value v;
		v.setTable(vm->alloc<Table>(*arg.getTable()));
		vm->storeReturn(0, v);
	}

	void setmetatable(const CallContext& ctx)
	{
		auto vm = ctx.vm;
		auto arg1 = vm->getLocal(0);
		auto arg2 = vm->getLocal(1);
		arg1.checkTable();
		arg2.checkTable();
		arg1.setMetaTable(arg2);
	}
	void getmetatable(const CallContext& ctx) {
		auto vm = ctx.vm;
		auto arg1 = vm->getLocal(0);
		arg1.checkTable();
		Value v;
		v.setTable(arg1.getMetatable());
		vm->storeReturn(0, v);
	}

	void _getline(const CallContext& ctx)
	{
		auto vm = ctx.vm;
		Value v;
		std::string s;
		std::getline(std::cin, s);
		v.setString(s);
		vm->storeReturn(0, v);
	}

	void StringLib::length(const CallContext& ctx) {
		auto vm = ctx.vm;
		auto arg1 = vm->getLocal(0);
		arg1.checkString();
		Value v;
		v.setInt((int)arg1.getString()->str().length());
		vm->storeReturn(0, v);
	}

	void StringLib::sub(const CallContext& ctx) {
		auto vm = ctx.vm;
		int c = ctx.nArgs;
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

	void StringLib::Char(const CallContext& ctx) {
		auto vm = ctx.vm;
		auto arg1 = vm->getLocal(0);
		arg1.checkInt();
		Value v;
		std::string s;
		s += arg1.getInt();
		v.setString(s);
		vm->storeReturn(0, v);
	}

	void StringLib::byte(const CallContext& ctx) {
		auto vm = ctx.vm;
		/*	auto arg1 = vm->getLocal(0);
			arg1.checkString();
			Value v;
			v.setInt((int)&arg1->getString()[0]);
			vm->storeReturn(0, v);*/
	}

	void FileLib::open(const CallContext& ctx) {
		auto vm = ctx.vm;
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

	void FileLib::read(const CallContext& ctx) {
		auto vm = ctx.vm;
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

	void FileLib::write(const CallContext& ctx) {
		auto vm = ctx.vm;
		auto arg1 = vm->getLocal(0);
		arg1.checkTable();
		auto file = arg1.get("fp");
		file.checkUserData();
		FILE* f = static_cast<FILE*>(file.getUserData());
		auto arg2 = vm->getLocal(1);
		const std::string s = arg2.str();
		fprintf(f, "%s", s.c_str());
	}

	void FileLib::close(const CallContext& ctx) {
		auto vm = ctx.vm;
		auto arg1 = vm->getLocal(0);
		arg1.checkTable();
		auto file = arg1.get("fp");
		file.checkUserData();
		FILE* f = static_cast<FILE*>(file.getUserData());
		fclose(f);
	}
	void collectGarbage(const CallContext& ctx) {
		auto vm = ctx.vm;
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
	void pCall(const CallContext& ctx) {
		auto vm = ctx.vm;
		auto func = vm->getLocal(0);
		func.checkClosure();
		auto info = vm->getCurrentState()->save();
		try {
			vm->call(func.getClosure(), 0);
			Value ret;
			ret.setBool(true);
			vm->storeReturn(0, ret);
		}
		catch (RuntimException& e) {
			Value ret;
			vm->storeReturn(0, ret);
			Value msg;
			SerializeContext ctx(vm);
			msg.store(e.what(), &ctx);
			vm->storeReturn(1, msg);
		}
	}
	void next(const CallContext& ctx) {
		auto vm = ctx.vm;
		auto table = vm->getLocal(0);
		if (!table.isTable()) {
			throw RuntimException("table object expected in next()");
		}
		class Next : public Callable {
			Table* table;
			Table::iterator iter;
		public:
			Next(Table* table) :table(table),iter(table) {
				
			}
			void call(const CallContext& ctx) {
				auto vm = ctx.vm;
				auto pair = iter.get(vm);
				vm->storeReturn(0, pair.first);
				vm->storeReturn(1, pair.second);
				iter.next();
			}
			void markReferences(GC* gc)const {
				gc->mark(table);
			}
			size_t nBytes()const override {
				return sizeof(*this);
			}
		};
		auto next = ctx.vm->alloc<Next>(table.getTable());
		Value v;
		v.setNativeFunction(next);
		vm->storeReturn(0, v);
	}
}

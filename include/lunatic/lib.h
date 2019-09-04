#pragma once
#include <common.h>
#include <vm.h>
namespace lunatic {
	class StringLib
	{
	public:
		static void length(const CallContext&);
		static void sub(const CallContext&);
		static void Char(const CallContext&);
		static void byte(const CallContext&);
	};

	class FileLib {
	public:
		static void open(const CallContext&);
		static void read(const CallContext&);
		static void write(const CallContext&);
		static void close(const CallContext&);
	};
	class TableLib {
	public:
		static void clone(const CallContext&);
	};

	void print(const CallContext&);
	void run(const CallContext&);
	void tonumber(const CallContext&);
	void tostring(const CallContext&);
	void setmetatable(const CallContext&);
	void getmetatable(const CallContext&);
	void _getline(const CallContext&);
	void collectGarbage(const CallContext&);
	void pCall(const CallContext&);
	void next(const CallContext&);
}

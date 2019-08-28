#pragma once
#include "common.h"
#include "vm.h"
namespace lunatic {
	class StringLib
	{
	public:
		static void length(VM* vm);
		static void sub(VM* vm);
		static void Char(VM* vm);
		static void byte(VM* vm);
	};

	class FileLib {
	public:
		static void open(VM* vm);
		static void read(VM* vm);
		static void write(VM* vm);
		static void close(VM* vm);
	};
	class TableLib {
	public:
		static void clone(VM* vm);
	};

	void print(VM* vm);
	void run(VM* vm);
	void tonumber(VM*);
	void tostring(VM*);
	void setmetatable(VM* vm);
	void getmetatable(VM* vm);
	void _getline(VM* vm);
	void collectGarbage(VM*vm);
}

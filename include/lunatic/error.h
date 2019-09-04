#pragma once
#include <lunatic/common.h>
namespace lunatic {
	enum class ErrorCode {
		None,
		ParserError,
		CompilerError,
		RuntimeError,
	};
	struct Error {
		ErrorCode code;
		std::string message;
		Error() :code(ErrorCode::None) {}
		Error(ErrorCode code, const std::string& message = "") :code(code), message(message) {}
	};
}
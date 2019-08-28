#pragma once
#include <string>
#include <exception>


const char* printstr(const char* t);
const char* printstr(int t);
const char* printstr(double t);
const char* printstr(float t);
const char* printstr(char t);
const char* printstr(size_t t);
const char* printstr(const std::string& t);

class BadFormatException :public std::exception {
	std::string msg;
public:
	BadFormatException(const std::string& s) { msg = s; }
	const char* what()const  noexcept { return msg.c_str(); }
};
template<typename S = std::string, typename T>
S format(const char* fmt) {
	return std::string(fmt);
}
template<typename S = std::string, typename T>
S format(const char* fmt, T a) {
	S out;
	for (int i = 0; fmt[i]; i++) {
		if (fmt[i] == '{' && fmt[i + 1] == '{') {
			out += fmt[i];
			i++;
		}
		else if (fmt[i] == '}' && fmt[i + 1] == '}') {
			out += fmt[i];
			i++;
		}
		else if (fmt[i] == '{' && fmt[i + 1] == '}') {
			const char* s = printstr(a);
			out.append(s);
			i++;
		}
		else if (fmt[i] == '{' || fmt[i] == '}') {
			throw BadFormatException(format("stary '{}'", fmt[i]).c_str());
		}
		else {
			out += fmt[i];
		}

	}
	return out;
}

template<typename S = std::string, typename T, typename... Args>
S format(const char* fmt, T a, Args... args) {
	S out;
	for (int i = 0; fmt[i]; i++) {
		if (fmt[i] == '{' && fmt[i + 1] == '}') {
			const char* s = printstr(a);
			out.append(s);
			out.append(format(fmt + i + 2, args...));
			return out;
		}
		else {
			out += fmt[i];
		}
	}
	return out;
}
void println(const char* fmt);
void println(const std::string& fmt);


template< typename... Args>
void println(const char* fmt, Args... a) {
	auto str = format(fmt, a...);
	printf("%s", str.c_str());
}
template< typename... Args>
void fprintln(FILE* f, const char* fmt, Args... a) {
	auto str = format(fmt, a...);
	fprintf(f, "%s",str.c_str());
}
#pragma once
#include <string>

class StringTool {
public:
	static std::size_t replace_all(std::string& inout, std::string what, std::string with)
	{
		std::size_t count{};
		for (std::string::size_type pos{};
			inout.npos != (pos = inout.find(what.data(), pos, what.length()));
			pos += with.length(), ++count) {
			inout.replace(pos, what.length(), with.data(), with.length());
		}
		return count;
	}

	static std::size_t remove_all(std::string& inout, std::string what) {
		return replace_all(inout, what, "");
	}
};

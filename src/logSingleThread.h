#pragma once
#include <string>
#include<fstream>
#include<sstream>
#include <chrono>
#include <mutex>
#include <time.h>

#include <stdio.h>

#define LOG(format, ...)  printf(format, __VA_ARGS__);

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

class TradeTime {
public:
	static std::string getCtime(time_t t_c) {
		struct tm nowTime;
		std::string strTime;
#ifdef __UNIX__
		localtime_r( &t_c, &nowTime);
		strTime =  asctime(&nowTime);
#else
		localtime_s(&nowTime, &t_c);
		strTime =  asctime(&nowTime);
#endif	
		return strTime.substr(0, strTime.size() - 1);
	}
};


class Logger {
private:
	std::ofstream logStream;
	std::mutex logMut;
public:
	std::ostringstream stringLog;

	Logger(const std::string& fileName): logStream(fileName, std::ios::out){

	}
	~Logger() {
		logStream.flush();
	}
	void logInfo(){
		logInfo(stringLog.str());
		stringLog.str("");
	}

	void logInfo(const std::string& msg) {
		time_t t_c = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
		logStream<<"INFO:" << "  " << TradeTime::getCtime(t_c) << "  " << msg << std::endl;
		
	}

	void logErr(const std::string& msg) {
		time_t t_c = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
		logStream << "ERR:" << "  " << TradeTime::getCtime(t_c)<< "  " << msg << std::endl;
	}

	void logErr() {
		logErr(stringLog.str());
		stringLog.str("");
	}

	bool logIsOpen() {
		return logStream.is_open();
	}

	static std::string initFileName(const std::string& prefix) {
		time_t t_c = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
		std::string tim_str = TradeTime::getCtime(t_c);
		StringTool::replace_all(tim_str, " ", "_");
		StringTool::replace_all(tim_str, ":", "_");
		return prefix + tim_str + ".log";
	}
};

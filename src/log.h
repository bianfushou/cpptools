#pragma once
#include <string>
#include<fstream>
#include <chrono>
#include <mutex>
#include <ctime>
#include "stringTool.h"

class Logger {
private:
	std::ofstream infoStream;
	std::ofstream errStream;
public:
	Logger(std::string infoName, std::string errName ): infoStream(infoName), errStream(errName){

	}
	~Logger() {
		infoStream.flush();
		errStream.flush();
	}

	void log_info(std::string msg) {
		infoStream << msg << std::endl;
	}

	void log_err(std::string msg) {
		errStream << msg << std::endl;
	}
	bool info_open() {
		return infoStream.is_open();
	}

	bool err_open() {
		return errStream.is_open();
	}
};

class DataLogger {
private:
	Logger* logger = nullptr;
	std::chrono::time_point<std::chrono::system_clock> dataTime;
	std::chrono::hours hour = std::chrono::hours(24);
	std::string infoFileName;
	std::string errFileName;
	std::mutex timeMut;
	std::mutex infoMut;
	std::mutex errMut;

	std::string getCtime(std::time_t t_c) {
		timeMut.lock();
		std::string tim_str = std::asctime(std::localtime(&t_c));
		timeMut.unlock();
		return tim_str;
	}
	void initFileName(){
		std::time_t t_c = std::chrono::system_clock::to_time_t(dataTime);
		
		std::string tim_str = getCtime(t_c);
		StringTool::replace_all(tim_str, " ", "_");
		infoFileName = "info_" + tim_str + ".log";
		errFileName = "err_" + tim_str + ".log";
	}

	void tryCreateLogger(const std::chrono::time_point<std::chrono::system_clock>& nowTime) {
		if (!logger) {

			if ((nowTime - hour) > dataTime) {
				dataTime = nowTime;
			}
			initFileName();
			logger = new Logger(infoFileName, errFileName);
		}
		else {
			if ((nowTime - hour) > dataTime) {
				delete logger;
				dataTime = nowTime;
				initFileName();
				logger = new Logger(infoFileName, errFileName);
			}
		}
	}
public:
	DataLogger() {
		dataTime = std::chrono::system_clock::now();
		
	}
	~DataLogger() {
		if(logger)
			delete logger;
	}

	void log_info(const std::string& msg) {
		auto nowTime = std::chrono::system_clock::now();
		
		tryCreateLogger(nowTime);
		std::time_t t_c = std::chrono::system_clock::to_time_t(nowTime);
		std::string tim_str = getCtime(t_c);
		{
			std::lock_guard<std::mutex> guard(infoMut);
			logger->log_info(tim_str + "_" + msg);
		}
	}

	void log_err(std::string msg) {
		auto nowTime = std::chrono::system_clock::now();

		tryCreateLogger(nowTime);
		std::time_t t_c = std::chrono::system_clock::to_time_t(nowTime);
		std::string tim_str = getCtime(t_c);
		{
			std::lock_guard<std::mutex> guard(errMut);
			logger->log_err(tim_str + "_" + msg);
		}
	}
};

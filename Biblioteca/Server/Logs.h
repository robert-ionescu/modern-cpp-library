#pragma once
#include<fstream>
#include <ctime>
#include <mutex>
#include <string>
#include <cstdio>
#include <iostream>
#include <stdio.h>
#include <time.h>
#include<thread>

#pragma warning(disable:4996)
class Logs
{
public:
	enum class Priority {
		Informational,
		Low,
		Medium,
		High,
		Error,
		Critical
	};
private:
	std::mutex log_mutex;
	std::string buffer;
	std::string timestamp_format = "%T  %d-%m-%Y";
	Priority priority = Priority::Informational;
	inline std::string getPriorityText(Priority p);
	std::string filepath;
	std::fstream file;
	static Logs& get_instance();
	void free_file();
	bool enable_file_output();
	std::string convertToString(char* a, int size)
	{
		int i;
		std::string s = "";
		for (i = 0; i < size; i++) {
			s = s + a[i];
		}
		return s;
	}

	
	void log(Priority message_priority, const std::string message);

	void log(int line_number, const std::string source_file, Priority message_priority, const std::string message);
public:

	Logs();
	~Logs();
	Logs(const Logs&) = delete;
	Logs& operator= (const Logs&) = delete;

	// Returns the current filepath for file logging
	// if Logs::EnableFileOutput was called without specifying a filepath, the filepath will be "log.txt"
	// if file output was not enabled, the filepath will contain NULL
	static const std::string GetFilepath();
	// Returns true is file output was enabled and file was successfully opened, false if it wasn't
	static bool IsFileOutputEnabled();

	// Set a log timestamp format
	// Format follows <ctime> strftime format specification
	// Default format is "%T  %d-%m-%Y" (e.g. 17:45:25  21-11-2021)
	// 4 spaces are added automatically to the end of timestamp each time the message is logged
	static void SetTimestampFormat(const std::string new_timestamp_format);

	// Get the current log timestamp format
	static const std::string GetTimestampFormat();
	static bool EnableFileOutput(const std::string new_filepath);
	static bool EnableFileOutput();
	//void writeToLog(std::string description, Priority severity);
	static void Informational(const std::string message);

	static void Low(const std::string message);

	
	static void Medium(const std::string message);

	
	static void High(const std::string message);

	
	static void Error(const std::string message);

	
	static void Critical(const std::string message);


	
	
	static void Informational(int line, const std::string source_file, const std::string message);

	
	static void Low(int line, const std::string source_file, const std::string message);

	
	static void Medium(int line, const std::string source_file, const std::string message);

	
	static void High(int line, const std::string source_file, const std::string message);

	
	static void Error(int line, const std::string source_file, const std::string message);

	
	static void Critical(int line, const std::string source_file, const std::string message);

};

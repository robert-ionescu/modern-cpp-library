#include"Logs.h"

inline std::string Logs::getPriorityText(Priority p)
{
	switch (p)
	{
	case Priority::Informational:
		return ("Informational");
		break;
	case Priority::Low:
		return ("Low");
		break;
	case Priority::Medium:
		return ("Medium");
		break;
	case Priority::High:
		return ("High");
		break;
	case Priority::Error:
		return ("Error");
		break;
	case Priority::Critical:
		return ("Critical");
		break;
	default:
		break;
	}
}


inline void Logs::log(Priority message_priority, const std::string message) 
{
	std::time_t current_time = std::time(0);
	std::tm* timestamp = std::localtime(&current_time);
	char buffer[80];
	strftime(buffer, 80, "%c", timestamp);
	std::string bufferString = convertToString(buffer, 80);
	const std::string message_priority_str = getPriorityText(message_priority);
	std::scoped_lock lock(log_mutex);
	std::cout << bufferString << '\t';
	std::cout << message_priority_str;
	std::cout << message << std::endl;

	if (file.is_open())
	{
		file << buffer << '\t';
		file << message_priority_str << '\t';
		file << message  << std::endl;
	}
}

inline void Logs::log(int line_number, const std::string source_file, Priority message_priority, const std::string message)
{
	std::time_t current_time = std::time(0);
	std::tm* timestamp = std::localtime(&current_time);
	char buffer[80];
	strftime(buffer, 80, "%c", timestamp);
	std::string bufferString = convertToString(buffer, 80);
	const std::string message_priority_str = getPriorityText(message_priority);
	std::scoped_lock lock(log_mutex);
	std::cout << bufferString << '\t';
	std::cout << message_priority_str;
	std::cout << message;
	std::cout << " on line " << line_number << " in " << source_file << std::endl;

	if (file.is_open())
	{
		file << buffer << '\t';
		file << message_priority_str << '\t';
		file << message;
		file << " on line " << line_number << " in " << source_file << std::endl;
	}
}
void Logs::Informational(const std::string message )
{
	get_instance().log(Priority::Informational, message);
}
void Logs::Low(const std::string message)
{
	get_instance().log( Priority::Low, message);
}

void Logs::Medium(const std::string message)
{
	get_instance().log( Priority::Medium, message);
}

void Logs::High(const std::string message)
{
	get_instance().log( Priority::High, message);
}

void Logs::Error(const std::string message)
{
	get_instance().log(Priority::Error, message);
}

void Logs::Critical(const std::string message)
{
	get_instance().log(Priority::Critical, message);
}
Logs& Logs::get_instance()
{
	static Logs instance;
	return instance;
}

void Logs::free_file()
{
	if (file)
	{
		file.close();
	}
}

bool Logs::enable_file_output()
{
	free_file();
	file.open(filepath);
	if (file)
	{
		return false;
	}
	return true;
}

Logs::Logs()
{
	//file.open("logs.txt");
	//std::ofstream file("logspleasework3.txt");
	//EnableFileOutput("logspleasework3.txt");
}

Logs::~Logs()
{
	free_file();
}

const std::string Logs::GetFilepath()
{
	return get_instance().filepath;
}
bool Logs::IsFileOutputEnabled()
{
	return get_instance().file.is_open();
}
void Logs::SetTimestampFormat(const std::string new_timestamp_format)
{
	get_instance().timestamp_format = new_timestamp_format;
}
const std::string Logs::GetTimestampFormat()
{
	return get_instance().timestamp_format;
}
bool Logs::EnableFileOutput()
{
	Logs& logger_instance = get_instance();
	logger_instance.filepath = "log.txt";
	return logger_instance.enable_file_output();
}
bool Logs::EnableFileOutput(const std::string new_filepath)
{
	Logs& logger_instance = get_instance();
	logger_instance.filepath = new_filepath;
	return logger_instance.enable_file_output();
}

inline void Logs::Informational(int line, const std::string source_file, const std::string message)
{
	get_instance().log(line, source_file, Priority::Informational, message);
}


inline void Logs::Low(int line, const std::string source_file, const std::string message)
{
	get_instance().log(line, source_file, Priority::Low, message);
}

inline void Logs::Medium(int line, const std::string source_file, const std::string message)
{
	get_instance().log(line, source_file, Priority::Medium, message);
}

inline void Logs::High(int line, const std::string source_file, const std::string message)
{
	get_instance().log(line, source_file, Priority::High, message);
}

inline void Logs::Error(int line, const std::string source_file, const std::string message)
{
	get_instance().log(line, source_file, Priority::Error, message);
}

inline void Logs::Critical(int line, const std::string source_file, const std::string message)
{
	get_instance().log(line, source_file, Priority::Critical, message);
}
//void Logs::writeToLog(std::string description, Priority priority)
//{
//	std::ofstream log;
//	//log.open(file, std::ofstream::app);
//	log << getPriorityText(priority) << " " << description << "\n";
//}

#ifndef _HI_LOG_H
#define _HI_LOG_H

#include <time.h>
#include <sstream>
#include <cstdio>
#include <iostream>
#include <fstream>
#include "gloox/loghandler.h"
#include <glib.h>
#include <glib/gstdio.h>

using namespace gloox;


typedef enum {	LOG_AREA_XML = 2,
				LOG_AREA_PURPLE = 4
				} LogAreas;

class LogClass;

class LogMessage
{
public:
	LogMessage(std::ofstream &file, bool newline = true) : m_file(file), m_newline(newline) {}
	~LogMessage() {
		if (m_newline)
			os << std::endl;
		fprintf(stdout, "%s", os.str().c_str());
		fflush(stdout);
		if (m_file.is_open()) {
			m_file << os.str();
			m_file.flush();
		}
	}

	std::ostringstream& Get(const std::string &user) {
		time_t now = time(0);
		char timestamp_buf[25] = "";
		strftime (timestamp_buf, 25, "%x %H:%M:%S", localtime(&now));
		std::string timestamp = std::string(timestamp_buf);
		os << "[" << timestamp << "] <" << user << "> ";
		return os;
	}
protected:
	std::ostringstream os;
	std::ofstream &m_file;
	bool m_newline;
};

class LogClass : public LogHandler {
	public:
		LogClass() {}
		~LogClass() {
			if (m_file.is_open())
				m_file.close();
		}

		void setLogFile(const std::string &file) {
			if (m_file.is_open())
				m_file.close();
			m_file.open(file.c_str(), std::ios_base::app);
			g_chmod(file.c_str(), 0640);
		}
		
		std::ofstream &fileStream() { return m_file; }
		void handleLog(LogLevel level, LogArea area, const std::string &message) {
			if (area == LogAreaXmlIncoming)
				LogMessage(m_file).Get("XML IN") << message;
			else
				LogMessage(m_file).Get("XML OUT") << message;
		}
		
	private:
		std::ofstream m_file;
};
#ifdef TESTS
	#define Log(HEAD,STRING) 
#else
	#define Log(HEAD,STRING) LogMessage(Log_.fileStream()).Get(HEAD) << STRING;
#endif

#endif

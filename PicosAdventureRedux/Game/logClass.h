#ifndef _LOG_CLASS_H_
#define _LOG_CLASS_H_

#include "../Utils/clockClass.h"

#include <windows.h>

#include <mmsystem.h>

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

class LogClass
{
	public:
		LogClass();
		LogClass(const LogClass &);
		~LogClass();

		void setup(std::string name);
		void update();

		void setStartTime();
		void addEntry(std::string type, int arg1, int arg2);
		void setEndTime();

		int getSeconds();
		int getMinutes();
		int getHours();
		int getYear();
		int getMonth();
		int getDay();

		static LogClass* Instance();

	private:
		// LOGGING
		std::ofstream	logFile_;

		ClockClass*		overallGameClock_;

		// Singleton
		static LogClass logClass_;

};

#endif //_LOG_CLASS_H_
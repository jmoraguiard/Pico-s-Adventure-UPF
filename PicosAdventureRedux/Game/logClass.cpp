#include "logClass.h"

LogClass LogClass::logClass_;

LogClass::LogClass()
{
}

LogClass::LogClass(const LogClass &)
{

}

LogClass::~LogClass()
{

}

void LogClass::setup(std::string name)
{
	std::stringstream logFileName;
	logFileName << "./Data/logfile/ " << name <<  getYear() << "-" << getMonth() << "-" << getDay() << " " << getHours() << "-" << getMinutes() << ".csv";
	logFile_.open(logFileName.str());

	std::stringstream fileHeader;
	fileHeader << "EVENT" << "; " << "TIME" << "; " << "ARG1" << "; " << "ARG2\n";
	logFile_ << fileHeader.str();

	overallGameClock_ = new ClockClass();
	if(!overallGameClock_)
	{
		MessageBoxA(NULL, "Could not start log clock.", "LogClass - Error", MB_ICONERROR | MB_OK);
	}
	overallGameClock_->reset();
	overallGameClock_->tick();

	setStartTime();
}

void LogClass::update()
{
	overallGameClock_->tick();
}

void LogClass::setStartTime()
{
	std::stringstream startTime;
	startTime << "Start; " << overallGameClock_->getTime() << "; " << getYear() << "-" << getMonth() << "-" << getDay() << " " << getHours() << ":" << getMinutes() << ":" << getSeconds() << "; 0\n";
	logFile_ << startTime.str();
}

void LogClass::addEntry(std::string type, int arg1, int arg2)
{
	std::stringstream fileEvent;
	fileEvent << type << "; " << overallGameClock_->getTime() << "; " << arg1 << "; " << arg2 << "\n";
	logFile_ << fileEvent.str();
}

void LogClass::setEndTime()
{
	std::stringstream startTime;
	startTime << "End; " << overallGameClock_->getTime() << "; " << getYear() << "-" << getMonth() << "-" << getDay() << " " << getHours() << ":" << getMinutes() << ":" << getSeconds() << "; 0\n";
	logFile_ << startTime.str();

	logFile_.close();
}

LogClass* LogClass::Instance()
{
	return (&logClass_);
}

int LogClass::getSeconds(){
	time_t 	curr;
	tm 		local;
	time(&curr);
	localtime_s(&local, &curr);
	return local.tm_sec;
}

//--------------------------------------------------
int LogClass::getMinutes(){
	time_t 	curr;
	tm 		local;
	time(&curr);
	localtime_s(&local, &curr);
	return local.tm_min;
}

//--------------------------------------------------
int LogClass::getHours(){
	time_t 	curr;
	tm 		local;
	time(&curr);
	localtime_s(&local, &curr);
	return local.tm_hour;
}

//--------------------------------------------------
int LogClass::getYear(){
  time_t    curr;
  tm       local;
  time(&curr);
  localtime_s(&local, &curr);
  int year = local.tm_year + 1900;
  return year;
}

//--------------------------------------------------
int LogClass::getMonth(){
  time_t    curr;
  tm       local;
  time(&curr);
  localtime_s(&local, &curr);
  int month = local.tm_mon + 1;
  return month;
}

//--------------------------------------------------
int LogClass::getDay(){
  time_t    curr;
  tm       local;
  time(&curr);
  localtime_s(&local, &curr);
  return local.tm_mday;
}
#ifndef _CLOCK_CLASS_H_
#define _CLOCK_CLASS_H_

#include <windows.h>
#include <ctime>

class ClockClass 
{
	public:
		ClockClass();
		~ClockClass();

		float getTime() const;
		float getDeltaTime() const;

		void reset();
		void start();
		void stop();
		void tick();
		
	private:
		double secondsPerCount_;
		double deltaTime_;

		__int64 baseTime_;
		__int64 pausedTime_;
		__int64 stopTime_;
		__int64 previousTime_;
		__int64 currentTime_;

		bool stopped_;

};

#endif //_CLOCK_CLASS_H_

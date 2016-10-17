#include "clockClass.h"

ClockClass::ClockClass()
{
	secondsPerCount_ = 0.0;
	deltaTime_ = 0.0;
	baseTime_ = 0;
	pausedTime_ = 0;
	stopTime_ = 0;
	previousTime_ = 0;
	currentTime_ = 0;
	stopped_ = false;

	__int64 countsPerSec;
	QueryPerformanceFrequency((LARGE_INTEGER*)&countsPerSec);
	secondsPerCount_ = 1.0 / (double)countsPerSec;
}

ClockClass::~ClockClass()
{
}

void ClockClass::tick()
{
	if(stopped_)
	{
		deltaTime_ = 0.0;
		return;
	}

	// Get the time
	__int64 currentTime;
	QueryPerformanceCounter((LARGE_INTEGER*) &currentTime);
	currentTime_ = currentTime;

	// Time difference between this frame and the previous
	deltaTime_ = (currentTime_ - previousTime_)*secondsPerCount_;

	// Prepare for next frame
	previousTime_ = currentTime_;

	// Force nonnegative as if the processor goes to a power save mode
	// it can result in negative deltaTime_
	if(deltaTime_ < 0.0)
	{
		deltaTime_ = 0.0;
	}
}

float ClockClass::getDeltaTime()const
{
	return (float)deltaTime_;
}

void ClockClass::reset()
{
	__int64 currentTime;
	QueryPerformanceCounter((LARGE_INTEGER*) &currentTime);

	baseTime_ = currentTime;
	previousTime_ = currentTime;
	stopTime_ = 0;
	stopped_ = false;
}

void ClockClass::stop()
{
	// If we have already stopped we do nothing
	if(!stopped_)
	{
		__int64 currentTime;
		QueryPerformanceCounter((LARGE_INTEGER*) &currentTime);

		// We save the time at which we stopped and set the boolean
		// flag to true
		stopTime_ = currentTime;
		stopped_ = true;
	}
}

void ClockClass::start()
{
	__int64 startTime;
	QueryPerformanceCounter((LARGE_INTEGER*) &startTime);

	// Accumulate the time between stopped and start

	// If we were resuming time
	if(stopped_)
	{
		pausedTime_ += (startTime - stopTime_);

		// Since we are starting the timer back up, the current previous time
		// is not valid, as it occurred while paused.
		previousTime_ = startTime;

		// No longer stopped
		stopTime_ = 0;
		stopped_ = false;
	}
}

float ClockClass::getTime()const
{
	// If wer are stopped, do not count the time that has passed since stopped. Moreover,
	// if we previously already had a pause, the distyance stopTime_-baseTime_ includes
	// paused time, wich we do not want to count.
	if(stopped_)
	{
		return (float) (((stopTime_-pausedTime_) - baseTime_)*secondsPerCount_);
	}
	// the distance currentTime_-baseTime_ includes paused time, which we do not want to
	// count.
	else
	{
		return (float) (((currentTime_-pausedTime_) - baseTime_)*secondsPerCount_);
	}
}

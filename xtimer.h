#ifndef __XTIMER__
#define __XTIMER__

#include <time.h>
#include <iostream>

#if defined(__WIN32) || defined(__WIN64) || defined(WIN32) || defined(WIN64)
#include <windows.h>
#else
#include <sys/time.h>
#endif

class timer
{
public:
  //запустить таймер
  virtual void start() = 0;
  //остановить таймер
  virtual void stop() = 0;
  //получить временной отрезок между текущим замером и предыдущим
  //обратите внимание: время здесь не замеряется,
  //только лишь возвращается уже замерянный результат
  virtual double getTimeInterval() = 0;
};

class tmgeneric: public timer
{

	double currentCheckTime; //текущий замер времени
	double lastCheckTime; //предыдущий замер времени
	double gpuTime;
public:
	tmgeneric():currentCheckTime(0),lastCheckTime(0),gpuTime(0) {}
	~tmgeneric() {}

	void start()
	{
		currentCheckTime = (double)clock();
	}

	void stop()
	{
		lastCheckTime = currentCheckTime;
		currentCheckTime = (double)clock();
	}

	double getTimeInterval()
	{
		gpuTime = (currentCheckTime - lastCheckTime)/CLOCKS_PER_SEC;
		return gpuTime;
	}

	void printTimeInterval(std::ostream& x_msg_stream, const char* comment)
	{
		x_msg_stream << comment << ": " << getTimeInterval() << " secs.\n";
	}

};

class xtimer: public timer
{
	double currentCheckTime; //текущий замер времени
	double lastCheckTime; //предыдущий замер времени
	double gpuTime;

	double getT()
	{
	#if defined(__WIN32) || defined(__WIN64) || defined(WIN32) || defined(WIN64)
		LARGE_INTEGER c,f;
		QueryPerformanceFrequency(&f);
		QueryPerformanceCounter(&c);
		return (double)c.QuadPart/(double)f.QuadPart;
	#else
		struct timeval tv;
		gettimeofday(&tv,0);
		return (double)tv.tv_sec+1.0e-6*(double)tv.tv_usec;
	#endif
	}

public:
	xtimer(): currentCheckTime(0),lastCheckTime(0),gpuTime(0) {}
	~xtimer() {}

	void start()
	{
		currentCheckTime = getT();
	}

	void stop()
	{
		lastCheckTime = currentCheckTime;
		currentCheckTime = getT();
	}

	double getTimeInterval()
	{
		gpuTime = currentCheckTime - lastCheckTime;
		return gpuTime;
	}

	void printTimeInterval(std::ostream& x_msg_stream, const char* comment)
	{
		x_msg_stream << comment << ": " << getTimeInterval() << " secs.\n";
	}

};

#endif

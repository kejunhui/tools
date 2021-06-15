#pragma once

class CDate
{
protected:
	int year, month, day;
	friend class CDateTime;
public:
	CDate();
	CDate(int year,int month,int day);
	int GetYear() const;
	int GetMonth() const;
	int GetDay() const;
	int doy() const;
	const char *ToDateStr(char *szDate) const; // 日期串: yyyymmdd
	const char *ToDateStr2(char *szDate) const; // 日期串: yyyy-mm-dd
	bool IsLeap(int year = 0) const; // 是否为润年
	bool operator==(const CDate &dt);
	bool operator!=(const CDate &dt);
};

class CDateTime
{
protected:
	#define TIMEZONE 8
	CDate date;
	int hour, minute, second, msec;
	char datetime[32];
	void Construct(unsigned int lt, int msec);

public:
	CDateTime();
	CDateTime(const char *szDateTime);
	CDateTime(unsigned int lt, int msec = 0);
	CDateTime(int year, int month, int day, int hour = 0, int minute = 0, int second = 0, int msec = 0);
	CDateTime &operator=(const CDateTime &dt);
	CDateTime &operator=(unsigned int lt);
	void SetYear(int year);
	void SetMonth(int month);
	void SetDay(int day);
	void SetHour(int hour);
	void SetMinute(int minute);
	void SetSecond(int sceond);
	void SetDate(int year, int month, int day);
	void SetTime(int hour = 0, int minute = 0, int second = 0, int msec = 0);
	void SetDateTime(int year, int month, int day, int hour = 0, int minute = 0, int second = 0, int msec = 0);

	CDate GetDate() const;
	int GetYear() const;
	int GetMonth() const;
	int GetDay() const;
	int GetHour() const;
	int GetMinute() const;
	int GetSecond() const;
	long GetMiliSeconds() const;
	void GetTime(int &hour, int &minute, int &second) const;
	void GetDateTime(int &year, int &month, int &day, int &hour, int &minute, int &second) const;
	const char *ToTimeStr(char *szTime) const; // 时间串: hh24:mi:ss
	const char *ToTimeString(char *szTime) const; // 时间串: hh24:mi:ss.xxx
	const char *ToString(char *szDateTime) const; // 标准格式时间串: yyyy-mm-dd hh24:mi:ss
	const char *ToString2(char *szDateTime) const; // 标准格式时间串: yyyy-mm-dd hh24:mi:ss,xxx
	const char *ToString3(char *szDateTime = nullptr);
	char *Format(char *szDate, const char *format) const;
	unsigned int GetGMTSeconds() const;
	unsigned int GetSeconds() const;
	void stepMonth(int month);
	int secsTo(CDateTime &t);
	
	static unsigned int Time();
	static unsigned int ToSeconds(const char *szDateTime);
	static char *Format(int lt, char *szDate, const char *format);
	static char *CurrTime(char *szDate, const char *format = "%Y-%m-%d %H:%M:%S");
	static CDateTime currentDateTime();
	CDateTime toUTC();
};

#ifndef time64_t
	typedef long long time64_t; 
#endif
class PTimer
{
	time64_t m_tBegin, m_nTimeOut;
public:
	PTimer(time64_t nTimerOut = 0);
	void ReStart(time64_t nTimeOut = 0);
	bool IsTimeOutNow();
	time64_t GetCountValue();
};

#ifndef _WIN32
#include <sys/time.h>
class CProgress_timer
{
	char m_str[32];
	struct timeval m_tv;
	CProgress_timer();
public:
	CProgress_timer(const char *str);
	~CProgress_timer();
};

#else
class CProgress_timer
{
	char m_str[32];
	int m_sec, m_msec;
	CProgress_timer();
public:
	CProgress_timer(const char *str);
	~CProgress_timer();
};
#endif

long long GetCPUTime();
long long GetMiliSeconds();
long long GetSeconds();

unsigned long long GetNTPTime();
unsigned int GetCompactNTPTimestamp(unsigned long long ntpTimestamp);


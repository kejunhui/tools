#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "datetime.h"
#ifdef _WIN32
#pragma warning(disable : 4996)
#endif

CDate::CDate():year(0),month(0),day(0)
{

}

CDate::CDate(int y, int m, int d) : year(y), month(m), day(d)
{

}

int CDate::GetYear() const
{
	return year;
}

int CDate::GetMonth() const
{
	return month;
}

int CDate::GetDay() const
{
	return day;
}

const char *CDate::ToDateStr(char *szDate) const // 日期串: yyyymmdd
{
	sprintf(szDate, "%d%02d%02d", year, month, day);
	return szDate;
}

const char *CDate::ToDateStr2(char *szDate) const // 日期串: yyyy-mm-dd
{
	sprintf(szDate, "%d-%02d-%02d", year, month, day);
	return szDate;
}

bool CDate::IsLeap(int year) const
{
	if (year == 0) year = this->year;
	if (year % 400 == 0) return true;
	return (year % 4 == 0 && year % 100 != 0);
}

int CDate::doy() const
{
	int sum = 0, leap;
	switch (month) /*先计算某月以前月份的总天数*/
	{
	case 1:sum = 0; break;
	case 2:sum = 31; break;
	case 3:sum = 59; break;
	case 4:sum = 90; break;
	case 5:sum = 120; break;
	case 6:sum = 151; break;
	case 7:sum = 181; break;
	case 8:sum = 212; break;
	case 9:sum = 243; break;
	case 10:sum = 273; break;
	case 11:sum = 304; break;
	case 12:sum = 334; break;
	default:printf("data error"); break;
	}
	sum = sum + day; /*再加上某天的天数*/
	if (year % 400 == 0 || (year % 4 == 0 && year % 100 != 0)) /*判断是不是闰年*/
	{
		leap = 1;
	}
	else
	{
		leap = 0;
	}
	if (leap == 1 && month > 2) /*如果是闰年且月份大于2,总天数应该加一天*/
	{
		sum++;
	}
	return sum;
}
bool CDate::operator==(const CDate &dt)
{
	if (dt.year == year && dt.month == month && dt.day == day)
	{
		return true;
	}
	return false;
}

bool CDate::operator!=(const CDate &dt)
{
	if (dt.year != year || dt.month != month || dt.day != day)
	{
		return true;
	}
	return false;
}

void CDateTime::Construct(unsigned int lt, int msec)
{
	int nDays = lt / 86400, nSecs = lt % 86400;
	int days[][13] = { { 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365 }, 
		{ 0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335, 366 } };

	date.year = 1970 + (nDays / 1461) * 4;
	for(nDays %= 1461; nDays >= 366; ++date.year)
		nDays -= (date.IsLeap() ? 366 : 365);

	int *p = days[date.IsLeap()];
	if(nDays == 365 && p == days[0])
	{
		++date.year;
		date.month = date.day = 1;
	}
	else for(int i = 1; /*i < 13*/; ++i)
		if(p[i] > nDays)
		{
			date.day = nDays - p[i - 1] + 1;
			date.month = i;
			break;
		}

	hour = nSecs / 3600;
	minute = (nSecs % 3600) / 60;
	second = nSecs % 60;
	this->msec = msec;
}

CDateTime::CDateTime()
{
#ifndef _WIN32
	struct timeval tv;
	gettimeofday(&tv, NULL);
	Construct(tv.tv_sec + TIMEZONE * 3600, tv.tv_usec / 1000);
#else
	Construct((unsigned int)time(NULL) + TIMEZONE * 3600, 0);
#endif
}

CDateTime::CDateTime(const char *szDateTime)
{
	char szTime[16];
	int year, month, day, hour, minute, second;
	memcpy(szTime, szDateTime, 14);
	szTime[14] = '\0';
	second = atoi(&szTime[12]);
	szTime[12] = '\0';
	minute = atoi(&szTime[10]);
	szTime[10] = '\0';
	hour = atoi(&szTime[8]);
	szTime[8] = '\0';
	day = atoi(&szTime[6]);
	szTime[6] = '\0';
	month = atoi(&szTime[4]);
	szTime[4] = '\0';
	year = atoi(&szTime[0]);
	SetDate(year, month, day);
	SetTime(hour, minute, second, 0);
}

CDateTime::CDateTime(unsigned int lt, int msec)
{
	Construct(lt, msec);
}

CDateTime::CDateTime(int year, int month, int day, int hour, int minute, int second, int msec)
{
	SetDate(year, month, day);
	SetTime(hour, minute, second, msec);
}

void CDateTime::SetDate(int year, int month, int day)
{
	if(year >= 1970 && year <= 2099) this->date.year = year;
	else this->date.year = year = (year > 2099 ? 2099 : 1970);
	if(month > 0 && month < 13) this->date.month = month;
	else this->date.month = month = (month <= 0 ? 1 : 12);

	if(day <= 28) this->date.day = (day > 0 ? day : 1);
	else if(month == 2) this->date.day = (date.IsLeap() ? 29 : 28);
	else if(day < 31) this->date.day = day;
	else this->date.day = (month == 4 || month == 6 || month == 9 || month == 11) ? 30 : 31;
}

void CDateTime::SetTime(int hour, int minute, int second, int msec)
{
	this->hour = (hour >= 0 && hour < 24) ? hour : 0;
	this->minute = (minute >= 0 && minute < 60) ? minute : 0;
	this->second = (second >= 0 && second < 60) ? second : 0;
	this->msec = (msec >= 0 && msec < 1000) ? msec : 0;
}

void CDateTime::SetDateTime(int year, int month, int day, int hour, int minute, int second, int msec)
{
	SetDate(year, month, day);
	SetTime(hour, minute, second, msec);
}

CDateTime &CDateTime::operator=(const CDateTime &dt)
{
	memcpy(this, &dt, sizeof(CDateTime));
	return *this;
}

CDateTime &CDateTime::operator=(unsigned int lt)
{
	Construct(lt, 0);
	return *this;
}

void CDateTime::SetYear(int year)
{
	if(year < 1970) year = 1970;
	else if(year > 2099) year = 2099;
	if(date.month == 2 && date.day == 29 && !date.IsLeap(year)) --date.day;
	this->date.year = year;
}

void CDateTime::SetMonth(int month)
{
	if(month < 1) month = 1;
	else if(month > 12) month = 12;
	if((month == 4 || month == 6 || month == 9 || month == 11) && date.day == 31) --date.day;
	else if(month == 2 && date.day > 28) date.day = date.IsLeap() ? 29 : 28;
	this->date.month = month;
}

void CDateTime::SetDay(int day)
{
	if(day < 1) day = 1;
	else if(day > 31) day = 31;
	if((date.month == 4 || date.month == 6 || date.month == 9 || date.month == 11) && day == 31) --day;
	else if(date.month == 2 && day > 28) day = date.IsLeap() ? 29 : 28;
	this->date.day = day;
}

void CDateTime::SetHour(int hour)
{
	this->hour = (hour >= 0 && hour < 24) ? hour : 0;
}

void CDateTime::SetMinute(int minute)
{
	this->minute = (minute >= 0 && minute < 60) ? minute : 0;
}

void CDateTime::SetSecond(int second)
{
	this->second = (second >= 0 && second < 60) ? second : 0;
}

int CDateTime::GetYear() const { return date.year; }
int CDateTime::GetMonth() const { return date.month; }
int CDateTime::GetDay() const { return date.day; }
int CDateTime::GetHour() const { return hour; }
int CDateTime::GetMinute() const { return minute; }
int CDateTime::GetSecond() const { return second; }

CDate CDateTime::GetDate() const
{
	return date;
}

void CDateTime::GetTime(int &hour, int &minute, int &second) const
{
	hour = this->hour;
	minute = this->minute;
	second = this->second;
}

void CDateTime::GetDateTime(int &year, int &month, int &day, int &hour, int &minute, int &second) const
{
	year = this->date.year;
	month = this->date.month;
	day = this->date.day;
	hour = this->hour;
	minute = this->minute;
	second = this->second;
}

const char *CDateTime::ToTimeStr(char *szTime) const
{
	sprintf(szTime, "%02d:%02d:%02d", hour, minute, second);
	return szTime;
}

const char *CDateTime::ToTimeString(char *szTime) const
{
	sprintf(szTime, "%02d:%02d:%02d.%03d", hour, minute, second, msec);
	return szTime;
}

const char *CDateTime::ToString(char *szDateTime) const
{
	sprintf(szDateTime, "%d-%02d-%02d %02d:%02d:%02d", date.year, date.month, date.day, hour, minute, second);
	return szDateTime;
}

const char *CDateTime::ToString2(char *szDateTime) const
{
	sprintf(szDateTime, "%d-%02d-%02d %02d:%02d:%02d,%03d", 
		date.year, date.month, date.day, hour, minute, second, msec);
	return szDateTime;
}

const char *CDateTime::ToString3(char *szDateTime)
{
	if (szDateTime != nullptr)
	{
		sprintf(szDateTime, "%d%02d%02d%02d%02d%02d\0", date.year, date.month, date.day, hour, minute, second);
		return szDateTime;
	}
	sprintf(datetime, "%d%02d%02d%02d%02d%02d\0", date.year, date.month, date.day, hour, minute, second);
	return datetime;
}

char *CDateTime::Format(char *szDateTime, const char *format) const
{
	int i, j = 0;
	for(; *format != '\0'; ++format)
		if(*format == '%')
		{
			switch(*(++format))
			{
			case 'Y': i = date.year; break;
			case 'm': i = date.month; break;
			case 'd': i = date.day; break;
			case 'H': i = hour; break;
			case 'M': i = minute; break;
			case 'S': i = second; break;
			case '%': i = -1; break;
			default: i = (--format, -2);
			}
			if(i >= 1970) sprintf(szDateTime + j, "%d", i), j += 4;
			else if(i >= 0) sprintf(szDateTime + j, "%02d", i), j += 2;
			else if(i == -1) szDateTime[j++] = '%';
		}
		else szDateTime[j++] = *format;
	szDateTime[j] = '\0';
	return szDateTime;
}

unsigned int CDateTime::GetGMTSeconds() const
{
	return GetSeconds() - 3600 * TIMEZONE;
}

long CDateTime::GetMiliSeconds() const
{
	return GetGMTSeconds() * 1000l + msec;
}

unsigned int CDateTime::GetSeconds() const
{
	int nDays, nLeaps = (date.year > 1972 ? (date.year - 1973) / 4 + 1 : 0);
	int days[12] = { 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334 };
	//for(int i = 1972; i < year; i += 4) if(year % 400 == 0 || (year % 100)) ++nLeaps;
	nDays = (date.year - 1970) * 365 + nLeaps + days[date.month - 1] + date.day - 1;
	if(date.month > 2 && this->date.IsLeap()) ++nDays;
	return 86400 * nDays + hour * 3600 + minute * 60 + second;
}

void CDateTime::stepMonth(int month)
{
	date.year += month / 12;
	if((month = month % 12) == 0) /*return*/;
	else if((this->date.month += month) > 12) this->date.month -= (++date.year, 12);
	else if(this->date.month < 1) this->date.month += (--date.year, 12);
	if(date.day == 29 && this->date.month == 2 && !date.IsLeap())
		this->date.month = (date.day = 1) + 2;
}

unsigned int CDateTime::Time()
{
	return (unsigned int)time(NULL) + TIMEZONE * 3600;
}

unsigned int CDateTime::ToSeconds(const char *szDateTime)
{
	char szTime[32]; //yyyy-mm-dd hh24:mi:ss
	int year, month, day, hour, minute, second, nDays, nLeaps;
	int days[12] = { 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334 };

	if(strlen(strcpy(szTime, szDateTime)) < 19) return 0;
	szTime[4] = szTime[7] = szTime[10] = szTime[13] = szTime[16] = '\0';
	year = atoi(szTime);
	month = atoi(szTime + 5);
	day = atoi(szTime + 8);
	hour = atoi(szTime + 11);
	minute = atoi(szTime + 14);
	second = atoi(szTime + 17);
	nLeaps = (year > 1972 ? (year - 1973) / 4 + 1 : 0);
	//for(int i = 1972; i < year; i += 4) if(year % 400 == 0 || (year % 100)) ++nLeaps;
	nDays = (year - 1970) * 365 + nLeaps + days[month - 1] + day - 1;
	if(month > 2 && (year % 400 == 0 || (year % 4 == 0 && year % 100 != 0))) ++nDays;
	return 86400 * nDays + hour * 3600 + minute * 60 + second;
}

int CDateTime::secsTo(CDateTime &t)
{
	return  t.GetSeconds() - GetSeconds();	
}

char *CDateTime::CurrTime(char *szDate, const char *format)
{
	CDateTime dt;
	return dt.Format(szDate, format);
}

CDateTime CDateTime::currentDateTime()
{
	CDateTime dt;
	return dt;
}

CDateTime CDateTime::toUTC()
{
	return CDateTime(GetGMTSeconds(), msec);
}

char *CDateTime::Format(int lt, char *szDate, const char *format)
{
	CDateTime dt(lt);
	return dt.Format(szDate, format);
}

PTimer::PTimer(time64_t nTimeOut)
{
	ReStart(nTimeOut);
}

bool PTimer::IsTimeOutNow()
{
	return (GetCountValue() - m_nTimeOut > 0);
}

#ifndef _WIN32

void PTimer::ReStart(time64_t nTimeOut)
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	m_tBegin = tv.tv_sec * 1000000ll + tv.tv_usec;
	m_nTimeOut = nTimeOut;
}

time64_t PTimer::GetCountValue()
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	time64_t nCountValue = tv.tv_sec * 1000000ll + tv.tv_usec - m_tBegin;
	if(nCountValue < 0) m_tBegin = tv.tv_sec * 1000000ll + tv.tv_usec;
	return nCountValue;
}

CProgress_timer::CProgress_timer(const char *str)
{
	strcpy(m_str, str);
	gettimeofday(&m_tv, NULL);
	m_tv.tv_sec += 3600 * TIMEZONE;
}

#include <errno.h>
#include <pthread.h>
CProgress_timer::~CProgress_timer()
{
	char szTime[32];
	struct timeval tv;
	gettimeofday(&tv, NULL);
	tv.tv_sec += 3600 * TIMEZONE;
	time64_t lt = (tv.tv_sec - m_tv.tv_sec) * 1000000ll + tv.tv_usec - m_tv.tv_usec;
	fprintf(stderr, "%s, %lu, %s,%06d, [%d, %06d]\n", m_str, pthread_self(), CDateTime
		(m_tv.tv_sec).ToTimeStr(szTime), (int)m_tv.tv_usec, int(lt / 1000000), int(lt % 1000000));
}

long long GetCPUTime()
{
	timespec time;
	if(clock_gettime(CLOCK_MONOTONIC, &time) == 0)
		return (long long)time.tv_sec * 1000 + time.tv_nsec / 1000000;
	fprintf(stderr, "clock_gettime error, errno = %u\n", errno);
	return -1;
}

long long GetMiliSeconds()
{
	struct timeval stv;
	if(gettimeofday(&stv, NULL) != -1)
		return (long long)stv.tv_sec * 1000 + stv.tv_usec / 1000;
	fprintf(stderr, "gettimeofday error, errno = %u\n", errno);
	return -1;
}

long long GetSeconds()
{
	struct timeval stv;
	if(gettimeofday(&stv, NULL) != -1)
		return (long long)stv.tv_sec;
	fprintf(stderr, "gettimeofday error, errno = %u\n", errno);
	return -1;
}

#else // windows

#include <windows.h>
void PTimer::ReStart(time64_t nTimeOut)
{
	SYSTEMTIME sys;
	GetLocalTime(&sys);
	CDateTime dt(sys.wYear, sys.wMonth, sys.wDay);
	dt.SetTime(sys.wHour, sys.wMinute, sys.wSecond);
	m_tBegin = dt.GetSeconds() * 1000ll + sys.wMilliseconds;
	m_nTimeOut = (time64_t)(nTimeOut * 0.001);
}

time64_t PTimer::GetCountValue()
{
	SYSTEMTIME sys;
	GetLocalTime(&sys);
	CDateTime dt(sys.wYear, sys.wMonth, sys.wDay);
	dt.SetTime(sys.wHour, sys.wMinute, sys.wSecond);
	time64_t nCountValue = dt.GetSeconds() * 1000ll + sys.wMilliseconds - m_tBegin;
	if(nCountValue < 0) m_tBegin = dt.GetSeconds() * 1000ll + sys.wMilliseconds;
	return nCountValue;
}

CProgress_timer::CProgress_timer(const char *str)
{
	SYSTEMTIME sys;
	GetLocalTime(&sys);
	strcpy(m_str, str);
	CDateTime dt(sys.wYear, sys.wMonth, sys.wDay);
	dt.SetTime(sys.wHour, sys.wMinute, sys.wSecond);
	m_sec = dt.GetSeconds(), m_msec = sys.wMilliseconds;
}

CProgress_timer::~CProgress_timer()
{
	int sec, msec;
	char szTime[32];
	SYSTEMTIME sys;
	GetLocalTime(&sys);
	CDateTime dt(sys.wYear, sys.wMonth, sys.wDay);
	dt.SetTime(sys.wHour, sys.wMinute, sys.wSecond);
	sec = dt.GetSeconds() - m_sec;
	if((msec = sys.wMilliseconds - m_msec) < 0) --sec, msec += 1000;
	fprintf(stderr, "%s, %lu, %s,%03d, [%d, %03d]\n", m_str, 
		GetCurrentThreadId(), CDateTime(m_sec).ToTimeStr(szTime), m_msec, sec, msec);
}

#pragma comment(lib, "winmm.lib")
long long GetCPUTime()
{
	return timeGetTime();
}

long long GetMiliSeconds()
{
	SYSTEMTIME sys;
	GetLocalTime(&sys);
	CDateTime dt(sys.wYear, sys.wMonth, sys.wDay);
	dt.SetTime(sys.wHour, sys.wMinute, sys.wSecond);
	return dt.GetGMTSeconds() * 1000ll + sys.wMilliseconds;
}

long long GetSeconds()
{
	SYSTEMTIME sys;
	GetLocalTime(&sys);
	CDateTime dt(sys.wYear, sys.wMonth, sys.wDay);
	dt.SetTime(sys.wHour, sys.wMinute, sys.wSecond);
	return dt.GetGMTSeconds();
}

#endif

unsigned long long GetNTPTime()
{
	unsigned long long msFrom1900To1970 = 2208988800000ull;
	unsigned long long factorFromMsToNtpUnit = 4294967ull;
	return (GetMiliSeconds() + msFrom1900To1970) * factorFromMsToNtpUnit;
}

unsigned int GetCompactNTPTimestamp(unsigned long long ntpTimestamp)
{
	return ((ntpTimestamp >> 16) & 0xffffffff);
}

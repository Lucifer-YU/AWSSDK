/*
 * DateTime.cpp
 *
 *  Created on: Dec 19, 2014
 *      Author: Lucifer
 */

#include "Foundation.h"

#ifndef	WIN32
#include <sys/time.h>
#endif
#include <limits.h>
#ifndef LONG_LONG_MIN
#define	LONG_LONG_MIN	LLONG_MIN
#define	LONG_LONG_MAX	ULLONG_MAX
#endif

#define LOG_TAG "DateTime"
#define DATETIME_DEFAULT_FORMAT "%04d-%02d-%02d %02d:%02d:%02d.%03d"

// Number of 1ms ticks per time unit
static const int64_t TicksPerMillisecond = 1;
static const int64_t TicksPerSecond = TicksPerMillisecond * 1000;
static const int64_t TicksPerMinute = TicksPerSecond * 60;
static const int64_t TicksPerHour = TicksPerMinute * 60;
static const int64_t TicksPerDay = TicksPerHour * 24;
static const int DaysPerYear = 365;
static const int DaysPer4Years = DaysPerYear * 4 + 1;
static const int DaysPer100Years = DaysPer4Years * 25 - 1;
static const int DaysPer400Years = DaysPer100Years * 4 + 1;

// Number of days from 1/1/0001 to 12/31/1969
static const int DaysTo1970 = DaysPer400Years * 4 + DaysPer100Years * 3
		+ (DaysPer4Years * 17) + (DaysPerYear * 1);
static const int64_t TicksTo1970 = DaysTo1970 * TicksPerDay;

static const int DaysToMonth365[] = { 0, 31, 59, 90, 120, 151, 181, 212, 243,
		273, 304, 334, 365 };
static const int DaysToMonth366[] = { 0, 31, 60, 91, 121, 152, 182, 213, 244,
		274, 305, 335, 366 };

DateTime::DateTime() {
	_ticks = (currentMillisecondsSince1970() * TicksPerMillisecond) + TicksTo1970;
	_timezoneOffset = currentTimezoneOffset();
	_ticks -= (_timezoneOffset * TicksPerMinute);
}
DateTime::DateTime(int64_t milliseconds) {
	_ticks = (milliseconds * TicksPerMillisecond) + TicksTo1970;
	_timezoneOffset = currentTimezoneOffset();
	_ticks -= (_timezoneOffset * TicksPerMinute);
}
DateTime::DateTime(int year, int month, int day, int hour, int minute,
		int second) {
	_ticks = (int64_t) (dateToTicks(year, month, day)
			+ timeToTicks(hour, minute, second));
	_timezoneOffset = currentTimezoneOffset();
	_ticks -= (_timezoneOffset * TicksPerMinute);
}
DateTime::DateTime(const DateTime& datetime) {
	_ticks = datetime._ticks;
	_timezoneOffset = datetime._timezoneOffset;
}
DateTime::~DateTime() {
}

DateTime& DateTime::operator =(const DateTime& datetime) {
	_ticks = datetime._ticks;
	_timezoneOffset = datetime._timezoneOffset;
	return (*this);
}
DateTime DateTime::toUTC() const {
	DateTime result(*this);
	result._ticks += (_timezoneOffset * TicksPerMinute);
	result._timezoneOffset = 0;
	return result;
}
int DateTime::getYear() const {
	return getDateField(DF_Year);
}

int DateTime::getMonth() const {
	return getDateField(DF_Month);
}

int DateTime::getDay() const {
	return getDateField(DF_Day);
}

// Returns the hour field of this DateTime. The returned value is an integer between 0 and 23.
int DateTime::getHour() const {
	return (int) ((_ticks / TicksPerHour) % 24);
}

int DateTime::getMinute() const {
	return (int) ((_ticks / TicksPerMinute) % 60);
}

// Returns the second field of this DateTime. The returned value is an integer between 0 and 59.
int DateTime::getSecond() const {
	return (int) ((_ticks / TicksPerSecond) % 60);
}

int DateTime::getMillisecond() const {
	return (int)((_ticks/ TicksPerMillisecond) % 1000);
}
uint64_t DateTime::getMillisecndsSince1970() const {
	return ((_ticks - TicksTo1970) / TicksPerMillisecond)
			+ (_timezoneOffset * TicksPerMinute);
}

// Returns a given date field of this DateTime. This method is used
// to compute the year, day-of-year, month, or day field.
int DateTime::getDateField(DateField field) const {
	int64_t ticks = _ticks;
	// n = number of days since 1/1/0001
	int n = (int) (ticks / TicksPerDay);
	// y400 = number of whole 400-year periods since 1/1/0001
	int y400 = n / DaysPer400Years;
	// n = day number within 400-year period
	n -= y400 * DaysPer400Years;
	// y100 = number of whole 100-year periods within 400-year period
	int y100 = n / DaysPer100Years;
	// Last 100-year period has an extra day, so decrement result if 4
	if (y100 == 4)
		y100 = 3;
	// n = day number within 100-year period
	n -= y100 * DaysPer100Years;
	// y4 = number of whole 4-year periods within 100-year period
	int y4 = n / DaysPer4Years;
	// n = day number within 4-year period
	n -= y4 * DaysPer4Years;
	// y1 = number of whole years within 4-year period
	int y1 = n / DaysPerYear;
	// Last year has an extra day, so decrement result if 4
	if (y1 == 4)
		y1 = 3;
	// If year was requested, compute and return it
	if (field == DF_Year) {
		return y400 * 400 + y100 * 100 + y4 * 4 + y1 + 1;
	}
	// n = day number within year
	n -= y1 * DaysPerYear;
	// If day-of-year was requested, return it
	if (field == DF_DayOfYear)
		return n + 1;
	// Leap year calculation looks different from IsLeapYear since y1, y4,
	// and y100 are relative to year 1, not year 0
	bool leapYear = y1 == 3 && (y4 != 24 || y100 == 3);
	const int* days = leapYear ? DaysToMonth366 : DaysToMonth365;
	// All months have less than 32 days, so n >> 5 is a good conservative
	// estimate for the month
	int m = (n >> 5) + 1;
	// m = 1-based month number
	while (n >= days[m])
		m++;
	// If month was requested, return it
	if (field == DF_Month)
		return m;
	// Return 1-based day-of-month
	return n - days[m - 1] + 1;
}

bool DateTime::parse(const char* str, const char* format) {
	LOGI("DateTime::parse(str: %s, format:%s)", str, format);

	BFX_ASSERT(str);

	int year = 0;
	int month = 0;
	int day = 0;
	int hour = 0;
	int minute = 0;
	int second = 0;
	int millisec = 0;
	int ret;

	if (format == NULL)
		format = DATETIME_DEFAULT_FORMAT;

	// 'yyyy-MM-dd'
	// 'yyyy-MM-dd HH:mm:ss'
	// 'yyyy-MM-dd HH:mm:ss.SSS'
	ret = sscanf(str, format, &year, &month, &day, &hour, &minute, &second,
			&millisec);

	if (ret <= 0) {
		// 0: indicates that no fields were assigned.
		// EOF: for an error or if the end of the string is reached before the first conversion.
		LOGE("Failed to parse datetime:'%s' with format:'%s'.", str, format);
		return false;
	}

	LOGI("Parsed time: " DATETIME_DEFAULT_FORMAT, year, month, day, hour,
			minute, second, millisec);

	_ticks = (int64_t) (dateToTicks(year, month, day)
			+ timeToTicks(hour, minute, second));

	return true;
}

// Converts the value of the current DateTime to its equivalent string representation.
String DateTime::format(const char* format) const {
	if (format == NULL)
		format = DATETIME_DEFAULT_FORMAT;

	return String::format(format, getYear(), getMonth(), getDay(), getHour(),
			getMinute(), getSecond(), getMillisecond());
}

// Checks whether a given year is a leap year. This method returns true if
// year is a leap year, or false if not.
bool DateTime::isLeapYear(int year) {
	BFX_ASSERT(year > 0 && year < 9999);

	if (year < 1 || year > 9999) {
		LOGE("The year:'%d' out of range.", year);
		return false;
	}
	return year % 4 == 0 && (year % 100 != 0 || year % 400 == 0);
}

// Returns the tick count corresponding to the given year, month, and day.
// Will check the if the parameters are valid.
int64_t DateTime::dateToTicks(int year, int month, int day) const {
	if (year >= 1 && year <= 9999 && month >= 1 && month <= 12) {
		const int* days = isLeapYear(year)? DaysToMonth366: DaysToMonth365;
		if (day >= 1 && day <= days[month] - days[month - 1]) {
			int y = year - 1;
			int n = y * 365 + y / 4 - y / 100 + y / 400 + days[month - 1] + day
					- 1;
			return n * TicksPerDay;
		}
	}
	LOGE("The year:'%d', month:'%d', day:'%d' out of range.", year, month, day);
	BFX_ASSERT(false);
	return -1;
}

// Return the tick count corresponding to the given hour, minute, second.
// Will check the if the parameters are valid.
int64_t DateTime::timeToTicks(int hour, int minute, int second) const {
	//we need to put some error checking out here.
	if (hour >= 0 && hour < 24 && minute >= 0 && minute < 60 && second >= 0
			&& second < 60) {
		// totalSeconds is bounded by 2^31 * 2^12 + 2^31 * 2^8 + 2^31,
		// which is less than 2^44, meaning we won't overflow totalSeconds.
		const int64_t MaxSeconds = LONG_LONG_MAX / TicksPerSecond;
		const int64_t MinSeconds = LONG_LONG_MIN / TicksPerSecond;
		int64_t totalSeconds = (int64_t) hour * 3600 + (int64_t) minute * 60
				+ (int64_t) second;
		if (totalSeconds > MaxSeconds || totalSeconds < MinSeconds) {
			goto Failure;
		}
		return totalSeconds * TicksPerSecond;
	}
Failure:
	LOGE("The hour:'%d', minute:'%d', second:'%d' out of range.", hour, minute,
			second);
	BFX_ASSERT(false);
	return -1;
}

#ifdef WIN32

#include <time.h>

struct timezone {
  int tz_minuteswest;
  int tz_dsttime;
};

int gettimeofday(struct timeval * __p, struct timezone *__tz) {
	if (__p) {
		time_t clock;
		struct tm tm;
		SYSTEMTIME wtm;

		GetLocalTime(&wtm);
		tm.tm_year = wtm.wYear - 1900;
		tm.tm_mon = wtm.wMonth - 1;
		tm.tm_mday = wtm.wDay;
		tm.tm_hour = wtm.wHour;
		tm.tm_min = wtm.wMinute;
		tm.tm_sec = wtm.wSecond;
		tm. tm_isdst = -1;
		clock = mktime(&tm);
		__p->tv_sec = (long)clock;
		__p->tv_usec = wtm.wMilliseconds * 1000;
	}
	if (__tz) {
		static int _tzflag = 0;
		if (!_tzflag) {
			_tzset();
			_tzflag++;
		}
		__tz->tz_minuteswest = _timezone / 60;
		__tz->tz_dsttime = _daylight;
	}
	return (0);
}

#endif	// WIN32

// Returns the current time in milliseconds since midnight, January 1, 1970 UTC.
int64_t DateTime::currentMillisecondsSince1970() {
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return (((int64_t) tv.tv_sec * 1000) + (tv.tv_usec / 1000));// - (tz.tz_minuteswest * 60 * 1000));
}

// Returns the offset, measured in minutes, for the local time zone relative to UTC
int DateTime::currentTimezoneOffset() {
	struct timeval tv;
	struct timezone tz;
	gettimeofday(&tv, &tz);
	return tz.tz_minuteswest;
}

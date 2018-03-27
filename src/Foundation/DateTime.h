/*
 * DateTime.h
 *
 *  Created on: Dec 19, 2014
 *      Author: Lucifer
 */

#ifndef __TEST_MESSAGES_DATETIME_H_
#define __TEST_MESSAGES_DATETIME_H_

#include "../Foundation/Foundation.h"
#include <stdio.h>
#include <time.h>

class DateTime {
public:
	DateTime();
	DateTime(int64_t milliseconds);
	DateTime(int year, int month, int day, int hour, int minute, int second);
	DateTime(const DateTime& datetime);

	virtual ~DateTime();

	DateTime& operator =(const DateTime& datetime);
	DateTime toUTC() const;

	int64_t getTicks() const;
	int getYear() const;
	int getMonth() const;
	int getDay() const;
	int getHour() const;
	int getMinute() const;
	int getSecond() const;
	int getMillisecond() const;
	// Returns the number of milliseconds since January 1, 1970, 00:00:00 GMT
	uint64_t getMillisecndsSince1970() const;
	// Converts the string representation of a date and time to its DateTime equivalent.
	bool parse(const char* str, const char* format = NULL);
	// Converts the value of the current DateTime to its equivalent string representation.
	String format(const char* format = NULL) const;

	// Returns the current time in milliseconds since midnight, January 1, 1970 UTC.
	static int64_t currentMillisecondsSince1970();
	// Returns the offset, measured in minutes, for the local time zone relative to UTC
	static int currentTimezoneOffset();

	// Checks whether a given year is a leap year. This method returns true if
	// year is a leap year, or false if not.
	static bool isLeapYear(int year);

private:
	// Returns a given date field of this DateTime. This method is used
	// to compute the year, day-of-year, month, or day field.
	enum DateField {
		DF_Year = 0,
		DF_DayOfYear = 1,
		DF_Month = 2,
		DF_Day = 3
	};
	int getDateField(DateField field) const;
	// Returns the tick count corresponding to the given year, month, and day.
	// Will check the if the parameters are valid.
	int64_t dateToTicks(int year, int month, int day) const;
	// Return the tick count corresponding to the given hour, minute, second.
	// Will check the if the parameters are valid.
	int64_t timeToTicks(int hour, int minute, int second) const;

	int64_t _ticks;
	int _timezoneOffset;
};

#endif /* __TEST_MESSAGES_DATETIME_H_ */

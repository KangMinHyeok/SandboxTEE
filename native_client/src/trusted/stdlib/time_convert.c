#include <stdint.h> 
#include "native_client/src/trusted/stdlib/time.h"

uint8_t month_days[13] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

uint8_t timestamp_to_weekday(timestamp_t timestamp_sec)
{
	uint8_t result = (timestamp_sec / ONE_DAY + UTC_TIME_WEEKDAY_OFFSET) % 7;
	if (result == 0) {
		result = 7;
	}
	return result;
}

int is_leap_year(uint16_t year)
{
	if ((year % 4 == 0 && ((year % 100) != 0)) || ((year % 400) == 0)) {
		return 0;
	}
	else
		return -1;
}

void utc_timestamp_to_date(timestamp_t timestamp, datedata_t* date, timedata_t* time)
{

    uint8_t  month;
	uint32_t days;
	uint16_t days_in_year;
	uint16_t year;
	timestamp_t second_in_day;

	second_in_day = timestamp % ONE_DAY;
	time->second = second_in_day % 60;

	second_in_day /= 60;
	time->minute = second_in_day % 60;

	second_in_day /= 60;
	time->hour = (second_in_day % 24) + 9;
	

	days = timestamp / ONE_DAY;
	
	for (year = 1970; year <= 2200; year++) {
		if (is_leap_year(year))
			days_in_year = 365;
		else
			days_in_year = 366;

		if (days >= days_in_year)
			days -= days_in_year;
		else
			break;
	}
	
	date->year = year;
	if (!is_leap_year(date->year)) 
		month_days[2] = 29; 
	else
		month_days[2] = 28;

	for (month = 1; month <= 12; month++) {
		if (days >= month_days[month])
			days -= month_days[month];
		else
			break;
	}
	date->month = month;
	date->day = days + 1;
}

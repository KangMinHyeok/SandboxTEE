#ifndef TIME_H
#define TIME_H

typedef uint32_t timestamp_t; //seconds

typedef struct {
    uint16_t    year;
    uint8_t     month;
    uint8_t     day;
    uint8_t     hour;
    uint8_t     minute;
    uint8_t     second;
    uint8_t     week;
    uint8_t     weekday;
} datetime_t;

#define ONE_DAY                  (1*60*60*24)
#define UTC_TIME_WEEKDAY_OFFSET (4)

void utc_timestamp_to_date(timestamp_t timestamp, datetime_t* datetime);

#endif

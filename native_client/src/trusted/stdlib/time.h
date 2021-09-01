#ifndef TIME_H
#define TIME_H

typedef uint32_t timestamp_t; //seconds

typedef struct {
    uint16_t    year;
    uint8_t     month;
    uint8_t     day;
} datedata_t;

typedef struct {
    uint8_t     hour;
    uint8_t     minute;
    uint8_t     second;
} timedata_t;

#define ONE_DAY                  (1*60*60*24)
#define UTC_TIME_WEEKDAY_OFFSET (4)

void utc_timestamp_to_date(timestamp_t timestamp, datedata_t* date, timedata_t* time);

#endif

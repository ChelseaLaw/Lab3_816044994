
#ifndef RTC_DRIVER_H
#define RTC_DRIVER_H

/*
 * Lab 3 - Question 2
 * RTC Driver Header
 * Arduino MKRZERO - SAMD21
 */

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Structure for storing the date and time. */
typedef struct
{
    uint16_t year;
    uint8_t month;
    uint8_t day;
    uint8_t hour;
    uint8_t minute;
    uint8_t second;

} rtc_datetime_t;

/* RTC function declarations. */

/* Initialise the RTC peripheral. */
bool rtc_init(void);

/* Start the RTC. */
bool rtc_start(void);

/* Stop the RTC. */
bool rtc_stop(void);

/* Check whether the RTC is running. */
bool rtc_is_running(void);

/* Read the current time and date. */
bool rtc_read_time(rtc_datetime_t *out_time);

/* Write a new time and date. */
bool rtc_write_time(const rtc_datetime_t *new_time);

#ifdef __cplusplus
}
#endif

#endif

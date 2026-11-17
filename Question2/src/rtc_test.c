
/*
 * Lab 3 - Question 2
 * RTC Test Program
 *
 * Name: Chelsea Law
 * ID: 816044994
 *
 * Tests RTC initialization, reading,
 * writing, starting and stopping.
 */

#include "rtc_driver.h"

#include <stdbool.h>
#include <stdint.h>

/* Printing functions supplied by main.cpp. */
extern void test_print(const char *message);

extern void test_print_datetime(
    const char *label,
    const rtc_datetime_t *dt
);

/* Initialise and start the RTC test. */
bool rtc_test_begin(void)
{
    const rtc_datetime_t initial =
    {
        2026u,
        11u,
        5u,
        12u,
        0u,
        0u
    };

    if (!rtc_init())
    {
        test_print("ERROR: RTC initialization failed");
        return false;
    }

    if (!rtc_write_time(&initial))
    {
        test_print("ERROR: RTC initial write failed");
        return false;
    }

    test_print_datetime(
        "Initial RTC write: ",
        &initial
    );

    if (!rtc_start() || !rtc_is_running())
    {
        test_print("ERROR: RTC start failed");
        return false;
    }

    test_print("RTC started - reading every second");

    return true;
}

/* Repeatedly read and test the RTC. */
void rtc_test_cycle(void)
{
    static uint32_t cycles = 0u;

    rtc_datetime_t now;

    if (!rtc_read_time(&now))
    {
        test_print("ERROR: RTC read failed");
        return;
    }

    test_print_datetime(
        "RTC read: ",
        &now
    );

    cycles++;

    /*
     * Every five cycles, stop the RTC,
     * rewrite the current time and restart.
     */
    if ((cycles % 5u) == 0u)
    {
        if (!rtc_stop() || rtc_is_running())
        {
            test_print("ERROR: RTC stop failed");
            return;
        }

        test_print("RTC stopped");

        if (!rtc_write_time(&now))
        {
            test_print("ERROR: RTC rewrite failed");
            return;
        }

        test_print_datetime(
            "RTC repeated write: ",
            &now
        );

        if (!rtc_start() || !rtc_is_running())
        {
            test_print("ERROR: RTC restart failed");
            return;
        }

        test_print("RTC restarted");
    }
}

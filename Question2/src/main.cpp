
/*
 * Lab 3 - Question 2
 * RTC Test Application
 *
 * Name: Chelsea Law
 * ID: 816044944
 *
 * Board: Arduino MKRZERO
 * Peripheral: Built-in RTC
 */

#include <Arduino.h>
#include <stdio.h>

#include "rtc_driver.h"

/* C test functions. */
extern "C" bool rtc_test_begin(void);

extern "C" void rtc_test_cycle(void);

/* Print messages to the Serial Monitor. */
extern "C" void test_print(const char *message)
{
    SerialUSB.println(message);
}

/* Display the current RTC date and time. */
extern "C" void test_print_datetime(
    const char *label,
    const rtc_datetime_t *dt)
{
    char text[40];

    if (dt == nullptr)
    {
        SerialUSB.println("[invalid date]");
        return;
    }

    snprintf(
        text,
        sizeof(text),
        "%04u-%02u-%02u %02u:%02u:%02u",
        (unsigned)dt->year,
        (unsigned)dt->month,
        (unsigned)dt->day,
        (unsigned)dt->hour,
        (unsigned)dt->minute,
        (unsigned)dt->second
    );

    SerialUSB.print(label);
    SerialUSB.println(text);
}

/* Track whether RTC initialization succeeded. */
static bool ready = false;

/* Arduino setup function. */
void setup(void)
{
    SerialUSB.begin(115200);

    unsigned long start = millis();

    while (!SerialUSB &&
           (millis() - start < 5000UL))
    {
    }

    SerialUSB.println();
    SerialUSB.println(
        "LAB 3 - QUESTION 2: BUILT-IN RTC"
    );

    SerialUSB.println(
        "SAMD21 MODE2 Clock/Calendar"
    );

    ready = rtc_test_begin();
}

/* Arduino main loop. */
void loop(void)
{
    if (ready)
    {
        rtc_test_cycle();
    }

    delay(1000);
}

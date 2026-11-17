
/*
 * Lab 3 - Question 2
 * RTC Peripheral Driver
 *
 * Name: Chelsea Law
 * ID: 816044944
 *
 * Platform: Arduino MKRZERO
 * Microcontroller: SAMD21G18A
 *
 * Peripheral: Built-in Real-Time Clock
 * Mode: Clock/Calendar (MODE2)
 */

#include "rtc_driver.h"
#include <sam.h>

static bool rtc_initialized = false;

/* Wait for Generic Clock synchronisation. */
static void wait_gclk_sync(void)
{
    while (GCLK->STATUS.bit.SYNCBUSY)
    {
    }
}

/* Wait for RTC register synchronisation. */
static void wait_rtc_sync(void)
{
    while (RTC->MODE2.STATUS.bit.SYNCBUSY)
    {
    }
}

/* Check whether the supplied date and time are valid. */
static bool valid_datetime(const rtc_datetime_t *t)
{
    static const uint8_t days_in_month[12] =
    {
        31, 28, 31, 30, 31, 30,
        31, 31, 30, 31, 30, 31
    };

    uint8_t max_day;

    if (t == 0 ||
        t->year < 2000u ||
        t->year > 2063u ||
        t->month < 1u ||
        t->month > 12u ||
        t->hour > 23u ||
        t->minute > 59u ||
        t->second > 59u)
    {
        return false;
    }

    max_day = days_in_month[t->month - 1u];

    /* Account for leap years. */
    if (t->month == 2u &&
        (t->year % 4u) == 0u)
    {
        max_day = 29u;
    }

    return (t->day >= 1u &&
            t->day <= max_day);
}

/* Function 1: Initialise the RTC. */
bool rtc_init(void)
{
    rtc_initialized = false;

    /* Enable the RTC peripheral bus clock. */
    PM->APBAMASK.reg |= PM_APBAMASK_RTC;

    /*
     * Configure Generic Clock 2.
     * Use the internal OSCULP32K clock.
     * Divide the clock by 32.
     */
    GCLK->GENDIV.reg =
        GCLK_GENDIV_ID(2) |
        GCLK_GENDIV_DIV(4);

    wait_gclk_sync();

    GCLK->GENCTRL.reg =
        GCLK_GENCTRL_ID(2) |
        GCLK_GENCTRL_SRC_OSCULP32K |
        GCLK_GENCTRL_DIVSEL |
        GCLK_GENCTRL_GENEN;

    wait_gclk_sync();

    /* Connect Generic Clock 2 to the RTC. */
    GCLK->CLKCTRL.reg =
        GCLK_CLKCTRL_ID(RTC_GCLK_ID) |
        GCLK_CLKCTRL_GEN_GCLK2 |
        GCLK_CLKCTRL_CLKEN;

    wait_gclk_sync();

    /* Disable the RTC before resetting it. */
    RTC->MODE2.CTRL.reg &=
        ~RTC_MODE2_CTRL_ENABLE;

    wait_rtc_sync();

    /* Reset the RTC peripheral. */
    RTC->MODE2.CTRL.reg =
        RTC_MODE2_CTRL_SWRST;

    while (RTC->MODE2.CTRL.bit.SWRST ||
           RTC->MODE2.STATUS.bit.SYNCBUSY)
    {
    }

    /*
     * Configure MODE2 Clock/Calendar operation.
     * Use 24-hour format.
     * Prescaler = 1024.
     */
    RTC->MODE2.CTRL.reg =
        RTC_MODE2_CTRL_MODE_CLOCK |
        RTC_MODE2_CTRL_PRESCALER_DIV1024;

    wait_rtc_sync();

    /* Disable unused RTC interrupts. */
    RTC->MODE2.INTENCLR.reg = 0xFFu;

    rtc_initialized = true;

    return true;
}

/* Function 2: Start the RTC. */
bool rtc_start(void)
{
    if (!rtc_initialized)
    {
        return false;
    }

    RTC->MODE2.CTRL.reg |=
        RTC_MODE2_CTRL_ENABLE;

    wait_rtc_sync();

    return rtc_is_running();
}

/* Function 3: Stop the RTC. */
bool rtc_stop(void)
{
    if (!rtc_initialized)
    {
        return false;
    }

    RTC->MODE2.CTRL.reg &=
        ~RTC_MODE2_CTRL_ENABLE;

    wait_rtc_sync();

    return !rtc_is_running();
}

/* Function 4: Check RTC running status. */
bool rtc_is_running(void)
{
    if (!rtc_initialized)
    {
        return false;
    }

    return ((RTC->MODE2.CTRL.reg &
             RTC_MODE2_CTRL_ENABLE) != 0u);
}

/* Function 5: Read the RTC date and time. */
bool rtc_read_time(rtc_datetime_t *out_time)
{
    RTC_MODE2_CLOCK_Type value;

    if (!rtc_initialized ||
        out_time == 0)
    {
        return false;
    }

    /* Request a synchronized RTC register read. */
    RTC->MODE2.READREQ.reg =
        RTC_READREQ_RREQ;

    wait_rtc_sync();

    /* Read the CLOCK register. */
    value.reg = RTC->MODE2.CLOCK.reg;

    out_time->year =
        (uint16_t)(2000u + value.bit.YEAR);

    out_time->month = value.bit.MONTH;
    out_time->day = value.bit.DAY;
    out_time->hour = value.bit.HOUR;
    out_time->minute = value.bit.MINUTE;
    out_time->second = value.bit.SECOND;

    return valid_datetime(out_time);
}

/* Function 6: Write the RTC date and time. */
bool rtc_write_time(const rtc_datetime_t *new_time)
{
    RTC_MODE2_CLOCK_Type value;

    if (!rtc_initialized ||
        !valid_datetime(new_time))
    {
        return false;
    }

    /* Prepare the new clock register value. */
    value.reg = 0u;

    value.bit.YEAR =
        (uint8_t)(new_time->year - 2000u);

    value.bit.MONTH = new_time->month;
    value.bit.DAY = new_time->day;
    value.bit.HOUR = new_time->hour;
    value.bit.MINUTE = new_time->minute;
    value.bit.SECOND = new_time->second;

    /* Write the new time to the RTC. */
    RTC->MODE2.CLOCK.reg = value.reg;

    wait_rtc_sync();

    return true;
}

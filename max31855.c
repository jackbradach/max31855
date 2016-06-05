/* Filename: max31855.c
 * Author: Jack Bradach <jack@bradach.net>
 * Github: http://github.com/JackBradach
 *
 * Utility functions to read from a Maxim 31855 Cold-Junction Compensated
 * Thermocouple-to-Digital Converter module.  It sends readings back to the
 * master via SPI.  Straightforward to use and it does all the compensation
 * math for you internally.  No initialization is necessary for this module,
 * although it does assume that you've initialized your SPI bus and that the
 * 'spi_slave_select' and 'spi_read_dword' functions work as advertised.
 *
 * I've provided functions to return the values in Celsisus, Kelvin, or
 * Fahrenheit.  If you want Rankine, I'll not stop you but I'll not help
 * either.
 */
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <avr/io.h>

#include "max31855.h"

// TODO - 2016/05/28 - Need a better SPI backend that plays nice with
// multiple slaves and the various different controllers used on AVRs.
#include "spi.h"

/* Returns the 32-bit value representing temperature data from the module. */
static uint32_t max31855_read(void)
{
    uint32_t raw_reading;

    spi_slave_select(SPI_SLAVE_MAX31855);
    raw_reading = spi_read_dword();
    spi_slave_deselect();
    return raw_reading;
}

/* Reads the MAX31855 and returns the internal and thermocouple
 * temperature values to the provided pointers.  If either pointer
 * is NULL, it'll skip that reading.
 *
 * Returns 0 on a successful reading.  If the fault bits are lit up,
 * they'll be returned to the caller.
 */
// TODO - 2016/05/30 - this would be a great place to use cexceptions.
max31855_status max31855_temp_cel(float *t_int, float *t_tc)
{
    uint32_t raw_reading;

    raw_reading = max31855_read();

    /* Oh noes! */
    if (raw_reading & (1UL << MAX31855_FAULT_BIT)) {
        return (raw_reading & MAX31855_FAULT);
    }

    /* Temperature reading from cold compensation junction. */
    if (NULL != t_int) {
        int16_t tmp_int;

        tmp_int = MAX31855_TEMP_INT(raw_reading);

        /* Extend sign bit if need be. */
        if (MAX31855_INT_SIGN_NEG & raw_reading) {
            tmp_int |= 0xF000;
        }

        *t_int = (float) (tmp_int * MAX31855_INT_SCALE);
    }

    /* Compensated temperature from thermocouple tip. */
    if (NULL != t_tc) {
        int16_t tmp_tc;

        tmp_tc = MAX31855_TEMP_TC(raw_reading);

        /* Extend sign bit if need be. */
        if (MAX31855_TC_SIGN_NEG & raw_reading) {
            tmp_tc |= 0xC000;
        }

        *t_tc = (float) (tmp_tc * MAX31855_TC_SCALE);
    }

    return 0;
}

/* Read the external and, optionally, internal temperatures from the MAX31855
 * and return them in Fahrenheit.  This is essentially a wrapper to the Celsius
 * reading call does the "9/5ths plus 32" conversion that we all learned in
 * grade school, at least you did if you grew up in a country that decided
 * that the metric system was the tool of the devil.  *sigh* Ah well.  My
 * car gets forty rods to the hogshead, and that's the way I likes it!
 */
max31855_status max31855_temp_fah(float *t_int, float *t_tc)
{
    uint8_t status;

    status = max31855_temp_cel(t_int, t_tc);
    if (status)
        return status;

    if(NULL != t_int)
        *t_int = *t_int * (9.0 / 5.0) + 32.0;

    if (NULL != t_tc)
        *t_tc = *t_tc * (9.0 / 5.0) + 32.0;

    return 0;
}

/* Read the external and, optionally, internal temperatures from the MAX31855
 * and return them in Kelvin.  Also a wrapper to the Celsius routine.  It
 * just adds 273.15 to the results.  But you knew that.
 */
max31855_status max31855_temp_kel(float *t_int, float *t_tc)
{
    max31855_status status;

    status = max31855_temp_cel(t_int, t_tc);
    if (status)
        return status;

    if(NULL != t_int)
        *t_int = *t_int + 273.15;

    if(NULL != t_tc)
        *t_tc = *t_tc + 273.15;

    return 0;
}

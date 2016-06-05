#ifndef MAX31855_H
#define MAX31855_H
/* Headers for the Maxim 31855 Cold-Junction Compensated Thermocouple-to-Digital
 * Converter.  It sends temperature readings back via SPI.  This code provides
 * functions that handle the reading and conversion, returning a
 * ready-to-use float from both the thermocouple tip and the internal
 * temperature sensor.
 */

/* Status return codes for the MAX31855 are implemented such
 * that any non-zero return code indicates a fault has been
 * detected.  Since multiple faults can be signaled simultaneously,
 * the bits set in the returned value must be compared with the bit
 * vectors of the faults.
 */
#define MAX31855_OK 0

/** Error bit vectors for the MAX31855.  The first three values
 * pull double-duty as the bit vectors used to indicate which
 * faults are occuring when both internal and external temperatures
 * are read.
 */
enum max31855_fault {
    /** Indicates an open circuit on the thermocouple */
    MAX31855_FAULT_OC = 0,

    /** Indicates that the thermocouple is shorted to ground. */
    MAX31855_FAULT_SCG,

    /** Indicates that the thermocouple is shorted to Vcc. */
    MAX31855_FAULT_SCV,

    /** Indicates that a fault has occured. */
    MAX31855_FAULT_BIT = 16,
};

#define MAX31855_FAULT \
    (_BV(MAX31855_FAULT_OC) | _BV(MAX31855_FAULT_SCG) | _BV(MAX31855_FAULT_SCV))

/* Constants for thermocouple readings */
#define MAX31855_TC_SCALE 0.25F
#define MAX31855_TC_SIGN_NEG (1UL << 31)
#define MAX31855_TC_MASK 0xFFFC0000
#define MAX31855_TC_SHIFT 18

/* Constants for internal sensor readings */
#define MAX31855_INT_SCALE 0.0625F
#define MAX31855_INT_SIGN_NEG  (1UL << 15)
#define MAX31855_INT_MASK   0xFFF0
#define MAX31855_INT_SHIFT  4

/* Does all the shifting and casting to produce a thermocouple or
 * internal reading from the raw 32-bit value provided by the module.
 */
#define MAX31855_TEMP_TC(raw_reading) \
    ((int16_t)((raw_reading & MAX31855_TC_MASK) >> MAX31855_TC_SHIFT))

#define MAX31855_TEMP_INT(raw_reading) \
    ((int16_t)((raw_reading & MAX31855_INT_MASK) >> MAX31855_INT_SHIFT))

/* Represents a return value from the therocouple.  Zero is
 * success.  Nonzero means that one or more faults have
 * occured.
 */
typedef uint8_t max31855_status;

/* Takes a reading from the temperature sensor and returns it
 * in either celsius, fahrenheit, or kelvin.  The internal and
 * thermocouple readings are passed back via pointers.  If
 * you only care about one reading and not the other, you
 * can simply pass in a NULL pointer for the one you aren't
 * interested in.
 */
max31855_status max31855_temp_cel(float *t_int, float *t_tc);
max31855_status max31855_temp_fah(float *t_int, float *t_tc);
max31855_status max31855_temp_kel(float *t_int, float *t_tc);

#endif // MAX31855_H

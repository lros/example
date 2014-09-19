//#include "odTransport.hpp"
#include <stdint.h>

namespace odt {  // Object Dictionary Transport

    // The following is the union of all the names defined in all the
    // versions of the Object Dictionary that we know about.
    // By the way, the order here is arbitrary.
    // I considered alphabetizing this list.
    // But it must start with OD_VERSION and end with NUM_OD_ENTRIES.
    enum OdName {
        OD_VERSION = 0,    // Must start with this
        OD_CAPS_WHEEL_BASE,
        OD_CAPS_WHEEL_RADIUS,
        OD_CAPS_ENCODER_COUNTS,
        OD_CAPS_GEAR_RATIO,
        OD_CAPS_PI_SAMPLING_INTERVAL,
        OD_CAPS_TELEMETRY_COUNTER_INTERVAL,
        OD_TIMER,
        OD_VGO_INFORMATION,
        OD_VGO_INVENTORY,
        OD_VEL_SET_LEFT,
        OD_VEL_LEFT,
        OD_VEL_LEFT_INTEGRAL,
        OD_VEL_LEFT_PWM_CMD,
        OD_VEL_SET_RIGHT,
        OD_VEL_RIGHT,
        OD_VEL_RIGHT_INTEGRAL,
        OD_VEL_RIGHT_PWM_CMD,
        OD_ACCEL_LIMIT,
        OD_DECEL_LIMIT,
        OD_VEL_KP,
        OD_VEL_KI,
        OD_VEL_DECAY,
        OD_VEL_MAX_INTEGRAL,
        OD_TELEMETRY_INTERVAL,
        OD_TELEMETRY_COUNT,
        OD_TELEMETRY_MAX,
        OD_ADC_VOLT_RANGE,
        OD_ADC_VREF_P,
        OD_ADC_VREF_M,
        OD_ADC_DIST_UP,
        OD_ADC_DIST_LEFT,
        OD_ADC_DIST_RIGHT,
        OD_ADC_DIST_BACK,
        OD_ADC_ACC_X,
        OD_ADC_ACC_Y,
        OD_ADC_ACC_Z,
        OD_ADC_GROUND,
        OD_ADC_PWR_IN,
        OD_ADC_BATT_VOLTS,
        OD_ADC_DOCKV,
        OD_ADC_TEMP2,
        OD_ADC_RMOTOR_CUR,
        OD_ADC_LMOTOR_CUR,
        OD_ADC_AMBIENT_LIGHT,
        OD_ADC_TEMP0,
        OD_ADC_IR_LIGHT,
        OD_ADC_BATT_CUR,
        OD_SPIC_GPIN,
        OD_SPIC_GPOUT,
        OD_SPIC_REVID,
        OD_MPIC_REVID,
        OD_PROXIMITY_STATE,
        OD_PROXIMITY_UP,
        OD_PROXIMITY_LEFT,
        OD_PROXIMITY_RIGHT,
        OD_PROXIMITY_BACK,
        OD_PROXIMITY_UP_COEF,
        OD_PROXIMITY_LEFT_COEF,
        OD_PROXIMITY_RIGHT_COEF,
        OD_PROXIMITY_BACK_COEF,
        OD_PROXIMITY_UP_THRE_MAX,
        OD_PROXIMITY_LEFT_THRE_MAX,
        OD_PROXIMITY_RIGHT_THRE_MAX,
        OD_PROXIMITY_BACK_THRE_MAX,
        OD_PROXIMITY_UP_THRE_MIN,
        OD_PROXIMITY_LEFT_THRE_MIN,
        OD_PROXIMITY_RIGHT_THRE_MIN,
        OD_PROXIMITY_BACK_THRE_MIN,
        OD_PROXIMITY_UP_OBSTACLE_TRIG,
        OD_PROXIMITY_LEFT_OBSTACLE_TRIG,
        OD_PROXIMITY_RIGHT_OBSTACLE_TRIG,
        OD_PROXIMITY_BACK_OBSTACLE_TRIG,
        OD_PROXIMITY_UP_CLIFF_TRIG,
        OD_PROXIMITY_LEFT_CLIFF_TRIG,
        OD_PROXIMITY_RIGHT_CLIFF_TRIG,
        OD_PROXIMITY_BACK_CLIFF_TRIG,
        OD_OBSTACLE_AVOIDANCE,
        OD_BATT_CHARGER_STATE,
        OD_BATT_FUEL_GAUGE,
        OD_CONTROL_MODE,
        OD_HEARTBEAT_TIMEOUT,
        OD_HEARTBEAT_TIMER,
        OD_ACC_ANGLE_X,
        OD_ACC_ANGLE_Y,
        OD_ACC_ANGLE_Z,
        OD_ACC_CLIFF_ANGLE_X,
        OD_ACC_CLIFF_ANGLE_Y,
        OD_PROXIMITY_LEFT_CLIFF_ANGLE_TRIG,
        OD_PROXIMITY_RIGHT_CLIFF_ANGLE_TRIG,
        OD_ENCODER_LEFT,
        OD_ENCODER_RIGHT,
        OD_DEBUG_1,
        OD_DEBUG_2,
        OD_BASE_REV,
        OD_SPIC_HW_REV,
        OD_SPIC_VGPIO_ECHO,
        OD_SPIC_HLD_TMR,
        OD_EEPROM_CTRL,
        OD_EPPROM_DBG,
        OD_EEPROM_READ,
        OD_EEPROM_WRITE,
        OD_CDOCK_STATE,
        OD_CDOCK_CTRL,
        OD_SPIC_INT_REF,
        OD_SPIC_PWR_STATE,
        OD_EEPROM_0,
        OD_EEPROM_1,
        OD_EEPROM_2,
        OD_EEPROM_3,
        OD_EEPROM_4,
        OD_EEPROM_5,
        OD_EEPROM_6,
        OD_EEPROM_7,
        OD_EEPROM_8,
        OD_EEPROM_9,
        OD_EEPROM_10,
        OD_EEPROM_11,
        OD_EEPROM_12,
        OD_EEPROM_13,
        OD_EEPROM_14,
        OD_EEPROM_15,
        OD_EEPROM_16,
        OD_EEPROM_17,
        OD_EEPROM_18,
        OD_EEPROM_19,
        OD_EEPROM_20,
        OD_EEPROM_21,
        OD_EEPROM_22,
        OD_EEPROM_23,
        OD_EEPROM_24,
        OD_EEPROM_25,
        OD_EEPROM_26,
        OD_EEPROM_27,
        OD_EEPROM_28,
        OD_EEPROM_29,
        OD_EEPROM_30,
        OD_EEPROM_31,
        OD_EEPROM_32,
        OD_EEPROM_33,
        OD_EEPROM_34,
        OD_EEPROM_35,
        OD_EEPROM_36,
        OD_EEPROM_37,
        OD_EEPROM_38,
        OD_EEPROM_39,
        OD_EEPROM_40,
        OD_EEPROM_41,
        OD_EEPROM_42,
        OD_EEPROM_43,
        OD_EEPROM_44,
        OD_EEPROM_45,
        OD_EEPROM_46,
        OD_EEPROM_47,
        OD_EEPROM_48,
        OD_EEPROM_49,
        OD_EEPROM_50,
        OD_EEPROM_51,
        OD_EEPROM_52,
        OD_EEPROM_53,
        OD_EEPROM_54,
        OD_EEPROM_55,
        OD_EEPROM_56,
        OD_EEPROM_57,
        OD_EEPROM_58,
        OD_EEPROM_59,
        OD_EEPROM_60,
        OD_EEPROM_61,
        OD_EEPROM_62,
        OD_EEPROM_63,
        OD_EEPROM_ERROR,
        OD_ACC_CAL_CTRL,
        OD_ENC_MUL,
        OD_BOOT_REVID,
        OD_HEARTBEAT_ERRCNT,
        OD_TEMP_01,
        OD_TEMP_02,
        OD_TEMP_03,
        OD_TEMP_04,
        OD_TEMP_05,
        // Following are not real OD entries; they do not belong here
        //OD_VERSION_VALUE,
        //OD_INVALID,
        // ... but these do:
        OD_MAX,  // this has to be treated like a real entry
        // Additions for v7:
        OD_TURBO_SETTING,
        OD_TURBO_ENGAGED,
        NUM_OD_ENTRIES   // Must end with this.
    };

}   // end namespace odt

uint16_t od_v6_table[odt::NUM_OD_ENTRIES];
uint8_t od_v6_size[odt::NUM_OD_ENTRIES];

uint16_t od_v7_table[odt::NUM_OD_ENTRIES];
uint8_t od_v7_size[odt::NUM_OD_ENTRIES];

void init() {
    unsigned i;
    for (i = 0; i < odt::NUM_OD_ENTRIES; i++) {
        od_v6_table[i] = 0xffff;
        od_v6_size[i] = 0xff;
        od_v7_table[i] = 0xffff;
        od_v7_size[i] = 0xff;
    }

    // Slurp in the Object Dictionary version 6:
    #define ENUM_VAL0(name, size) \
            i = 0; od_v6_table[odt::name] = i; od_v6_size[odt::name] = size
    #define ENUM_VAL(name, size) \
            i++; od_v6_table[odt::name] = i; od_v6_size[odt::name] = size
    #define ENUM_CONSTANT(name, value) 0
    #include "od_vals_v6.h"
    #undef ENUM_VAL0
    #undef ENUM_VAL
    #undef ENUM_CONSTANT
    ;  // The stuff above ends without a semicolon.

    // Slurp in the Object Dictionary version 7:
    #define ENUM_VAL0(name, size) \
            i = 0; od_v7_table[odt::name] = i; od_v7_size[odt::name] = size
    #define ENUM_VAL(name, size) \
            i++; od_v7_table[odt::name] = i; od_v7_size[odt::name] = size
    #define ENUM_CONSTANT(name, value) 0
    #include "od_vals_v7.h"
    #undef ENUM_VAL0
    #undef ENUM_VAL
    #undef ENUM_CONSTANT
    ;  // The stuff above ends without a semicolon.
}

int main() {
    init();
}


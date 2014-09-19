/**	@brief	Contains the definitions for the baseboard object dictionary.

	@file

	@ingroup	CommsLib

	<!-- Source License Info
		The included programs are the sole property of VGo Communications, Inc.,
		and contain its proprietary and confidential information.
		Copyright (c) 2007-2013 VGo Communications, Inc.
		All Rights Reserved.
	-->

	@par	License
			The included programs are the sole property of VGo Communications, Inc.,
			and contain its proprietary and confidential information.\n
			Copyright &copy; 2007-2013 VGo Communications, Inc.\n
			All Rights Reserved.

    $URL: http://svn/svn/public/branches/users/sclark/trunkprep/vendor/PulseInnovation/Include/od_vals.h $

    @author	Created by Nadim El-Fata, Pulse Innovation

    <!--
    @date	Created on 
    -->

    $Revision: 19851 $

    $LastChangedDate: 2013-02-13 21:49:42 -0500 (Wed, 13 Feb 2013) $

    $LastChangedBy: jmuller $

	<!-- @par requirements	-->

	<!-- 
		****
		NOTE: this file uses DoxyGen comment syntax for self-documenting code.
		DoxyGen is a JavaDoc-like tool that parses comments in source files to create code Documentation.
		see http://www.doxygen.org for details.
		This block is intentionally NOT decorated for Doxygen.
		****
	-->

 */

    //------------------------------------------------------------------------
    // READ ONLY VALUES
    //------------------------------------------------------------------------
    // NOTE:
    // - these values are updated periodically by PIC firmware or
    //   upon startup read only and should only
    // - they are considered read only and should never be modified by the host
    //
    // LEGEND
    // ------
    // RO: read only 
    // RW: read write
    //
    //------------------------------------------------------------------------
#define DEBUG_REV_BC
    // DESCRIPTION  : VGo Revision number
    // TYPE         : RO
    // RANGE        : 32-bit value
    // NOTE         : *** should be kept as the first entry ***
    ENUM_VAL0(OD_VERSION, 4),

	// DESCRIPTION  : Wheel base in meters
    // TYPE         : RO
    // RANGE        : 32-bit value, fixed point 32.16 format
    ENUM_VAL(OD_CAPS_WHEEL_BASE,4),
    // DESCRIPTION  : Wheel radius in meters
    // TYPE         : RO
    // RANGE        : 32-bit value, fixed point 32.16 format
	ENUM_VAL(OD_CAPS_WHEEL_RADIUS,4),
    // DESCRIPTION  : Number of encoder counts
    // TYPE         : RO
    // RANGE        : 32-bit value
	ENUM_VAL(OD_CAPS_ENCODER_COUNTS,4),
	// DESCRIPTION  : Gearing ration between motor shaft and wheels
	// TYPE         : RO
	// RANGE        : 32-bit value, fixed point 32.16
	ENUM_VAL(OD_CAPS_GEAR_RATIO,4),
    // DESCRIPTION  : PI sampling interval in milliseconds
    // TYPE         : RO
    // RANGE        : 32-bit value
	ENUM_VAL(OD_CAPS_PI_SAMPLING_INTERVAL,4),
    // DESCRIPTION  : Inteval between each telemetry timer decrement in milliseconds
    // TYPE         : RO
    // RANGE        : 32-bit value
	ENUM_VAL(OD_CAPS_TELEMETRY_COUNTER_INTERVAL,4),


    // DESCRIPTION  : Timer value, increments every ms and will wrap back to 0
    // TYPE         : RO
    // RANGE        : 32-bit value
    ENUM_VAL(OD_TIMER,4),

    // DESCRIPTION  : CPU load in percent
    // TYPE         : RO
    // RANGE        : 0..100
    //ENUM_VAL(OD_CPU_LOAD,4),
       ///  ** replaced ***

    //OD_CPU_LOAD    -> OD_VGO_INFORMATION
    //OD_VGO_INFORMATION[31:16] : = VGo Model Number
    //     Actually reports 1000 (0x03E8) but should prefix this with "V-" and be reported as "V-1000" to the User.
    //OD_VGO_INFORMATION[15:0] = VGo Hardware Revision
    //    The overall Version/revision of the VGo as assigned in MFG
    ENUM_VAL(OD_VGO_INFORMATION,4),
   
    // DESCRIPTION  : Maximum CPU load in percent
    // TYPE         : RO
    // RANGE        : 0..100
    //ENUM_VAL(OD_CPU_LOADMAX,4),
       ///  ** replaced ***

    //OD_CPU_LOADMAX -> OD_VGO_INVENTORY, Packed four 8-bit words into this 32-bit location:
    //OD_VGO_INVENTORY[31:24] = reserved  Currently reads 0

    //OD_VGO_INVENTORY[23:16] = VGo Motor Type, This will report the Motor Type
       //1= Base Line Motors as used in Beta and first production
       //2= Next Faster Motor

    //OD_VGO_INVENTORY[15:8] = VGo Cliff Sensor Type, This will report the 
       //1=Base Line Sensor Location/design. As done in Beta
       //2=Production Sensor location/design, as will be done in production units.
    //Currently only the Base firmware really needs to know. User to adjust the cliff thresholds.

    //OD_VGO_INVENTORY[7:0] = VGo Motor Encoder Type, This will report the Encoder type
       //1=Optical Encoders
       //2-31 = Hall Sensor (usually 25, set to lower value only to make VGo Faster)
       //32 = reserved for future
    ENUM_VAL(OD_VGO_INVENTORY,4),

    //
    // VELOCITY     : the following defines the velocity parameters for both left and right motors
    //                it is important to maintain the same parameters for both sides
    //

    // DESCRIPTION  : Set velocity for left motor, in encoder counts per measurement interval on dsPIC
    // TYPE         : RW
    // RANGE        : 16-bit value, fixed point 16.2 format
	ENUM_VAL(OD_VEL_SET_LEFT,2),
    // DESCRIPTION  : Actual velocity of the left motor, in encoder counts per measurement interval on dsPIC
    // TYPE         : RO
    // RANGE        : 16-bit value, fixed point 16.2 format
	ENUM_VAL(OD_VEL_LEFT,2),
    // DESCRIPTION  : Left motor PI controller integral value
    // TYPE         : RW
    // RANGE        : 32-bit value, fixed point 32.10 format
	ENUM_VAL(OD_VEL_LEFT_INTEGRAL,4),
    // DESCRIPTION  : Left motor PWM controller command
    // TYPE         : RO
    // RANGE        : 16-bit value
	ENUM_VAL(OD_VEL_LEFT_PWM_CMD,2),
    // DESCRIPTION  : Set velocity for right motor, in encoder counts per measurement interval on dsPIC
    // TYPE         : RW
    // RANGE        : 16-bit value, fixed point 16.2 format
	ENUM_VAL(OD_VEL_SET_RIGHT,2),
    // DESCRIPTION  : Actual velocity of the right motor, in encoder counts per measurement interval on dsPIC
    // TYPE         : RO
    // RANGE        : 16-bit value, fixed point 16.2 format
    ENUM_VAL(OD_VEL_RIGHT,2),
    // DESCRIPTION  : Right motor PI controller integral value
    // TYPE         : RW
    // RANGE        : 32-bit value, fixed point 32.10 format
	ENUM_VAL(OD_VEL_RIGHT_INTEGRAL,4),
    // DESCRIPTION  : Right motor PWM controller command
    // TYPE         : RO
    // RANGE        : 16-bit value
	ENUM_VAL(OD_VEL_RIGHT_PWM_CMD,2),

    // DESCRIPTION  : Set acceleration limit for left and right motors
    //                in encoder counts per measurement interval
    // TYPE         : RW
    // RANGE        : 16-bit value
	ENUM_VAL(OD_ACCEL_LIMIT,2),
    // DESCRIPTION  : Set deceleration limit for left and right motors
    //                in encoder counts per measurement interval
    // TYPE         : RW
    // RANGE        : 16-bit value
	ENUM_VAL(OD_DECEL_LIMIT,2),

    //
    // PID loop settings, for left & right motor controllers
    //
    // DESCRIPTION  : Proportional coefficient for PI loop
    // TYPE         : RW
    // RANGE        : 32-bit value, fixed point 32.8 format
    ENUM_VAL(OD_VEL_KP,4),
    // DESCRIPTION  : Integral coefficient for PI loop
    // TYPE         : RW
    // RANGE        : 32-bit value, fixed point 32.8 format
    ENUM_VAL(OD_VEL_KI,4),
    // DESCRIPTION  : Integral decay coefficient for PI loop, not implemented yet
    // TYPE         : RW
    // RANGE        : 32-bit value, fixed point 32.8 format
    ENUM_VAL(OD_VEL_DECAY,4),
    // DESCRIPTION  : Maximum value that integral portion of the PI controller allowed to reach
    // TYPE         : RW
    // RANGE        : 32-bit value, fixed point 32.10 format
	ENUM_VAL(OD_VEL_MAX_INTEGRAL,4),

    //
    // Telemetry settings
    //
    // DESCRIPTION  : Specifies how often to send telemetry to the host
	//				  value specified in number of dspic interrupt counts,
	//                currently interrupt runs at 1khz
	//                if set to zero then no telemetry will be sent
    // TYPE         : RW
    // RANGE        : 32-bit value
	ENUM_VAL(OD_TELEMETRY_INTERVAL, 4),
    // DESCRIPTION  : Current value of the interrupt counter, from the telemetry send routine
	//                once it reaches OD_TELEMETRY_INTERVAL telemetry packed will be formed and sent to the host
    // TYPE         : RO
    // RANGE        : 32-bit value
	ENUM_VAL(OD_TELEMETRY_COUNT, 4),
    // DESCRIPTION  : Maximum number of telemetry entries that could be added by the host
    //                it is a limit needed due to memory constraints
    // TYPE         : RO
    // RANGE        : 16-bit value
	ENUM_VAL(OD_TELEMETRY_MAX, 2),

    //
    // DESCRIPTION  : Current value of the MPIC ADC, updated periodcally at a rate of 1KHz
    // TYPE         : RO
    // RANGE        : 16-bit values
    // NOTE1        : all ADC values are 12-bit values from 0..4095 corresponding to 0.0 to 3.3 volts
    // NOTE2        : keep together
    //

    // DESCRIPTION  : ADC maximum voltage range
    // TYPE         : RO
    // RANGE        : 32-bit float, fixed point 32.16, value: 3.3
    ENUM_VAL(OD_ADC_VOLT_RANGE,4),

    // DESCRIPTION  : Positive ADC reference voltage
    // TYPE         : RO
    // RANGE        : 16-bit values: 0..4095
    // NOMINAL      : ~4095
#if defined(__dsPIC33FJ128MC804__)
    ENUM_VAL(OD_ADC_OFFSET,2),
    ENUM_VAL(OD_ADC_VREF_P,2) = OD_ADC_OFFSET,
#else
    ENUM_VAL(OD_ADC_VREF_P,2),
#endif
    // DESCRIPTION  : Negative ADC reference voltage
    // TYPE         : RO
    // RANGE        : 16-bit values: 0..4095
    // NOMINAL      : ~0
    ENUM_VAL(OD_ADC_VREF_M,2),
    // DESCRIPTION  : Up IR sensor
    // TYPE         : RO
    // RANGE        : 16-bit values: 0..4095
    ENUM_VAL(OD_ADC_DIST_UP,2),
    // DESCRIPTION  : Left IR sensor
    // TYPE         : RO
    // RANGE        : 16-bit values: 0..4095
    ENUM_VAL(OD_ADC_DIST_LEFT,2),
    // DESCRIPTION  : Right IR sensor
    // TYPE         : RO
    // RANGE        : 16-bit values: 0..4095
    ENUM_VAL(OD_ADC_DIST_RIGHT,2),
    // DESCRIPTION  : Back IR sensor
    // TYPE         : RO
    // RANGE        : 16-bit values: 0..4095
    // NOMINAL      : ~0 if no close object, ~4095 if close oject
    ENUM_VAL(OD_ADC_DIST_BACK,2),
    // DESCRIPTION  : X acceleration
    // TYPE         : RO
    // RANGE        : 16-bit values: 0..4095
    ENUM_VAL(OD_ADC_ACC_X,2),
    // DESCRIPTION  : Y acceleration
    // TYPE         : RO
    // RANGE        : 16-bit values: 0..4095
    ENUM_VAL(OD_ADC_ACC_Y,2),
    // DESCRIPTION  : Z acceleration
    // TYPE         : RO
    // RANGE        : 16-bit values: 0..4095
    ENUM_VAL(OD_ADC_ACC_Z,2),

    //
    // DESCRIPTION  : Current value of the SPIC DATA, updated periodically at a rate of 1Hz
    // TYPE         : RO
    // RANGE        : 16-bit values
    // NOTE         : keep together
    //
#if defined(__dsPIC33FJ128MC804__)
    ENUM_VAL(OD_ADC_LOCAL_MAX,2),
    ENUM_VAL(OD_ADC_GROUND,2) = OD_ADC_LOCAL_MAX,
#else
    ENUM_VAL(OD_ADC_GROUND,2),
#endif
    // DESCRIPTION  : Main power voltage
    // RANGE        : 16-bit value
    // UNIT         : mV
    // NOMINAL      : 10000-13000 (mv) (when on battery) or 14000-18000 (mv) (when on external/base power)
    ENUM_VAL(OD_ADC_PWR_IN,2),
    // DESCRIPTION  : Main power battery voltage
    // RANGE        : 16-bit value
    // UNIT         : mV
    ENUM_VAL(OD_ADC_BATT_VOLTS,2),
    // DESCRIPTION  : Dock Voltage (used to be OD_ADC_TEMP1)
    // RANGE        : 16-bit value
    // UNIT         : mV
    ENUM_VAL(OD_ADC_DOCKV,2),
    // DESCRIPTION  : Temperature measurement at location Z on Base Board
    // RANGE        : 16-bit value
    // UNIT         : degrees Celsius
    ENUM_VAL(OD_ADC_TEMP2,2),
    // DESCRIPTION  : Right motor peak current
    // RANGE        : 16-bit value
    // UNIT         : mA
    ENUM_VAL(OD_ADC_RMOTOR_CUR,2),
    // DESCRIPTION  : Left motor peak current
    // RANGE        : 16-bit value
    // UNIT         : mA
    ENUM_VAL(OD_ADC_LMOTOR_CUR,2),
    // DESCRIPTION  : Ambient light magnitude
    // RANGE        : 16-bit value
    // UNIT         : TBD
    ENUM_VAL(OD_ADC_AMBIENT_LIGHT,2),
    // DESCRIPTION  : Temperature measurement at location X on Base Board
    // RANGE        : 16-bit value
    // UNIT         : degrees Celsius
    ENUM_VAL(OD_ADC_TEMP0,2),
    // DESCRIPTION  : Infrared light magnitude
    // RANGE        : 16-bit value
    // UNIT         : TBD
    ENUM_VAL(OD_ADC_IR_LIGHT,2),
    // DESCRIPTION  : Battery charge current
    // RANGE        : 16-bit value
    // UNIT         : mA
    ENUM_VAL(OD_ADC_BATT_CUR,2),

    //
    // DESCRIPTION  : Used to provide the following digital inputs
    //                - BIT 0: old bumper right (0: not detected, 1: detected)
    //                - BIT 1: old bumper left (0: not detected, 1: detected)
    //                - BIT 2: old over current (0: normal, 1: over current)
    //                - BIT 3: old external peripheral (0: not detected, 1: detected)
    //                - BIT 4: old external power detected (0: not detected, 1: detected)
    //                - BIT 5: old dock power detected (0: not detected, 1: detected)
    //                - BIT 6: old battery detected (0: not detected, 1: detected)
    //                - BIT 7: reserved
    //                - BIT 8: new bumper right (0: not detected, 1: detected)
    //                - BIT 9: new bumper left (0: not detected, 1: detected)
    //                - BIT 10: new over current (0: normal, 1: over current)
    //                - BIT 11: new external peripheral (0: not detected, 1: detected)
    //                - BIT 12: new external power detected (0: not detected, 1: detected)
    //                - BIT 13: new dock power detected (0: not detected, 1: detected)
    //                - BIT 14: new battery detected (0: not detected, 1: detected)
    //                - BIT 15: reserved
    // TYPE         : RO
    // RANGE        : 16-bit value
    //
    ENUM_VAL(OD_SPIC_GPIN,2),

    //
    // DESCRIPTION  : Used to provide the following digital outputs
    //                - BIT 0: headlight control (0: off, 1: on)
    //                - BIT 1: dist/NS power control (0: off, 1: on)
	//                - BIT 2: extp control (0: off, 1: on)
	//                - BIT 3: Northstar control (0: off, 1: on)
	//                - BIT 8: Power off delay control (0: off, 1: on)
	//                - BIT 9: Power bchg control (0: off, 1: on)
	//                - BIT 10: Power bchg 50 pct control (0: off, 1: on)
	//                - BIT 11: Power bchg 25 pct control (0: off, 1: on)
	//				  - BIT 4-7,12-15: Reserved
    // TYPE         : RW
    // RANGE        : 16-bit value
    //
    ENUM_VAL(OD_SPIC_GPOUT,2),
    // DESCRIPTION  : SPIC firmware revision
    //                - BIT 0..7  : firmware ID (fixed to 0x04)
    //                - BIT 8..15 : firmware revision (changes with firmware releases)
    // TYPE         : RO
    // RANGE        : 16-bit
    ENUM_VAL(OD_SPIC_REVID,2),
    // DESCRIPTION  : MPIC firmware revision
    //                - BIT 0..7  : firmware ID (fixed to 0x01)
    //                - BIT 8..15 : firmware revision (changes with firmware releases)
    // TYPE         : RO
    // RANGE        : 16-bit
    ENUM_VAL(OD_MPIC_REVID,2),

    // DESCRIPTION  : Proximity detection status
    //                - BIT 0..3:   Left  0x0 (too close), 0x1 (invalid/saturated), 0x2 (floor),   0x3 (obstacle), 0x4 (cliff), 0xF (disabled)
    //                - BIT 4..7:   Right 0x0 (too close), 0x1 (invalid/saturated), 0x2 (floor),   0x3 (obstacle), 0x4 (cliff), 0xF (disabled)
    //                - BIT 8..11:  Up    0x0 (too close), 0x1 (invalid/saturated), 0x2 (ceiling), 0x3 (obstacle),              0xF (disabled)
    //                - BIT 12..15: Back  0x0 (too close), 0x1 (invalid/saturated), 0x2 (floor),   0x3 (obstacle), 0x4 (cliff), 0xF (disabled)
    // TYPE         : RO
    // RANGE        : 16-bit
    ENUM_VAL(OD_PROXIMITY_STATE,2),

	// DESCRIPTION  : Up/Left/Right/Back conditioned IR sensor (shows difference between threshold and instantaneous)
    //                to be used as a trigger for obstacle, cliff or ceiling along with OD_PROXIMITY_STATE
    // TYPE         : RO
    // RANGE        : 16-bit value, ADC counts
    // NOTE         :
    ENUM_VAL(OD_PROXIMITY_UP,2),
    ENUM_VAL(OD_PROXIMITY_LEFT,2),
    ENUM_VAL(OD_PROXIMITY_RIGHT,2),
    ENUM_VAL(OD_PROXIMITY_BACK,2),

	// DESCRIPTION  : Up/Left/Right/Back filter coeficients used to filter and compute the IR sensor threshold
    // TYPE         : RW
    // RANGE        : 32-bit value, fixed point 32.16 format
    // NOTE         : should be a value that can be represented as an exact binary number
    //                in sums of 2^n.2^-m otherwise the filter will oscillate
    ENUM_VAL(OD_PROXIMITY_UP_COEF,4),
    ENUM_VAL(OD_PROXIMITY_LEFT_COEF,4),
    ENUM_VAL(OD_PROXIMITY_RIGHT_COEF,4),
    ENUM_VAL(OD_PROXIMITY_BACK_COEF,4),

	// DESCRIPTION  : Up/Left/Right/Back threshold max limits, needed to keep threshold within certain range
    // TYPE         : RW
    // RANGE        : 16-bit value, ADC counts
    ENUM_VAL(OD_PROXIMITY_UP_THRE_MAX,2),
    ENUM_VAL(OD_PROXIMITY_LEFT_THRE_MAX,2),
    ENUM_VAL(OD_PROXIMITY_RIGHT_THRE_MAX,2),
    ENUM_VAL(OD_PROXIMITY_BACK_THRE_MAX,2),

	// DESCRIPTION  : Up/Left/Right/Back threshold min limits, needed to keep threshold within certain range
    // TYPE         : RW
    // RANGE        : 16-bit value, ADC counts
    ENUM_VAL(OD_PROXIMITY_UP_THRE_MIN,2),
    ENUM_VAL(OD_PROXIMITY_LEFT_THRE_MIN,2),
    ENUM_VAL(OD_PROXIMITY_RIGHT_THRE_MIN,2),
    ENUM_VAL(OD_PROXIMITY_BACK_THRE_MIN,2),

	// DESCRIPTION  : Up/Left/Right/Back thresholds used to trigger obstacle detection
    // TYPE         : RW
    // RANGE        : 16-bit value, ADC counts
    ENUM_VAL(OD_PROXIMITY_UP_OBSTACLE_TRIG,2),
    ENUM_VAL(OD_PROXIMITY_LEFT_OBSTACLE_TRIG,2),
    ENUM_VAL(OD_PROXIMITY_RIGHT_OBSTACLE_TRIG,2),
    ENUM_VAL(OD_PROXIMITY_BACK_OBSTACLE_TRIG,2),
	// DESCRIPTION  : Up/Left/Right/Back thresholds used to trigger cliff detection
    // TYPE         : RW
    // RANGE        : 16-bit value, ADC counts
    ENUM_VAL(OD_PROXIMITY_UP_CLIFF_TRIG,2),
    ENUM_VAL(OD_PROXIMITY_LEFT_CLIFF_TRIG,2),
    ENUM_VAL(OD_PROXIMITY_RIGHT_CLIFF_TRIG,2),
    ENUM_VAL(OD_PROXIMITY_BACK_CLIFF_TRIG,2),

    // DESCRIPTION  : Obstacle avoidance
    //                - Enable: enable obstacle avoidance by checking distance/proximity sensor
    //                  and limit commanded velocity based on obstacle proximity
    //                - Disable: disable obstacle avoidance by completely stopping processing, the obstacle
    //                  detection value will be set to disabled (0xF)
    //                - Override: disable obstacle avoidance but keep processing
    //                - BIT 0..3  : Left  0x0 (disable), 0x1 (enable), 0x2 (override), 0xF (invalid)
    //                - BIT 4..7  : Right 0x0 (disable), 0x1 (enable), 0x2 (override), 0xF (invalid)
    //                - BIT 8..11 : Up    0x0 (disable), 0x1 (enable), 0x2 (override), 0xF (invalid)
    //                - BIT 12..15: Back  0x0 (disable), 0x1 (enable), 0x2 (override), 0xF (invalid)
    // TYPE         : RW
    // RANGE        : 16-bit
    ENUM_VAL(OD_OBSTACLE_AVOIDANCE,2),

    // DESCRIPTION  : Battery charger state
    //                Byte 0 if 0x00, then charger is charging,
    //                if not then the following bits when set provide the possible reasons
    //                - BIT 0: battery not installed
    //                - BIT 1: right motor current detected
    //                - BIT 2: left motor current detected
    //                - BIT 3: headlights are loading the battery
    //                - BIT 4: low voltage from external power brick
    //                - BIT 5: no voltage from external power brick
    //                - BIT 6: TEMP0 reading too high
    //                - BIT 7: battery fully charged
    //                Byte 1 describes battery charger state when set, if 0x00 then it is OFF
    //                if < 0x10 then battery not charging, if > 0x10 battery is charging
    //                - 0x00: off
    //                - 0x01: start 1
    //                - 0x02: start 2
    //                - 0x03: start 3
    //                - 0x04: long T
    //                - 0x05: i am full
    //                - 0x1A: stage A, fast charging, current regulation
    //                - 0x1B: stage B, fast charging, voltage regulation
    //                - 0x1C: stage C, float charging, topping off
    // TYPE         : RO
    // RANGE        : 16-bit
    ENUM_VAL(OD_BATT_CHARGER_STATE,2),

    // DESCRIPTION  : Battery fuel gauge, percentage of battery life
    //                Byte 0: reserved reads as 0x00
    //                Byte 1: value from 0..100
    // TYPE         : RO
    // RANGE        : 16-bit
    ENUM_VAL(OD_BATT_FUEL_GAUGE,2),

    // DESCRIPTION  : Control mode
    //                - 0x0000: velocity control, normal operation
    //                - 0x0001: velocity control, ramp limiter disabled
    //                - 0x0002: velocity control disabled
    // TYPE         : RW
    // RANGE        : 16-bit
    ENUM_VAL(OD_CONTROL_MODE,2),

    // DESCRIPTION  : Timeout interval for velocity heartbeat command in milliseconds
	//              : if set to 0 then heartbeat will be disabled (use for debugging)
	//              : initialized to 0 on startup
    // TYPE         : RW
    // RANGE        : 32-bit
	ENUM_VAL(OD_HEARTBEAT_TIMEOUT, 4),
    // DESCRIPTION  : Current value of the heartbeat timer
    // TYPE         : RW
    // RANGE        : 32-bit
	ENUM_VAL(OD_HEARTBEAT_TIMER, 4),

	// DESCRIPTION  : Accelerometer angles
	// TYPE         : RO
	// RANGE        : 16-bit value, -90 to +90 degrees
	// NOTE         :
	ENUM_VAL(OD_ACC_ANGLE_X,2),
	ENUM_VAL(OD_ACC_ANGLE_Y,2),
	ENUM_VAL(OD_ACC_ANGLE_Z,2),
	// DESCRIPTION  : Accelerometer cliff angle threshold 
	// TYPE         : RO
	// RANGE        : 16-bit value, 0 to +90 degrees
	// NOTE         :
	ENUM_VAL(OD_ACC_CLIFF_ANGLE_X,2),
	ENUM_VAL(OD_ACC_CLIFF_ANGLE_Y,2),
	// DESCRIPTION  : Left/Right thresholds used to trigger cliff detection when at an angle
    // TYPE         : RW
    // RANGE        : 16-bit value, ADC counts
    ENUM_VAL(OD_PROXIMITY_LEFT_CLIFF_ANGLE_TRIG,2),
    ENUM_VAL(OD_PROXIMITY_RIGHT_CLIFF_ANGLE_TRIG,2),

	//
    // USED FOR DEBUGGING
    //

    ENUM_VAL(OD_ENCODER_LEFT,2),
    ENUM_VAL(OD_ENCODER_RIGHT,2),

    ENUM_VAL(OD_DEBUG_1,4),
    ENUM_VAL(OD_DEBUG_2,4),

#ifdef DEBUG_REV_BC
	// DESCRIPTION  : Baseboard Hardware Rev
    // TYPE         : RO
    // Values		: Rev B = 2
	//				  Rev C = 4
	// Note			: If rev < 3 treat as B. If rev >= 4 treat as C
	
	ENUM_VAL(OD_BASE_REV,2),

	// DESCRIPTION  : SPIC Hardware Rev
    // TYPE         : RO
    // Values		: First byte = FW_ID
	//				  Second byte = FW_REV
    // RANGE        : 16-bit value

	ENUM_VAL(OD_SPIC_HW_REV,2),

    // DESCRIPTION  : SPIC echo of the following digital outputs
    //                - BIT 0: headlight control (0: off, 1: on)
    //                - BIT 1: dist/NS power control (0: off, 1: on)
	//                - BIT 2: extp control (0: off, 1: on)
	//                - BIT 3: Northstar control (0: off, 1: on)
	//                - BIT 8: Power off delay control (0: off, 1: on)
	//                - BIT 9: Power bchg control (0: off, 1: on)
	//                - BIT 10: Power bchg 50 pct control (0: off, 1: on)
	//                - BIT 11: Power bchg 25 pct control (0: off, 1: on)
	//				  - BIT 4-7,11-15: Reserved
    // TYPE         : RO
    // RANGE        : 16-bit value

	ENUM_VAL(OD_SPIC_VGPIO_ECHO,2),

	// DESCRIPTION  : SPIC Power Button Hold Timer
    // TYPE         : RO
    // Values		: First byte = PBUT_HOLD_TIMER
	//				  Second byte = PBUT_HELD_TIMER
    // RANGE        : 16-bit value

	ENUM_VAL(OD_SPIC_HLD_TMR,2),

	// DESCRIPTION  : EEPROM CTRL
    // TYPE         : RW
    // Values         old - BIT 0: Read Byte (0: off, 1: on)
    //                old - BIT 1: Read Success (0: off, 1: on)
	//                old - BIT 2: Write Byte (0: off, 1: on)
	//                old - BIT 3: Write Success(0: off, 1: on)
	//				  old - BIT 4-15: Reserved
	//
	//				  New - used to initiate a write to eeprom, all values currently stored in OD_EEPROM_0-62
	//				  are written to eeprom and the checksum is recalculated (OD_EEPROM_63)
	// Usage		: set to 0xAA05 wait for value to clear
	//				: set to 0x505A wait for value to clear
	//				: set to 0xC5A0 wait for value to clear (takes 2-3 seconds to write all values)
	// Note			: Only designed to be used during initial board setup by python scripts
    // RANGE        : 16-bit value
	ENUM_VAL(OD_EEPROM_CTRL,2),

	// DESCRIPTION  : EEPROM DBG - NOT USED ANYMORE - LEFT IN FOR DEBUG USE
    // TYPE         : RW 
    // Values         - BIT 0: Read Byte (0: off, 1: on)
    //                - BIT 1: Read Success (0: off, 1: on)
	//                - BIT 2: Write Byte (0: off, 1: on)
	//                - BIT 3: Write Success(0: off, 1: on)
	//				  - BIT 4-15: Reserved
    // RANGE        : 16-bit value

	ENUM_VAL(OD_EPPROM_DBG,2),

	// DESCRIPTION  : EEPROM Read - NOT USED ANYMORE - LEFT IN FOR DEBUG USE
    // TYPE         : RW
	// Values		: First byte = Address
	//				  Second byte = Data
    // RANGE        : 16-bit value
	ENUM_VAL(OD_EEPROM_READ,2),

	// DESCRIPTION  : EEPROM Write - NOT USED ANYMORE - LEFT IN FOR DEBUG USE
    // TYPE         : RW
	// Values		: First byte = Address
	//				  Second byte = Data
    // RANGE        : 16-bit value
	ENUM_VAL(OD_EEPROM_WRITE,2),
	
	// DESCRIPTION  : CDOCK_STATE
    // TYPE         : RO
    // Values         - BIT 3:0:  CDOCK_LEFT_SENSOR_STATE  (useful for diag)
	//                - BIT 7:4:  CDOCK_RIGHT_SENSOR_STATE (useful for diag)
	//                - BIT 15:8  DD_STATE 
    // RANGE        : 16-bit value
	ENUM_VAL(OD_CDOCK_STATE,2),

    // DESCRIPTION  : CDOCK_CTRL
    // TYPE         : RW
    // Values         - BIT 3:0:  CDOCK_MUX_CONTROL (useful for diag)
    //                            0 = Let MPIC control it (normal operation).
    //                            1 = force to Front
    //                            2 = force to Side
    //                            3 = force to Rear
    //                          others values reserved
    //                - BIT 7:4:  BASE_DRIVE_MODE (BDM)
    //                            0 = BDM_NORMAL Mode, BASE never takes over Driving
    //                                   ARD disabled
    //                                   Docking aborted if in progress.
    //                            1 = BDM_D2D_C1,  BASE commanded to drive to the DOCK
    //                                   only if it 'Can See Dock'
    //                                   If command received and 'Can NOT See Dock', Error case.
    //                            2 = BDM_D2D_C2,  BASE commanded to drive to the DOCK
    //                                   go right to it if it 'Can See Dock'
    //                                   Hunt if it can't see it.
    //                            3 = BDM_D2D_C3,  BASE commanded to drive to the DOCK
    //                                   Alwasy start with a Hunt
    //                - BIT 8:  Auto-Re-Dock (ARD) Armed 
    //                            0 = ARD disabled, 1 for armed
    //                - BIT 9:  Follow-Me Armed 
    //                            0 = Follow-Me disabled, 1 for armed
    //                - BIT 14:10  reeserved 
    //                - BIT 15: DOCK_RESET 
    //                            1 = ask Dock tate machine to reset it self, 
    //                                The resetting action will clear this bit when reset happens. 
    // RANGE        : 16-bit value
    ENUM_VAL(OD_CDOCK_CTRL,2),
	
	// DESCRIPTION  : SPIC Internal voltage reference
    // TYPE         : RO
    // Values         Raw 16 bit value from SPIC, divide by 13.1 to convert to mV
    // RANGE        : 16-bit value
	ENUM_VAL(OD_SPIC_INT_REF,2),
	
	// DESCRIPTION  : SPIC_PWR_STATE
    // TYPE         : RO
	// VALUES		: See BaseBoard/pwr_spic/pwr_spic.h for constant values
	//				: "PST_STATE_OFF_INIT"
	//				: "PST_STATE_OFF_REST"
	//				: "PST_STATE_OFF_BATT"
	//				: "PST_STATE_W4_RESET"
	//				: "PST_STATE_ON__REST"
	//				: "PST_STATE_ON__BATT"
	//				: "PST_STATE_ON_PB_OF"
	//				: "PST_STATE_ON_LBATT"
	//				: "PST_STATE_ON_LV_OF"
    //				: "PST_STATE_ON_PC_OF"
    // RANGE        : 16-bit value
	ENUM_VAL(OD_SPIC_PWR_STATE,2),
	
	// DESCRIPTION  : OD_EEPROM_0-64 
    // TYPE         : RW
	// VALUES		: 128 Bytes of eeprom storage
	//				  Rev B boards will read all 0's
	//				  Rev C boards will read values currently stored in eeprom
	//				  Changing these values will not write back to eeprom unless the OD_EEPROM_CTRL entry is used
	//				  If the checksum fails, values will read all 0's, encoder_mul and acc_cal will use default values
	//
	//				OD_EEPROM_0-2   : Board_ID - ASCII characters 6 bytes
	//				OD_EEPROM_3     : Board_VER - ASCII characters 2 bytes
	//				OD_EEPROM_4     : Board_REV - ASCII characters 2 bytes
	//				OD_EEPROM_5-10  : Board_Serial - ASCII characters 12 bytes
	//				OD_EEPROM_6-31  : Reserved
	//				OD_EEPROM_32    : Encoder multiplier
	//				OD_EEPROM_33    : acc_sensor X calibration
	//				OD_EEPROM_34    : acc_sensor Y calibration
	//				OD_EEPROM_35    : acc_sensor Z calibration
	//				OD_EEPROM_36-62 : Reserved
	//				OD_EEPROM_63    : Checksum
	ENUM_VAL(OD_EEPROM_0,2),
	ENUM_VAL(OD_EEPROM_1,2),
	ENUM_VAL(OD_EEPROM_2,2),
	ENUM_VAL(OD_EEPROM_3,2),
	ENUM_VAL(OD_EEPROM_4,2),
	ENUM_VAL(OD_EEPROM_5,2),
	ENUM_VAL(OD_EEPROM_6,2),
	ENUM_VAL(OD_EEPROM_7,2),
	ENUM_VAL(OD_EEPROM_8,2),
	ENUM_VAL(OD_EEPROM_9,2),
	ENUM_VAL(OD_EEPROM_10,2),
	ENUM_VAL(OD_EEPROM_11,2),
	ENUM_VAL(OD_EEPROM_12,2),
	ENUM_VAL(OD_EEPROM_13,2),
	ENUM_VAL(OD_EEPROM_14,2),
	ENUM_VAL(OD_EEPROM_15,2),
	ENUM_VAL(OD_EEPROM_16,2),
	ENUM_VAL(OD_EEPROM_17,2),
	ENUM_VAL(OD_EEPROM_18,2),
	ENUM_VAL(OD_EEPROM_19,2),
	ENUM_VAL(OD_EEPROM_20,2),
	ENUM_VAL(OD_EEPROM_21,2),
	ENUM_VAL(OD_EEPROM_22,2),
	ENUM_VAL(OD_EEPROM_23,2),
	ENUM_VAL(OD_EEPROM_24,2),
	ENUM_VAL(OD_EEPROM_25,2),
	ENUM_VAL(OD_EEPROM_26,2),
	ENUM_VAL(OD_EEPROM_27,2),
	ENUM_VAL(OD_EEPROM_28,2),
	ENUM_VAL(OD_EEPROM_29,2),
	ENUM_VAL(OD_EEPROM_30,2),
	ENUM_VAL(OD_EEPROM_31,2),
	ENUM_VAL(OD_EEPROM_32,2),
	ENUM_VAL(OD_EEPROM_33,2),
	ENUM_VAL(OD_EEPROM_34,2),
	ENUM_VAL(OD_EEPROM_35,2),
	ENUM_VAL(OD_EEPROM_36,2),
	ENUM_VAL(OD_EEPROM_37,2),
	ENUM_VAL(OD_EEPROM_38,2),
	ENUM_VAL(OD_EEPROM_39,2),
	ENUM_VAL(OD_EEPROM_40,2),
	ENUM_VAL(OD_EEPROM_41,2),
	ENUM_VAL(OD_EEPROM_42,2),
	ENUM_VAL(OD_EEPROM_43,2),
	ENUM_VAL(OD_EEPROM_44,2),
	ENUM_VAL(OD_EEPROM_45,2),
	ENUM_VAL(OD_EEPROM_46,2),
	ENUM_VAL(OD_EEPROM_47,2),
	ENUM_VAL(OD_EEPROM_48,2),
	ENUM_VAL(OD_EEPROM_49,2),
	ENUM_VAL(OD_EEPROM_50,2),
	ENUM_VAL(OD_EEPROM_51,2),
	ENUM_VAL(OD_EEPROM_52,2),
	ENUM_VAL(OD_EEPROM_53,2),
	ENUM_VAL(OD_EEPROM_54,2),
	ENUM_VAL(OD_EEPROM_55,2),
	ENUM_VAL(OD_EEPROM_56,2),
	ENUM_VAL(OD_EEPROM_57,2),
	ENUM_VAL(OD_EEPROM_58,2),
	ENUM_VAL(OD_EEPROM_59,2),
	ENUM_VAL(OD_EEPROM_60,2),
	ENUM_VAL(OD_EEPROM_61,2),
	ENUM_VAL(OD_EEPROM_62,2),
	ENUM_VAL(OD_EEPROM_63,2),

	// DESCRIPTION  : EEPROM_ERROR
    // TYPE         : RO
    // Values  
	//				  0=PASS >0 = FAIL
    // RANGE        : 16-bit value	
	ENUM_VAL(OD_EEPROM_ERROR,2),

	// DESCRIPTION  : OD_ACC_CAL_CTRL
    // TYPE         : RW
    // Usage		: Set to 1 to initiate an accelerometer calibration, will clear when complete
	//				: On rev C boards OD_EEPROM_33-35 will be updated and new values calibration values will be used
	//				: New values will not be written to EEPROM unless OD_EEPROM_CTRL is used	  
    // RANGE        : 16-bit value
	ENUM_VAL(OD_ACC_CAL_CTRL,2),

	// DESCRIPTION  : OD_ENC_MUL
    // TYPE         : RW
    // Usage		: Rev C - Read from EEPROM if a valid checksum is found, otherwise a default value is used
	//				: Modifying this value will take effect immediatly. To make the change permenant, write to OD_EEPROM_32 then use OD_EEPROM_CTRL
	//				: Rev B - Uses default value, different projects exist to set default for Hall and optical sensors	  
    // RANGE        : 16-bit value
	ENUM_VAL(OD_ENC_MUL,2),

	ENUM_VAL(OD_BOOT_REVID,2), // was ENUM_VAL(OD_AUTO_DRIVE_MODE,2), we never used that...

	ENUM_VAL(OD_HEARTBEAT_ERRCNT,2),
	//Reserved for future use
	ENUM_VAL(OD_TEMP_01,2),
	ENUM_VAL(OD_TEMP_02,2),
	ENUM_VAL(OD_TEMP_03,2),
	ENUM_VAL(OD_TEMP_04,2),
	ENUM_VAL(OD_TEMP_05,2),

#endif
    // ------------------------------------------------------------------------
    //
    // DESCRIPTION  : Used to provide total number of OD entries
    // TYPE         : RO
    // RANGE        : 32-bit value
    // NOTE         : Keep last ENUM_VAL() entry in the list
    //
    ENUM_VAL(OD_MAX,4),

    // ************************************************************************
    // put constant values after this

    // ------------------------------------------------------------------------
    //
    // DESCRIPTION  : Used to keep track of the version of the object dictionary
    // TYPE         : RO
    // RANGE        : 32-bit value
    // NOTE         : Increment this value any time the object dictionary is changed in an incompatible way
    //                  (which is probably with _any_ change, other than comments)
    ENUM_CONSTANT(OD_VERSION_VALUE, 6),

    ENUM_CONSTANT(OD_INVALID, -1)

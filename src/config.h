#include <kissStepper.h>

#ifndef UNO_TEST
    // -----------------------------------------------------------------------------
    // Schrittmotoren
    // -----------------------------------------------------------------------------
    #define J1_STEP_PIN             22
    #define J1_DIR_PIN              23
    #define J2_STEP_PIN             26
    #define J2_DIR_PIN              27
    #define J3_STEP_PIN             30
    #define J3_DIR_PIN              31
    #define J4_STEP_PIN             34
    #define J4_DIR_PIN              35
    #define J5_STEP_PIN             38
    #define J5_DIR_PIN              39
    #define J6_STEP_PIN             42
    #define J6_DIR_PIN              43
#endif

const driveMode_t J1_MODE =         FULL_STEP;
const driveMode_t J2_MODE =         HALF_STEP;
const driveMode_t J3_MODE =         HALF_STEP;
const driveMode_t J4_MODE =         HALF_STEP;
const driveMode_t J5_MODE =         MICROSTEP_4;
const driveMode_t J6_MODE =         HALF_STEP;

#ifndef UNO_TEST
    // -----------------------------------------------------------------------------
    // Endschalter & ESTOP
    // -----------------------------------------------------------------------------
    #define J1_LIMIT_SWITCH_PIN     14
    #define J2_LIMIT_SWITCH_PIN     15
    #define J3_LIMIT_SWITCH_PIN     16
    #define J4_LIMIT_SWITCH_PIN     17
    #define J5_LIMIT_SWITCH_PIN     18
    #define J6_LIMIT_SWITCH_PIN     19

    #define EMERGENCY_STOP_PIN      7
    // -----------------------------------------------------------------------------
    // Servo
    // -----------------------------------------------------------------------------
    #define SERVO1_PIN              11
    #define SERVO2_PIN              12
    #define SERVO3_PIN              13

#endif // UNO_TEST

// -----------------------------------------------------------------------------
// Limits
// -----------------------------------------------------------------------------
#define J1_POS_STEP_LIMIT           2000000L
#define J1_NEG_STEP_LIMIT           -2000000L
#define J2_POS_STEP_LIMIT           2000000L
#define J2_NEG_STEP_LIMIT           -2000000L
#define J3_POS_STEP_LIMIT           2000000L
#define J3_NEG_STEP_LIMIT           -2000000L
#define J4_POS_STEP_LIMIT           2000000L
#define J4_NEG_STEP_LIMIT           -2000000L
#define J5_POS_STEP_LIMIT           2000000L
#define J5_NEG_STEP_LIMIT           -2000000L
#define J6_POS_STEP_LIMIT           2000000L
#define J6_NEG_STEP_LIMIT           -2000000L


// For testing
#ifdef UNO_TEST
        // -----------------------------------------------------------------------------
    // Schrittmotoren
    // -----------------------------------------------------------------------------
    #define J1_STEP_PIN             2
    #define J1_DIR_PIN              1
    #define J2_STEP_PIN             3
    #define J2_DIR_PIN              4
    #define J3_STEP_PIN             5
    #define J3_DIR_PIN              6
    #define J4_STEP_PIN             7
    #define J4_DIR_PIN              8
    #define J5_STEP_PIN             9
    #define J5_DIR_PIN              10
    #define J6_STEP_PIN             11
    #define J6_DIR_PIN              12
    // -----------------------------------------------------------------------------
    // Endschalter & ESTOP
    // -----------------------------------------------------------------------------
    #define J1_LIMIT_SWITCH_PIN     13
    #define J2_LIMIT_SWITCH_PIN     14
    #define J3_LIMIT_SWITCH_PIN     15
    #define J4_LIMIT_SWITCH_PIN     16
    #define J5_LIMIT_SWITCH_PIN     17
    #define J6_LIMIT_SWITCH_PIN     18

    #define EMERGENCY_STOP_PIN      19
#endif
//TODO
//Referenzstatus senden

#include "global.h"

// -----------------------------------------------------------------------------
// Setup
// -----------------------------------------------------------------------------
void setup(void) {
    // Kommunikation
    comSetup();
    // RoboCtrl
    roboCtrlSetup();
    // Init
    sendRES();
}

// -----------------------------------------------------------------------------
// Loop
// -----------------------------------------------------------------------------
void loop(void) {
    // Kommunikation
    comLoop();
    // RoboCtrl
    roboLoop();
}
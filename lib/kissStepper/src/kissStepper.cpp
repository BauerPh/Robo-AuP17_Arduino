#include "kissStepper.h"
#ifndef ARDUINO_ARCH_ESP32
    #include <Flash.h>
#endif
// ----------------------------------------------------------------------------------------------------
// Lookup-Table
// ----------------------------------------------------------------------------------------------------
#include "kissStepperLookUpTable.h"
//#include "kissStepperLookUpTableSmall.h"

// ----------------------------------------------------------------------------------------------------
// Initialize the motor in a default state
// ----------------------------------------------------------------------------------------------------
void kissStepper::begin(uint16_t maxStepsPerSec, uint16_t accelStepsPerSecPerSec) {
    // set pins to output
    if (pinEnable != pinNotSet) pinMode(pinEnable, OUTPUT);
    pinMode(pinDir, OUTPUT);
    pinMode(pinStep, OUTPUT);

    // initial position index and limits
    pos = 0;
    forwardLimit = defaultForwardLimit;
    reverseLimit = defaultReverseLimit;
    
    // initial pin states
    disable();
    stop();
    digitalWrite(pinDir, PINVAL_FORWARD);
    
#ifdef STEP_LOW
	digitalWrite(pinStep, HIGH);
#else
	digitalWrite(pinStep, LOW);
#endif

    // defaults
    setMaxSpeed(maxStepsPerSec);
    setAccel(accelStepsPerSecPerSec);
	setCurSpeed(0);
    correctionCounter = 0;

    // this allows us to convert from a standard Arduino pin number to an AVR port
    // for faster digital writes in the work() method at the cost of a few bytes of memory
    // we don't use this technique for other digitalWrites because they are infrequent
    stepBit = digitalPinToBitMask(pinStep);
    stepOut = portOutputRegister(digitalPinToPort(pinStep));
}

void kissStepper::setDefaultLimits(void) {
    forwardLimit = defaultForwardLimit;
    reverseLimit = defaultReverseLimit;
}

// ----------------------------------------------------------------------------------------------------
// ----------------------------------------------------------------------------------------------------
void kissStepper::enable(void) {
    if (pinEnable != pinNotSet) digitalWrite(pinEnable, PINVAL_ENABLED);
    enabled = true;
}

// ----------------------------------------------------------------------------------------------------
// ----------------------------------------------------------------------------------------------------
void kissStepper::disable(void) {
    if (pinEnable != pinNotSet) {
        delay(50); // this short delay stops motor momentum
        digitalWrite(pinEnable, PINVAL_DISABLED);
    }
    target = pos;
    enabled = false;
}

// ----------------------------------------------------------------------------------------------------
// ----------------------------------------------------------------------------------------------------
void kissStepper::setMinSpeed(uint16_t stepsPerSec) {
    minSpeed = stepsPerSec;
}

// ----------------------------------------------------------------------------------------------------
// ----------------------------------------------------------------------------------------------------
void kissStepper::setMaxSpeed(uint16_t stepsPerSec) {
	if ((uint32_t(stepsPerSec) << driveMode) > maxSpeedUSteps) maxSpeed = (maxSpeedUSteps >> driveMode);
    else maxSpeed = stepsPerSec;
    // if the motor is moving and acceleration is off, change speed immediately
    if ((!accel) && (moveState != STOPPED)) setCurSpeed(stepsPerSec);
}

// ----------------------------------------------------------------------------------------------------
// The errorCorrection variable is used in work() to correct the rounding error that occurs
// in the calculation of stepInterval. The errorCorrection variable indicates the number of times
// out of 255 that a timing correction needs to be applied. Its value is compared to a counter,
// correctionCounter, in order to determine whether or not to apply a correction.
// In effect, this adds 8 bits worth of "decimal places" to stepInterval.
// ----------------------------------------------------------------------------------------------------
void kissStepper::setCurSpeed(uint16_t stepsPerSec) {
    if (stepsPerSec > 0) {
		uint32_t uStepsPerSec = uint32_t(stepsPerSec) << driveMode;
		stepInterval = getStepInterval(uStepsPerSec);
		//stepInterval = halfSecond / uStepsPerSec;
		errorCorrection = accLookupErr[uStepsPerSec - 1];
        //errorCorrection = ((halfSecond % uStepsPerSec) << 8) / uStepsPerSec;
        if (curSpeed == 0) lastStepTime = micros() - stepInterval;
    }
    else stepInterval = maxTimeInterval;
    curSpeed = stepsPerSec;
}

// ----------------------------------------------------------------------------------------------------
// This method sets the motor acceleration in RPM/s
// ----------------------------------------------------------------------------------------------------
void kissStepper::setAccel(uint16_t stepsPerSecPerSec) {
    // calculate the time interval at which to increment curSpeed
    // and recalculate decelDistance
    if (stepsPerSecPerSec > 0)
        accelInterval = (oneSecond + (stepsPerSecPerSec >> 1)) / stepsPerSecPerSec;
    accel = stepsPerSecPerSec;
    calcDecel();
}

// ----------------------------------------------------------------------------------------------------
// This method figures out the distance required to decelerate from the current speed
// ----------------------------------------------------------------------------------------------------
void kissStepper::calcDecel(void) {
    if (accel != 0) {        
        if (curSpeed <= minSpeed) 
            decelDistance = 0;
        else    
            // s = (v^2 - v0^2) / 2a
            decelDistance = (((uint32_t)curSpeed * curSpeed - (uint32_t)minSpeed * minSpeed) * fullStepVal) / ((uint32_t)accel << 1);
            //decelDistance = ((uint32_t)curSpeed * curSpeed * fullStepVal) / ((uint32_t)accel << 1);
    } else
        decelDistance = 0;
}

// ----------------------------------------------------------------------------------------------------
// Makes the motor work. Call repeatedly and often for smooth motion.
// Returns true if the motor is moving, otherwise false.
// ----------------------------------------------------------------------------------------------------
bool kissStepper::work(void) {
	// check if it's necessary to move the motor
    if (moveState != STOPPED)
    {
        uint32_t curTime = micros();

        // Handle acceleration
        if (accel) {
            accelState_t newAccelState;
            int32_t distRemaining = ((moveState == FORWARD) ? (target - pos) : (pos - target));
            // Beim bremsen 2 addieren, damit nicht laufend zwischen Beschleunigen und Bremsen gewechselt wird (durch Berechnungsungenauigkeiten)
            if (distRemaining > (decelDistance + ((accelState == DECELERATING) ? 2 : 0)))
                newAccelState = ((curSpeed == maxSpeed) ? CONSTVEL : ((curSpeed < maxSpeed) ? ACCELERATING : DECELERATING));
            else
                newAccelState = (curSpeed > minSpeed) ? DECELERATING : CONSTVEL;

            if (accelState != newAccelState) {
                Serial.print("State: ");
                Serial.println(newAccelState);
            }
            if (newAccelState != CONSTVEL) {
                if (accelState != newAccelState) lastAccelTime = curTime;
                if ((curTime - lastAccelTime) >= accelInterval) {
                    lastAccelTime += accelInterval;
                    setCurSpeed(curSpeed + newAccelState);
                    calcDecel();
                }
            }
			accelState = newAccelState;
        }

        // Step, if it's time...
        if ((curTime - lastStepTime) >= stepInterval) {
            if (!(*stepOut & stepBit)) {
                // check if the target is reached
                int32_t newPos = (moveState == FORWARD) ? (pos + 1) : (pos - 1);
                if (((moveState == FORWARD) && (newPos <= target)) || ((moveState == BACKWARD) && (newPos >= target))) {
                    // the target is not yet reached, so advance the position index
                    pos = newPos;
                } else {
                    // the target is reached
                    stop();
                    return false;
                }
            }

            // toggle the step pin
			*stepOut ^= stepBit;

            // increment lastStepTime
            lastStepTime += stepInterval;

            // this adds a correction to the timing
            if (correctionCounter < errorCorrection) lastStepTime++;
            correctionCounter += counterIncrement;
        }
        return true;
    }
    return false;
}

// ----------------------------------------------------------------------------------------------------
// This method starts the motor moving towards a target
// ----------------------------------------------------------------------------------------------------
bool kissStepper::moveTo(int32_t newTarget) {
    if (moveState == STOPPED) {
        // enable the motor controller if necessary
        if (!enabled) enable();

        // constrain the target between reverseLimit and forwardLimit
        target = constrain(newTarget, reverseLimit, forwardLimit);

        // set moveState
        moveState = (target > pos) ? FORWARD : ((target < pos) ? BACKWARD : STOPPED);

        if (moveState != STOPPED) {
            // set the DIR pin
            digitalWrite(pinDir, ((moveState == FORWARD) ? PINVAL_FORWARD : PINVAL_BACKWARD));

            // if not accelerating, start motor at full speed
            if (!accel) setCurSpeed(maxSpeed);
            else setCurSpeed(minSpeed);

			// restart correction counter
			correctionCounter = 0;
			
            return true;
        }
    }
    return false;
}

// ----------------------------------------------------------------------------------------------------
// ----------------------------------------------------------------------------------------------------
void kissStepper::decelerate(void) {
    target = ((moveState == FORWARD) ? (pos + decelDistance) : (pos - decelDistance));
    target = constrain(target, reverseLimit, forwardLimit);
}

// ----------------------------------------------------------------------------------------------------
// ----------------------------------------------------------------------------------------------------
void kissStepper::stop(void) {
    target = pos;
    curSpeed = 0;
    stepInterval = maxTimeInterval;
    accelState = CONSTVEL;
    decelDistance = 0;
    moveState = STOPPED;
}

// ----------------------------------------------------------------------------------------------------
// ----------------------------------------------------------------------------------------------------
void kissStepper::setPos(int32_t newPos) {
    if (moveState == STOPPED) {
        pos = constrain(newPos, reverseLimit, forwardLimit);
        target = pos;
    }
}

// ----------------------------------------------------------------------------------------------------
// ----------------------------------------------------------------------------------------------------
uint32_t kissStepper::getStepInterval(uint32_t uStepsPerSec) {
	if (uStepsPerSec < 8) {
		return accLookup32Bit[uStepsPerSec - 1];
	} else if (uStepsPerSec < 1954) {
		return accLookup16Bit[uStepsPerSec - 8];
	} else {
		return accLookup8Bit[uStepsPerSec - 1954];
	}
}
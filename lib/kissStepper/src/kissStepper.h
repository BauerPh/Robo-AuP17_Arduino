#ifndef kissStepper_H
#define kissStepper_H

#include <Arduino.h>

enum driveMode_t: uint8_t
{
    FULL_STEP = 0,
    HALF_STEP = 1,
    MICROSTEP_4 = 2,
    MICROSTEP_8 = 3,
    MICROSTEP_16 = 4,
    MICROSTEP_32 = 5,
    MICROSTEP_64 = 6,
    MICROSTEP_128 = 7
};

enum accelState_t: int8_t
{
    DECELERATING = -1,
    CONSTVEL = 0,
    ACCELERATING = 1
};

enum moveState_t: int8_t
{
    BACKWARD = -1,
    STOPPED = 0,
    FORWARD = 1
};

class kissStepper
{

public:
	kissStepper(driveMode_t driveMode, uint8_t pinDir, uint8_t pinStep, uint8_t pinEnable = 255) :
		driveMode(driveMode),
		pinDir(pinDir),
		pinStep(pinStep),
		pinEnable(pinEnable),
		fullStepVal(1 << driveMode) {}

	void begin(uint16_t maxStepsPerSec = 100, uint16_t accelStepsPerSecPerSec = 0);
	void setDefaultLimits(void);
	void enable(void);
	void disable(void);
	void setMinSpeed(uint16_t stepsPerSec);
	void setMaxSpeed(uint16_t stepsPerSec);
	uint16_t getMaxSpeed(void)
	{
		return maxSpeed;
	}
	uint16_t getCurSpeed(void)
	{
		return curSpeed;
	}
	bool work(void);
	bool moveTo(int32_t newTarget);
	void decelerate(void);
	void stop(void);
	void setPos(int32_t newPos);
	int32_t getPos(void)
	{
		return pos;
	}
	int32_t getTarget(void)
	{
		return target;
	}
	bool isEnabled(void)
	{
		return enabled;
	}
	void setAccel(uint16_t stepsPerSecPerSec);
	uint16_t getAccel(void)
	{
		return accel;
	}
	accelState_t getAccelState(void)
	{
		return accelState;
	}
	moveState_t getMoveState(void)
	{
		return moveState;
	}
	int32_t forwardLimit;
	int32_t reverseLimit;
	const uint8_t fullStepVal;

private:
	static const uint8_t PINVAL_FORWARD = LOW;
	static const uint8_t PINVAL_BACKWARD = HIGH;
	static const uint8_t PINVAL_ENABLED = LOW;
	static const uint8_t PINVAL_DISABLED = HIGH;
	static const uint32_t halfSecond = 500000UL;
	static const uint32_t oneSecond = 1000000UL;
	static const uint8_t counterIncrement = 17U;
	static const uint8_t pinNotSet = 255U;
	static const int32_t defaultForwardLimit = 1073741823L; // max int32 / 2
	static const int32_t defaultReverseLimit = -1073741824L; // max int32 / 2
	static const uint32_t maxTimeInterval = 4294967295UL;
	const driveMode_t driveMode;
    const uint8_t pinDir;
    const uint8_t pinStep;
    const uint8_t pinEnable;
    volatile uint8_t stepBit;
	#ifndef ARDUINO_ARCH_ESP32
    volatile uint8_t *stepOut;
	#else
	volatile uint32_t *stepOut;
	#endif
    int32_t pos;
    int32_t target;
	uint16_t minSpeed;
    uint16_t maxSpeed;
    uint16_t curSpeed;
    uint8_t errorCorrection;
    uint8_t correctionCounter;
    uint32_t stepInterval;
    uint32_t accelInterval;
    bool enabled;
    accelState_t accelState;
    moveState_t moveState;
    int32_t decelDistance;
    uint32_t lastAccelTime;
    uint32_t lastStepTime;
    uint16_t accel;
    void setCurSpeed(uint16_t stepsPerSec);
    void calcDecel(void);
	void timerInterrupt(void);
	uint32_t getStepInterval(uint32_t uStepsPerSec);
};

#endif

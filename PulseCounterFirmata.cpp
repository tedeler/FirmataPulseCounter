/*
  PulseCounterFirmata.cpp - Firmata library
*/

#include <ConfigurableFirmata.h>
#include "PulseCounterFirmata.h"

#define DECODE28BIT(p) (((uint32_t) (p)[0]) << 21) + (((uint32_t) (p)[1]) << 14) + (((uint32_t) (p)[2]) << 7) + (p)[3]


PulseCounterFirmata::PulseCounterFirmata()
{
}

boolean PulseCounterFirmata::handleSysex(byte command, byte argc, byte* argv)
{
    if(command != PULSECOUNTER_DATA)
        return false;

    byte pulsecounterCommand = argv[0];
    
    if(pulsecounterCommand == PULSECOUNTER_ATTACH) {
        byte pulseCntNum, pin, polarity;
        uint32_t minPauseBefore_us, minPulseLength_us, maxPulseLength_us;

        pulseCntNum = argv[1];
        pin = argv[2];
        polarity = argv[3];

        minPauseBefore_us = DECODE28BIT(argv+4);
        minPulseLength_us = DECODE28BIT(argv+8);
        maxPulseLength_us = DECODE28BIT(argv+12);

        if (pulseCntNum > MAXPULSECOUNTER)
            return true;

        this->counter[pulseCntNum].done();
        Firmata.setPinMode(pin, PIN_MODE_PULSECOUNTER);
        this->counter[pulseCntNum].init(pin, polarity, minPauseBefore_us, minPulseLength_us, maxPulseLength_us);
    }

    if(pulsecounterCommand == PULSECOUNTER_RESET_COUNTER) {
        byte pulseCntNum;
        pulseCntNum = argv[1];
        if(pulseCntNum > MAXPULSECOUNTER)
            return true;

        this->counter[pulseCntNum].resetCounter();
    }

    if(pulsecounterCommand == PULSECOUNTER_DETACH) {
        byte pulseCntNum;
        pulseCntNum = argv[1];
        if(pulseCntNum > MAXPULSECOUNTER)
            return true;

        this->counter[pulseCntNum].done();
    }


    if(pulsecounterCommand == PULSECOUNTER_REPORT) {
        byte pulseCntNum;
        pulseCntNum = argv[1];
        if(pulseCntNum > MAXPULSECOUNTER)
            return true;
            
        this->counter[pulseCntNum].setChangedFlag();
    }

    return true;
}

void PulseCounterFirmata::_report28BitValue(uint32_t value) {
    for(int i=0; i<4; i++) 
        Firmata.write(  (value >> (i*7)) & 0x7F );
}

void PulseCounterFirmata::report(void)
{
    for(int i=0; i < MAXPULSECOUNTER; i++) {
        if(!this->counter[i].isActive())
            continue;
        if(!this->counter[i].hasChanged())
            continue;

        uint32_t cnt_shortPause, cnt_shortPulse, cnt_longPulse, cnt_pulse;
        this->counter[i].getCounterValues(cnt_shortPause, cnt_shortPulse, cnt_longPulse, cnt_pulse);
        uint32_t pulseLenght, pauseLength;
        this->counter[i].getPulseInfo(&pulseLenght, &pauseLength);

        Firmata.write(START_SYSEX);
        Firmata.write(PULSECOUNTER_DATA);
        Firmata.write((byte) i);
        _report28BitValue(cnt_shortPause);
        _report28BitValue(cnt_shortPulse);
        _report28BitValue(cnt_longPulse);
        _report28BitValue(cnt_pulse);
        _report28BitValue(pulseLenght);
        _report28BitValue(pauseLength);
        Firmata.write(END_SYSEX);
    }
}

boolean PulseCounterFirmata::handlePinMode(byte pin, int mode)
{
    return mode == PIN_MODE_PULSECOUNTER;
}

void PulseCounterFirmata::handleCapability(byte pin)
{
    #if (ARDUINO_SAMD_VARIANT_COMPLIANCE >= 10606)
    EExt_Interrupts PinIntCapability = g_APinDescription[pin].ulExtInt;
    #else
    EExt_Interrupts in = digitalPinToInterrupt(pin);
    #endif 

    if (!IS_PIN_DIGITAL(pin))
        return;

    if (PinIntCapability == NOT_AN_INTERRUPT || PinIntCapability == EXTERNAL_INT_NMI)
        return;

    Firmata.write((byte)PIN_MODE_PULSECOUNTER);
    Firmata.write(1);                            
}

void PulseCounterFirmata::reset()
{
    for(int i=0; i < MAXPULSECOUNTER; i++) {
        this->counter[i].done();
    }
}

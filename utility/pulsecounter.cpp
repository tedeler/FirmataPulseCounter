#include <Arduino.h>
#include "pulsecounter.h"


int PulseCounter::nextID = 0;
PulseCounter *PulseCounter::PC[2];

void ISR_S0IN1() {
    PulseCounter::PC[0]->pinChangedInterrupt();
}
void ISR_S0IN2() {
    PulseCounter::PC[1]->pinChangedInterrupt();
}


PulseCounter::PulseCounter() 
{
    this->myID = nextID++;
    this->PC[this->myID] = this;
    this->active = false;
}

void PulseCounter::init(byte pin, byte polarity, uint32_t minPauseBefore_us, uint32_t minPulseLength_us, uint32_t maxPulseLength_us) 
{
    pinMode(pin, INPUT);
    this->pin = pin;
    this->polarity = polarity;
    this->minPauseBefore_us = minPauseBefore_us;
    this->minPulseLength_us = minPulseLength_us;
    this->maxPulseLength_us = maxPulseLength_us;
    switch(this->myID) {
        case 0:
            attachInterrupt(pin, ISR_S0IN1, CHANGE);
            break;
        case 1:
            attachInterrupt(pin, ISR_S0IN2, CHANGE);
            break;
    }
    this->resetCounter();
    this->active = true;
}

void PulseCounter::done() {
    if(!this->active)
        return;

    detachInterrupt(this->pin);
    this->active = false;
}

void PulseCounter::pinChangedInterrupt() {
    auto value = digitalRead(this->pin);
    int pulsestate = (value==polarity);

    if(pulsestate && !this->pulsestate){
        pulsestart = micros();
        _pauseLength = pulsestart - pulseend;
    }
    if(!pulsestate && this->pulsestate){
        pulseend = micros();
        pulseLength = pulseend - pulsestart;
        pauseLength = _pauseLength;
        this->count();
    }

    this->pulsestate = pulsestate;
}

void PulseCounter::resetCounter() {
    cnt_shortPulse = cnt_shortPause = cnt_longPulse = cnt_pulse = 0;
    counterChanged = true;
}

void PulseCounter::getCounterValues(uint32_t &cnt_shortPause, uint32_t &cnt_shortPulse,
                    uint32_t &cnt_longPulse, uint32_t &cnt_pulse)
{
    cnt_shortPause = this->cnt_shortPause;
    cnt_shortPulse = this->cnt_shortPulse;
    cnt_longPulse = this->cnt_longPulse;
    cnt_pulse = this->cnt_pulse;
    counterChanged = false;
}

void PulseCounter::setCounterValues(uint32_t cnt_shortPause, uint32_t cnt_shortPulse,
                    uint32_t cnt_longPulse, uint32_t cnt_pulse)
{
    this->cnt_shortPause = cnt_shortPause;
    this->cnt_shortPulse = cnt_shortPulse;
    this->cnt_longPulse = cnt_longPulse;
    this->cnt_pulse = cnt_pulse;
}

void PulseCounter::getPulseInfo(uint32_t *pulseLength, uint32_t *pauseLength) {
    *pulseLength = this->pulseLength;
    *pauseLength = this->pauseLength;
}

void PulseCounter::count() {
    if (pauseLength < minPauseBefore_us)
        cnt_shortPause++;
    else if (pulseLength < minPulseLength_us)
        cnt_shortPulse++;
    else if (pulseLength > maxPulseLength_us)
        cnt_longPulse++;
    else
        cnt_pulse++;    

    counterChanged = true;
}

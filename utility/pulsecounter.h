#include <Arduino.h>

#ifndef __PULSECOUNTER_H__
#define __PULSECOUNTER_H__

#define MAXPULSECOUNTER 2

void ISR_S0IN1();
void ISR_S0IN2();

class PulseCounter {
    public:
    static PulseCounter *PC[2];
    static int nextID;

    PulseCounter();
    void init(byte pin, byte polarity, uint32_t minPauseBefore_us, uint32_t minPulseLength_us, uint32_t maxPulseLength_us);
    void done();
    void pinChangedInterrupt();
    void resetCounter();
    bool hasChanged() {return counterChanged && active;}
    void setChangedFlag() {counterChanged = true;}
    byte getPinNo() {return pin;}
    bool isActive() {return active;}
    void getCounterValues(uint32_t &cnt_shortPause, uint32_t &cnt_shortPulse,
                     uint32_t &cnt_longPulse, uint32_t &cnt_pulse);

    void setCounterValues(uint32_t cnt_shortPause, uint32_t cnt_shortPulse,
                     uint32_t cnt_longPulse, uint32_t cnt_pulse);
    void getPulseInfo(uint32_t *pulseLength, uint32_t *pauseLength);

    private:
    void count();

    byte pin;
    byte polarity;
    
    bool active;
    int pulsestate;
    int myID;
    uint32_t pulsestart, pulseend;
    uint32_t pulseLength, pauseLength, _pauseLength;
    bool counterChanged;

    uint32_t minPauseBefore_us, minPulseLength_us, maxPulseLength_us;
    uint32_t cnt_shortPause, cnt_shortPulse, cnt_longPulse, cnt_pulse;
};

#endif //__PULSECOUNTER_H__

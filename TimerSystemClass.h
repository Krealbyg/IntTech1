//TimerSystemClass.h
//Timer class setup for spray delays
#ifndef TimerSystemClass_h
#define TimerSystemClass_h
#include <Arduino.h>

class TIMESYSTEMCLASS{
  private:
    int setDelay;
    bool counting;
    unsigned long startTime;

  public:
    TIMESYSTEMCLASS (int customDelay)
    bool isBusy();
    void reset();
    void miliCurrent();

};
#endif











